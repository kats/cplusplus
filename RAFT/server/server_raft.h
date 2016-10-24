#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <thread>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "../replicas/replicas.h"
#include "../store.h"
#include "../timer.h"
#include "../tcp/tcpstream.h"
#include "../tcp/tcpacceptor.h"
#include "../tcp/tcpconnector.h"
#include "../tcp/tcpexception.h"
#include "../rpc/server_proto_operation.h"
#include "../rpc/server_proto_parser.h"
#include "server_raft_receiver.h"
#include "server_raft_sender.h"

template<typename TK, typename TV>
class server_raft
{
    friend class server_proto_undef<TK, TV>;
    friend class server_proto_stop<TK, TV>;
    friend class server_proto_vote_init<TK, TV>;
    friend class server_proto_vote<TK, TV>;
    friend class server_proto_vote_for<TK, TV>;
    friend class server_proto_heartbeat<TK, TV>;
    friend class server_proto_get<TK, TV>;
    friend class server_proto_del<TK, TV>;
    friend class server_proto_set<TK, TV>;
    friend class server_proto_syncset<TK, TV>;
    friend class server_proto_syncset_for<TK, TV>;

    int                             m_term;
    int                             m_status;
    int                             m_last_applied_index;
    bool                            m_started;
    replica                         m_self;
    replica                         m_leader;
    replicas                        m_replicas;
    store<TK, TV>                   m_store;
    server_raft_receiver<TK, TV>    m_receiver;
    server_raft_sender              m_sender;

    std::thread     heartBeatSender;
    std::thread     heartBeatWaiter;

    std::mutex      m_mtx;
    timer           m_timer;

    std::unique_ptr<replica>                    m_vote_for_replica;
    std::unique_ptr<int>                        m_vote_for_term;
    std::unordered_map<int, std::vector<bool>>  m_voting;

    std::unordered_map<int, std::tuple<std::shared_ptr<TCPStream>, TK, TV, std::vector<bool>>> m_syncs;

    void startHeartBeatSending();
    void startHeartBeatWaiting();
    void startRequstHandling();

    void handler(std::shared_ptr<TCPStream>& stream);
public:
    enum serverStatus
    {
        follower = 0,
        candidate = 1,
        leader = 2,
    };

    server_raft(const replica& self, std::string replicaspath, std::string logpath, bool restore = false);
    ~server_raft();

    void start();
};

template<typename TK, typename TV>
server_raft<TK, TV>::server_raft(const replica& self, std::string replicaspath, std::string logpath, bool restore)
    : m_term(1), m_status(serverStatus::follower), m_started(false),
      m_self(self), m_leader(self), m_replicas(replicaspath),
      m_store(logpath, restore), m_receiver(self), m_sender(self)
{
    std::cout << "!!!!!NOW FOLLOWER!!!" << std::endl;
    if (restore)
        m_store.showStore();
    m_last_applied_index = restore ? m_store.lastAppliedIndex() : 0;
}

template<typename TK, typename TV>
server_raft<TK, TV>::~server_raft()
{
    if (heartBeatSender.joinable()) heartBeatSender.join();
    if (heartBeatWaiter.joinable()) heartBeatWaiter.join();
}

template<typename TK, typename TV>
void server_raft<TK, TV>::start()
{
    if (m_started)
        return;

    m_started = true;

    m_receiver.startRequestReciving([this](std::shared_ptr<TCPStream>& stream){ handler(stream); });
    m_sender.startRequestSending();

    startHeartBeatWaiting();
    startHeartBeatSending();
}

template<typename TK, typename TV>
void server_raft<TK, TV>::handler(std::shared_ptr<TCPStream>& stream)
{
    if (stream)
    {
        std::string request;
        stream >> request;
        std::cout << "<<< " << request << std::endl;
        server_proto_parser<TK, TV> parser;
        auto operation = parser.parse(request, stream);

        m_mtx.lock();
        if (m_started)
            operation->applyTo(this);
        m_mtx.unlock();
    }
    else throw TCPException("stream is null");
}


template<typename TK, typename TV>
void server_raft<TK, TV>::startHeartBeatSending()
{
    if (!heartBeatSender.joinable())
    {
        heartBeatSender = std::thread([this](){
            while(m_started)
            {
                m_mtx.lock();
                if (m_status == serverStatus::leader &&
                    m_started)
                {
                    std::stringstream heartBeatMessage;
                    heartBeatMessage << "hb " << m_self << " " << m_term << " " << m_last_applied_index;
                    m_sender.sendRequest(m_replicas, m_self, heartBeatMessage.str());
                    m_mtx.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(m_timer.getSleepTimeoutMs()));
                }
                else m_mtx.unlock();
                std::this_thread::yield();
            }
        });
    }
}

template<typename TK, typename TV>
void server_raft<TK, TV>::startHeartBeatWaiting()
{
    m_timer.start();
    if (!heartBeatWaiter.joinable())
    {
        heartBeatWaiter = std::thread([this](){
            while(m_started)
            {
                m_mtx.lock();
                if(m_status == serverStatus::follower && 
                    m_started && m_timer.isExpired())
                {
                    m_timer.clear();
                    m_sender.sendRequest(m_self, "vote_init");
                }
                m_mtx.unlock();
                std::this_thread::yield();
            }
        });
    }
}

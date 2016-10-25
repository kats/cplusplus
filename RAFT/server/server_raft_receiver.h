#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <utility> 
#include <thread>
#include "../replicas/replica.h"
#include "../tcp/tcpacceptor.h"
#include "../thread_safe_queue.h"


template<typename TK, typename TV>
class server_raft_receiver
{
    typedef std::shared_ptr<TCPStream> spt_strm;
    typedef thread_safe_queue<spt_strm> ts_queue;

    bool                                m_started;
    std::thread                         m_handler;
    TCPAcceptor                         m_acceptor;
    ts_queue                            m_queue;

    void startRequestReciving()
    {
        m_started = m_acceptor.start();
        if (!m_handler.joinable())
        {
            m_handler = std::thread([this](){
                while (m_started)
                {
                    auto stream = m_acceptor.accept();
                    std::cout << "accepted" << std::endl;
                    m_queue.push(stream);
                    std::this_thread::yield();
                }
            });
        }
    }
public:
    server_raft_receiver(const replica& self)
        : m_started(false), m_acceptor(self)
    {
        startRequestReciving();
    }

    ~server_raft_receiver()
    {
        if(m_handler.joinable()) m_handler.join();
    }


    std::pair<bool, std::shared_ptr<TCPStream>> try_get_stream()
    {
        if (m_started)
        {
            auto front_p = m_queue.try_pop();
            if (front_p.first)
            {
                return std::make_pair(true, front_p.second);
            }
        }
        return std::make_pair(false, nullptr);
    }

    void delay_stream_later(std::shared_ptr<TCPStream>& stream)
    {
        m_queue.push(stream);
    }

    void stop()
    {
        m_started = false;
    }
};
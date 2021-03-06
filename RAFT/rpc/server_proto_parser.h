#pragma once

#include <memory>
#include <string>
#include <sstream>
#include "server_proto_operation.h"

template<typename TK, typename TV>
class server_proto_parser
{
    typedef server_proto_operation<TK, TV> spo;
    typedef std::shared_ptr<spo> spt_spo;
    typedef std::shared_ptr<TCPStream> spt_str;

public:
    spt_spo parse(spt_str& stream) throw(TCPException)
    {
        std::string request;
        stream >> request;
        std::cout << "<<< " << request << std::endl;

        std::stringstream requestStream(request);
        std::string method;
        requestStream >> method;

        if (method == "stop")           return spt_spo((spo*)new server_proto_stop<TK, TV>(stream));
        if (method == "vote_init")      return spt_spo((spo*)new server_proto_vote_init<TK, TV>());
        if (method == "vote")           return spt_spo((spo*)new server_proto_vote<TK, TV>(requestStream));
        if (method == "vote_for")       return spt_spo((spo*)new server_proto_vote_for<TK, TV>(requestStream));
        if (method == "hb")             return spt_spo((spo*)new server_proto_heartbeat<TK, TV>(requestStream));
        if (method == "hb_for")         return spt_spo((spo*)new server_proto_heartbeat_for<TK, TV>(requestStream));
        if (method == "get")            return spt_spo((spo*)new server_proto_get<TK, TV>(requestStream, stream));
        if (method == "del")            return spt_spo((spo*)new server_proto_del<TK, TV>(requestStream, stream));
        if (method == "set")            return spt_spo((spo*)new server_proto_set<TK, TV>(requestStream, stream));
        if (method == "syncset")        return spt_spo((spo*)new server_proto_syncset<TK, TV>(requestStream));
        if (method == "syncset_for")    return spt_spo((spo*)new server_proto_syncset_for<TK, TV>(requestStream));
                                        return spt_spo((spo*)new server_proto_undef<TK, TV>(stream));
    }
};
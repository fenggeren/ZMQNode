//
//  ZMQServerNodeBase.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/4.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <unordered_map>
#include <czmq.h>
#include <set>
#include <map>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "ZMQNode.hpp"
#include "CPGFuncHelper.hpp"

/*
 有效组合
 PUB and SUB
 REQ and REP
 REQ and ROUTER (take care, REQ inserts an extra null frame)
 DEALER and REP (take care, REP assumes a null frame)
 DEALER and ROUTER
 DEALER and DEALER
 ROUTER and ROUTER
 PUSH and PULL
 PAIR and PAIR
 */

class ZMQServerNodeBase
{
public:
    ZMQServerNodeBase(CPGServerType serviceType)
    : serviceType_(serviceType)
    {
        reactor_ = std::make_shared<ZMQReactor>();
        masterClient_ = std::make_shared<ZMQMasterClient>(reactor_, serviceType_);
    }
    
    void start();
    
    template<int TYPE>
    void messageRead(zsock_t* sock)
    {
        int sourceType = zsock_type(sock);
        zmsg_t* msg = zmsg_recv(sock);
        
        std::string extra;
        // PUB -> SUB | DEALER -> ROUTER    all has a first id frame
        if (TYPE == ZMQ_SUB || TYPE == ZMQ_ROUTER ||
            (TYPE == ZMQ_REQ &&  sourceType != ZMQ_REP))
        {
            zframe_t* first = zmsg_first(msg);
            extra = std::string(reinterpret_cast<char*>(zframe_data(first)),
                                  zframe_size(first));
        }
        
        readData(extra, msg);
        zmsg_destroy(&msg);
    }
    
protected:
    
    void readData(const std::string& extra, zmsg_t* msg);
    
    virtual void handleData(const PacketHead& head,
                    char* data, size_t len);
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services);

    virtual std::list<ServiceProfile> allServiceProfiles() { return {}; };
protected:
    CPGServerType serviceType_;
    std::shared_ptr<ZMQReactor> reactor_;
    std::shared_ptr<ZMQMasterClient> masterClient_;
    std::string uuid;
};



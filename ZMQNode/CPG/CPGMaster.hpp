//
//  CPGMaster.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <unordered_map>
#include <czmq.h>
#include "Packet.h"
#include "Defines.h"
#include <thread>
#include <memory>
#include <functional>
#include <list>
#include "ZMQReactor.hpp"
#include "server.pb.h"

class CPGMaster
{
public:
    
    CPGMaster()
    : router_(zsock_new(ZMQ_ROUTER))
    , pub_(zsock_new(ZMQ_PUB))
    , reactor_()
    {
        zsock_bind(router_, "tcp://*:%d", MASTER_ROUTER_PORT);
        zsock_bind(pub_, "tcp://*:%d", MASTER_PUB_PORT);
    }
    
    ~CPGMaster()
    {
        
    }
    
    void start()
    {
        reactor_.addSocket(router_, std::bind(&CPGMaster::messageIn, this, std::placeholders::_1));
        reactor_.asyncLoop();
    }
    
private:
    
    void messageIn(zsock_t* sock);
    
    void parseData(const std::string& source,
                   const PacketHead& head,
                   char* data, size_t len);
    
    // gateWay需要连接的 services
    void sendGateWayConnectors(const std::string& source);
 
    void registerService(const std::string& source, const char* data, size_t len);
    
    std::list<ServiceProfile> requiredConnectService(int serviceType);
    
private:
    zsock_t* router_;
    zsock_t* pub_;
    ZMQReactor reactor_;
    std::unordered_map<int, ServiceProfile> services_;
};


















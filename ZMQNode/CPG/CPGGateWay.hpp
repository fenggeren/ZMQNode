//
//  CPGGateWay.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <czmq.h>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "ZMQNode.hpp"
#include "CPGFuncHelper.hpp"

class CPGGateWay
{
public:
    
    CPGGateWay()
    : serviceType_(kGateWay)
    {
        count++;
        uuid = std::string("GS-") + CPGFuncHelper::localIP() + "-" + std::to_string(count);
        reactor_ = std::make_shared<ZMQReactor>();
        masterClient_ = std::make_shared<ZMQMasterClient>(reactor_, serviceType_);
    }
     
    
    void start();
     
    void parseClientData(const PacketHead& head,
                         char* data, size_t len);
    
    void registerServiceCallback(const PacketHead& head,
                                 char* data, size_t len);
    
    
private: 
    
    void messageRead(zsock_t* sock);
    
    void newServiceProfile(const std::list<ServiceProfile>& services);
private:
    zsock_t* loginDealer_;
    zsock_t* matchPull_;
    
    CPGServerType serviceType_;
    std::shared_ptr<ZMQReactor> reactor_;
    std::shared_ptr<ZMQMasterClient> masterClient_;
    std::unordered_multimap<int, zsock_t*> matchDealers_;
    
    
    std::string uuid;
    
    static int count;
};






















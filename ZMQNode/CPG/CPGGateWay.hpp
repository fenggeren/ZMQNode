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

class CPGGateWay
{
public:
    
    CPGGateWay()
    : reactor_()
    {
        
    }
    
    void registerMaster()
    {
        masterDealer_ = zsock_new(ZMQ_DEALER);
        zsock_connect(masterDealer_, "%s", MASTER.data());
        reactor_.addSocket(masterDealer_, [](zsock_t* sock){
            
        });
        reactor_.asyncLoop();
    }
    
private:
    zsock_t* masterDealer_;
    zsock_t* loginDealer_;
    zsock_t* matchPull_;
    ZMQReactor reactor_;
    std::unordered_multimap<int, zsock_t*> matchDealers_;
};






















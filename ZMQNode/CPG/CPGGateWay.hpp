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

class CPGGateWay
{
public:
    
    void registerMaster()
    {
        masterDealer_ = zsock_new(ZMQ_DEALER);
        zsock_connect(masterDealer_, "%s", MASTER.data());
    }
    
private:
    zsock_t* masterDealer_;
    
    zsock_t* loginDealer_;
    zsock_t* matchPull_;
    std::unordered_multimap<int, zsock_t*> matchDealers_;
};






















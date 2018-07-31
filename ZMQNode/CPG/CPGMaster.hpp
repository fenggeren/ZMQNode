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
#include "ZMQReactor.hpp"

class CPGMaster
{
public:
    
    CPGMaster()
    : router_(zsock_new(ZMQ_ROUTER))
    , reactor_()
    {
        zsock_bind(router_, "tcp://*:%d", MASTERPORT);
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
    
    void messageIn(zsock_t* sock)
    {
        
    }
    
private:
    zsock_t* router_;
    ZMQReactor reactor_;
    std::unordered_map<int, ServiceProfile> services_;
};


















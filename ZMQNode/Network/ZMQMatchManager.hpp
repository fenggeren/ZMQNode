//
//  ZMQMatchManager.hpp
//  ZMQNode
//
//  Created by huanao on 2018/9/28.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include <czmq.h>
#include <string>
#include "ZMQReactor.hpp"
#include "ZMQNetworkDefine.h"


namespace ZGW
{
    class ZMQMatchManager
    {
    public:
        
        ZMQMatchManager()
        {
            sub_ = zsock_new(ZMQ_PUB);
            push_ = zsock_new(ZMQ_PUSH);
        }
        
        void start()
        {
            zsock_connect(sub_, "tcp://%s:%d", GATEWAY_PUB_IP, GATEWAY_PUB_PORT);
            zsock_connect(push_, "tcp://%s:%d", GATEWAY_PULL_IP, GATEWAY_PULL_PORT);
            reactor_.addSocket(sub_, [](zsock_t* sock){
                zmsg_t* msg = zmsg_recv(sock);
                zmsg_print(msg);
                zmsg_destroy(&msg);
            });
            reactor_.loop();
        }
        
        
    private:
        zsock_t* sub_;
        zsock_t* push_;
        
        ZMQReactor reactor_;
    };
}

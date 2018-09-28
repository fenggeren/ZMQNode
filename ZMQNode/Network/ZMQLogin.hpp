//
//  ZMQLogin.hpp
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
    class ZMQLogin
    {
    public:
        
        ZMQLogin()
        {
            router_ = zsock_new(ZMQ_ROUTER);
        }
        
        void start()
        {
            zsock_bind(router_, "tcp://*:%d", LOGIN_ROUTER_PORT);
            reactor_.addSocket(router_, [](zsock_t* sock){
                zmsg_t* msg = zmsg_recv(sock);
                zmsg_print(msg);
                zmsg_destroy(&msg);
            });
            reactor_.loop();
        }
        
    public:

        
        
    private:
        zsock_t* router_;
        ZMQReactor reactor_;
    };
}









//
//  ZMQGateWay.hpp
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
#include <map>

namespace ZGW
{
    
class ZMQGateWay
{
public:
    
    ZMQGateWay()
    {
        pub_ = zsock_new(ZMQ_PUB);
        pull_ = zsock_new(ZMQ_PULL);
        dealer_ = zsock_new(ZMQ_DEALER);
    }
    
    void start()
    {
        zsock_bind(pub_, "tcp://*:%d", GATEWAY_PUB_PORT);
        zsock_bind(pull_, "tcp://*:%d", GATEWAY_PULL_PORT);
        zsock_connect(dealer_, "tcp://127.0.0.1:%d", LOGIN_ROUTER_PORT);
        reactor_.addSocket(dealer_, [](zsock_t* sock){
            
        });
        reactor_.addSocket(pull_, [](zsock_t* sock){
            zmsg_t* msg = zmsg_recv(sock);
            zmsg_print(msg);
            zmsg_destroy(&msg);
        });
        test();
        reactor_.loop();
    }
    
    void test()
    {
        reactor_.addTimer(3000, 1, [&]{
            while (true)
            {
                sendLoginRQ(123, "PING PONG");
                zclock_sleep(1);
                sendMatchListRQ(123);
                zclock_sleep(1);
                sendMatchJoinRQ(123, 30001);
                zclock_sleep(1);
                sendMatchJoinRQ(123, 30001);
                zclock_sleep(1);
                sendMatchUnjoinRQ(123, 40001);
                zclock_sleep(1);
                sendMatchUnjoinRQ(123, 40001);
                zclock_sleep(1);
            }
        });
    }
    
public:
    void sendLoginRQ(int uid, const std::string& token);
    void sendMatchListRQ(int uid);
    void sendMatchJoinRQ(int uid, int mid);
    void sendMatchUnjoinRQ(int uid, int mid);

public:
    
    
private:
    zsock_t* dealer_;
    zsock_t* pub_;
    zsock_t* pull_;
    ZMQReactor reactor_;
};
    
}




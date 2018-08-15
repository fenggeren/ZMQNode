//
//  main.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include <iostream>
#include "ZMQReactor.hpp"
#include "CPGGateWay.hpp"
#include "CPGMaster.hpp"
#include "CPGLoginServer.hpp"
#include "CPGMatchManager.hpp"
#include "CPGMatchServer.hpp"
#include "dependences/Queue.hpp"

/*
 处理接收到的数据，
 如果是需要创建新sock，需要在主线程处理！！！
 引入aiso?
 */
void test()
{
    CPGMaster master;
    master.start();
    
    CPGLoginServer login;
    login.start();

    zclock_sleep(1000);
    
    std::vector<CPGGateWay> gateWays;
    for (int i = 0; i < 5; i++) {
        gateWays.push_back(CPGGateWay());
    }
    
    for(auto& gw : gateWays)
    {
        gw.start();
    }
    
    zclock_sleep(1000);
    
    CPGLoginServer login2;
    login2.start();
    
    CPGMatchManager mm;
    mm.start();
    
    CPGMatchServer ms;
    ms.start();
 
    zclock_sleep(3000);
    while (true)
    {
        int idx = rand() % gateWays.size();
        gateWays[idx].sendLoginRQ(123, "123456");
        gateWays[idx].sendMatchListRQ(123);
        gateWays[idx].sendMatchJoinRQ(123, 321);
        gateWays[idx].sendMatchUnjoinRQ(123, 321);
    }
}

void testPub()
{
    CPGMaster master;
    master.start();
    
    zsock_t* sub = zsock_new(ZMQ_SUB);
    zsock_connect(sub, "tcp://localhost:5680");
    zsock_set_subscribe(sub, "");
    
    ZMQReactor reactor;
    reactor.addSocket(sub, [](zsock_t* sock){
        zmsg_t* msg = zmsg_recv(sock);
        zmsg_print(msg);
        zmsg_destroy(&msg);
    });
    
    reactor.asyncLoop();
    
    while (true)
    {
        zsock_connect(sub, "tcp://localhost:5680");
        master.testPub();
        zmq_sleep(2);
    }

}

int main(int argc, const char * argv[]) {
    
    test();
//    test();
    
    return 0;
}







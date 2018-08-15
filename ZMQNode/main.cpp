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
 sock非线程安全，
 多线程共享sock，会造成死锁！！！
 */
void test()
{
    Queue::MainQueue::MainQueueInit();
    CPGMaster master;
    master.start();
    
    CPGLoginServer login;
    login.start();

    zclock_sleep(1000);
    
    std::vector<CPGGateWay> gateWays;
    for (int i = 0; i < 1; i++) {
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
    
    gMainQueue.dispatchAfter(3, [&]{
        while (true)
        {
            int idx = rand() % gateWays.size();
            gateWays[idx].sendLoginRQ(123, "123456");
//            gateWays[idx].sendMatchListRQ(123);
//            gateWays[idx].sendMatchJoinRQ(123, 321);
//            gateWays[idx].sendMatchUnjoinRQ(123, 321);
            zclock_sleep(1);
        }
    });
 
    gMainQueue.runMainThread();
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

void testQueue()
{
    gMainQueue.MainQueueInit();
    
    gMainQueue.dispatchAfter(1, [&]{
        int demo = 1;
        while (true)
        {
            gMainQueue.dispatch([&]{
                demo++;
                printf("demo:     %d\n", demo);
            });
        }
    });
    
    gMainQueue.runMainThread();
}

int main(int argc, const char * argv[]) {
    
//    test();
    test();
//    testQueue();
    
    return 0;
}







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

int main(int argc, const char * argv[]) {
    
    CPGMaster master;
    master.start();
    
    CPGGateWay gateWay;
    gateWay.start();
    gateWay.registerMaster();
    
    
    while (true)
    {
        zclock_sleep(1000);
    }
    
    return 0;
}







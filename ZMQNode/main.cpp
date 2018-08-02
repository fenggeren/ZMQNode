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

int main(int argc, const char * argv[]) {
    
    CPGMaster master;
    master.start();
    
    CPGLoginServer login;
    login.start();
    
    zclock_sleep(5);
    
    std::vector<CPGGateWay> gateWays;
    for (int i = 0; i < 1; i++) {
        gateWays.push_back(CPGGateWay());
    }
    
    for(auto& gw : gateWays)
    {
        gw.start();
    }
 

    while (true)
    {
        zclock_sleep(1000);
    }
    
    return 0;
}







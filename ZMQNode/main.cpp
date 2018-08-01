//
//  main.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include <iostream>
#include "ZMQReactor.hpp"

int main(int argc, const char * argv[]) {
    
    
    ZMQReactor rector;
    
    rector.addTimer(1000, 0, []{
        printf("========\n");
    });
    
    rector.loop();
    
    
    return 0;
}

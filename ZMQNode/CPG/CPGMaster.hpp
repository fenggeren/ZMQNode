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


class CPGMaster
{
public:
    
    
    
private:
    zsock_t* router_;
    std::unordered_map<int, ServiceProfile> services_;
};


















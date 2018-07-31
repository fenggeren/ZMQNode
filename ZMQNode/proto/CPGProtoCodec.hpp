//
//  CPGProtoCodec.hpp
//  ZeroMQDemo
//
//  Created by huanao on 2018/7/25.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <stddef.h>
#include <memory>
#include "server.pb.h"

using GMessagePtr = std::shared_ptr<google::protobuf::Message>;




class CPGProtoCodec
{
public:
    
    
    
    GMessagePtr parse(const char* buf, size_t len)
    {
        
        return nullptr;
    }
    
    
    
private:
    
};




















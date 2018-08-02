//
//  Defines.h
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once


// 心跳包时间   2s
#define kHeartbeatDuration 5

 

const static std::string MATCH_SERVER[] =
{
    "tcp://127.0.0.1:5560",
    "tcp://127.0.0.1:5561",
    "tcp://127.0.0.1:5562",
    "tcp://127.0.0.1:5563",
};
const static std::string MATCH_MANAGER[] =
{
    "tcp://127.0.0.1:5570",
    "tcp://127.0.0.1:5571",
};
const static std::string LOGIN_SERVER[] =
{
    "tcp://127.0.0.1:5580",
    "tcp://127.0.0.1:5581",
};
const static std::string MASTER_ROUTER_ENDPOINT = "tcp://127.0.0.1:5688";
const static std::string MASTER_PUB_ENDPOINT = "tcp://127.0.0.1:5689";
const static int MASTER_ROUTER_PORT = 5688;
const static int MASTER_PUB_PORT = 5680;


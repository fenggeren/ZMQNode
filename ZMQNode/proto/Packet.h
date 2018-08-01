//
//  Packet.h
//  ZeroMQDemo
//
//  Created by huanao on 2018/7/25.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <stddef.h>
#include <vector>
#include <string>
#include <unordered_map>


struct PacketCommand
{
    unsigned short mainCmdID;
    unsigned short subCmdID;
};
struct PacketInfo
{
    unsigned short dataKind;
    unsigned short checkCode;
    unsigned packetSize;
};

struct PacketHead
{
    PacketCommand command;
    PacketInfo info;
};



const static std::vector<std::string> MATCH_SERVER =
{
    "tcp://127.0.0.1:5560",
    "tcp://127.0.0.1:5561",
    "tcp://127.0.0.1:5562",
    "tcp://127.0.0.1:5563",
};
const static std::vector<std::string> MATCH_MANAGER =
{
    "tcp://127.0.0.1:5570",
    "tcp://127.0.0.1:5571",
};
const static std::vector<std::string> LOGIN_SERVER =
{
    "tcp://127.0.0.1:5580",
    "tcp://127.0.0.1:5581",
};
const static std::string MASTER = "tcp://127.0.0.1:5688";
const static int MASTER_ROUTER_PORT = 5688;
const static int MASTER_PUB_PORT = 5680;

enum CPGServerType
{
    kGateWay = 1 << 0,
    kMatchServer = 1 << 1,
    kMatchManager = 1 << 2,
    kLoginServer = 1 << 3,
    kMaster = 1 << 4,
};
 
struct ServiceProfile
{
    // 服务类型
    int serviceType{0};
    // zmq socket 类型
    int socketType{0};
    // 服务地址
    std::string addr;
};

struct ServiceConnectInfo
{
    int serviceType{0};
    int socketType{0};
};

// 服务节点 需要连接的其他服务
static const std::unordered_map<int, std::vector<ServiceConnectInfo>>
serviceConnectMap =
{
    {kGateWay, {
        {kLoginServer, ZMQ_ROUTER},
        {kMatchServer, ZMQ_PUSH},
        {kMatchServer, ZMQ_ROUTER},
        {kMatchManager, ZMQ_PUSH}
    }},
    {kMatchServer, {
        {kMatchServer, ZMQ_ROUTER}
    }}
};

///////// main command ///////////



///////// sub command ///////////

#define kSeviceRegisterRQ 10001
#define kSeviceRegisterRS 10002




















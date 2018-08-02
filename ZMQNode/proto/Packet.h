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
#include <zmq.h>

struct PacketCommand
{
    unsigned short mainCmdID;
    unsigned short subCmdID;
};
//struct PacketInfo
//{
//    unsigned short dataKind;
//    unsigned short checkCode;
//    unsigned packetSize;
//};
//
//struct PacketHead
//{
//    PacketCommand command;
//    PacketInfo info;
//};
using PacketHead = PacketCommand;


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
static std::unordered_map<int, std::vector<ServiceConnectInfo>>
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

#define kServiceHeartMsg                        10000

#define kSeviceRegisterRQ                       10001
#define kSeviceRegisterRS                       10002
#define kServicePublishNewServicesMsg           10003




















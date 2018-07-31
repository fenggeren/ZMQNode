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
const static int MASTERPORT = 5688;

enum CPGServerType
{
    kGateWay = 1,
    kMatchServer = 2,
    kMatchManager = 3,
    kLoginServer = 4,
    kMaster = 5,
};
 
struct ServiceProfile
{
    // 服务类型
    int serviceType;
    // zmq socket 类型
    int socketType;
    // 服务地址
    std::string addr;
};

///////// main command ///////////



///////// sub command ///////////

#define kSeviceRegisterRQ 10001
#define kSeviceRegisterRS 10002




















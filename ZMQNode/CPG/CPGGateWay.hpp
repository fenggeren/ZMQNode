//
//  CPGGateWay.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <czmq.h>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "ZMQNode.hpp"
#include "CPGFuncHelper.hpp"

class CPGGateWay
{
public:
    
    CPGGateWay()
    : reactor_()
    {
        count++;
        uuid = std::string("GS-") + CPGFuncHelper::localIP() + "-" + std::to_string(count);
    }
    
    // 向master注册服务
    void registerMaster();
    
    void start();
     
    void parseClientData(const PacketHead& head,
                         char* data, size_t len);
    
    void registerServiceCallback(const PacketHead& head,
                                 char* data, size_t len);
    
    
private:
    // master 服务 消息处理
    void masterServiceMessageCallback(zsock_t* sock);
    
    void clientMessageRead(zsock_t* sock);
private:
    CPGZMQMasterClient masterClient_;
    
    zsock_t* loginDealer_;
    zsock_t* matchPull_;
    ZMQReactor reactor_;
    std::unordered_multimap<int, zsock_t*> matchDealers_;
    
    int serviceType_;
    
    std::string uuid;
    
    static int count;
};






















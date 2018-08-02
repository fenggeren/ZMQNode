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
#include <thread>
#include <memory>
#include <functional>
#include <list>
#include "ZMQReactor.hpp"
#include "server.pb.h"

class CPGMaster
{
#define kSERVER_SCAN_HEATBEATS 20
public:
    
    CPGMaster()
    : router_(zsock_new(ZMQ_ROUTER))
    , pub_(zsock_new(ZMQ_PUB))
    , reactor_()
    {
        zsock_bind(router_, "tcp://*:%d", MASTER_ROUTER_PORT);
        zsock_bind(pub_, "tcp://*:%d", MASTER_PUB_PORT);
    }
    
    ~CPGMaster()
    {
        
    }
    
    void start()
    {
        reactor_.addSocket(router_, std::bind(&CPGMaster::messageHandle, this, std::placeholders::_1));
        reactor_.addTimer(kSERVER_SCAN_HEATBEATS * 1000, 0, std::bind(&CPGMaster::timerHeartbeatCheck, this));
        reactor_.asyncLoop();
    }
    
private:
    
    // ROUTER 消息处理
    void messageHandle(zsock_t* sock);
    
    void parseData(const std::string& source,
                   const PacketHead& head,
                   char* data, size_t len);
    
    // 心跳包检测， 是否有服务无响应
    void timerHeartbeatCheck();
    
private: // 消息处理
    struct ServiceNode;
    // 服务注册，  需要将服务节点的 服务sock信息发送过来，
    // <ZMQ_ROUTER, ZMQ_PUSH, ZMQ_PUB> 三种
    void registerService(const std::string& source, const char* data, size_t len);
    
    // 心跳更新
    void serviceHeart(const std::string& source, const char* data, size_t len);
    
    // gateWay需要连接的 services
    void sendNewNodeConnectors(const ServiceNode& node);
 

    std::vector<ServiceProfile>
        serviceProfiles(int serviceType, const std::string& uuid);
    // 有新服务注册， 发布新服务
    void publishNewService(const std::vector<ServiceProfile>& profile);
    // 有新服务注册， 获取新服务需要连接的其他服务节点。
    std::list<ServiceProfile> requiredConnectService(int serviceType);
    
    void addServiceNode(const ServiceNode& node, int serviceType);
private:
    zsock_t* router_;
    zsock_t* pub_;
    ZMQReactor reactor_;
    
    // 每个服务节点， 可能有0/N个 数据服务。 <ZMQ_ROUTER, ZMQ_PUSH, ZMQ_PUB>
    struct ServiceNode
    {
        int serviceType;
        time_t heartbeat; // 心跳包
        std::string uuid;
        std::vector<ServiceProfile> profiles;
    };
    // <serviceType, <服务节点uuid, 服务节点的服务vec>
    std::unordered_map<int, std::list<ServiceNode>> services_;
};


















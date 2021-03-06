//
//  CPGMaster.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGMaster.hpp"
#include "ZMQServerNodeBase.hpp"

#define SERVER_SESSION_ACTIVITY_TIMEOUT_SEC 60

void CPGMaster::messageHandle(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
//    zmsg_print(msg);
    
    size_t count = zmsg_size(msg);
    zframe_t* source = zmsg_first(msg);
    
    const std::string sourceStr(reinterpret_cast<char*>(zframe_data(source)),
                                zframe_size(source));
    
    for (int i = 0; i < count - 1; i+=2)
    {
        zframe_t* headFrame = zmsg_next(msg);
        PacketHead head;
        memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
        
        
        zframe_t* dataFrame = zmsg_next(msg);
        char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
        size_t size = zframe_size(dataFrame);
        
        parseData(sourceStr, head, data, size);
    }
    zmsg_destroy(&msg);
}

void CPGMaster::parseData(const std::string& source,
               const PacketHead& command,
               char* data, size_t len)
{
    switch (command.subCmdID) {
        case kServiceRegisterRQ:
            registerService(source, data, len);
            break;
        case kServiceHeartMsg:
            serviceHeart(source, data, len);
        default:
            break;
    }
}

void CPGMaster::timerHeartbeatCheck()
{
    time_t now = time(NULL);
    for (auto& pair : services_)
    {
        for (auto& node : pair.second)
        {
            if (node.heartbeat - now > SERVER_SESSION_ACTIVITY_TIMEOUT_SEC)
            {
                // 有服务超时响应， 处理之---
            }
        }
    }
}

// 需要连接的 services
void CPGMaster::sendNewNodeConnectors(const ServiceNode& node)
{
    CPG::ServiceRegisterRS rs;
    auto services = requiredConnectService(node.serviceType);
    for (auto& service : services)
    {
        auto profile = rs.add_connectservices();
        profile->set_servicetype(service.serviceType);
        profile->set_sockettype(service.socketType);
        profile->set_addr(service.addr);
    }
    zmsg_t* msg = zmsg_new();
    zmsg_addmem(msg, node.uuid.data(), node.uuid.size());
    CPGFuncHelper::appendZMsg(msg, kMaster, kServiceRegisterRS, rs);
    zmsg_send(&msg, router_);
    zmsg_destroy(&msg);
}

void CPGMaster::registerService(const std::string& source,
                                const char* data, size_t len)
{
    CPG::ServiceRegisterRQ rq;
    rq.ParseFromArray(data, (int)len);
    
    ServiceNode node;
    node.heartbeat = time(NULL);
    node.uuid = source;
    node.serviceType = rq.servicetype();
    printf("===========type: %s", source.data());
    for (auto& service : rq.services())
    {
        node.profiles.push_back({service.servicetype(), service.sockettype(), service.addr()});
    }
    addServiceNode(node, rq.servicetype());
    
    // cal
    sendNewNodeConnectors(node);
    // 将新加入的节点包含的所有 服务sock， 发布出去
    publishNewService(node.profiles);
}

void CPGMaster::serviceHeart(const std::string& uuid,
                             const char* data, size_t len)
{
    CPG::ServiceHeartbeatMsg msg;
    msg.ParseFromArray(data, len);
    int type = msg.servicetype();
    bool handle = false;
    
    auto iter = services_.find(type);
    if (iter != services_.end())
    {
        for (auto& node : iter->second)
        {
            if (node.uuid == uuid)
            {
                node.heartbeat = time(NULL);
                handle = true;
                break;
            }
        }
    }
    
    
    // TODO. 该服务还没有注册到MASTER   后续处理！
    if (handle == false)
    {
        
    }
}

std::vector<ServiceProfile>
CPGMaster::serviceProfiles(int serviceType, const std::string& uuid)
{
    auto nodeIters = services_.find(serviceType);
    if (nodeIters != services_.end())
    {
        for (auto& node : nodeIters->second)
        {
            if (node.uuid == uuid) {
                return node.profiles;
            }
        }
    }
    
    return {};
}
// 有新服务注册， 发布新服务
void CPGMaster::publishNewService(const std::vector<ServiceProfile>& services)
{
    std::map<int, std::vector<ServiceProfile>> profilesMap;
    for(auto& service : services)
    {
        profilesMap[service.serviceType].push_back(service);
    }
    
    for(auto& pair : profilesMap)
    {
        publishNewService(pair.first, pair.second);
    }
}

void CPGMaster::publishNewService(int serviceType,
                                  const std::vector<ServiceProfile>& services)
{
    CPG::ServicePublishNewServicesMsg publishMsg;
    
    for (auto& service : services)
    {
        auto profile = publishMsg.add_newservices();
        profile->set_servicetype(service.serviceType);
        profile->set_sockettype(service.socketType);
        profile->set_addr(service.addr);
    }
    
    std::string subTypeStr(std::to_string(serviceType));
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, subTypeStr.data());
    CPGFuncHelper::appendZMsg(msg, kMaster, kServicePublishNewServicesMsg, publishMsg); 
    zmsg_send(&msg, pub_);
    zmsg_destroy(&msg);
}

std::list<ServiceProfile>
CPGMaster::requiredConnectService(int serviceType)
{
    std::list<ServiceProfile> requiredServices;
    auto iter = serviceConnectMap.find(serviceType);
    if (iter == serviceConnectMap.end()) {
        return requiredServices;
    }
    
    auto& connectTypes = iter->second;
    for (auto& info : connectTypes)
    {
        // 所有服务节点
        auto nodes = services_[info.serviceType];
        for (auto& node : nodes)
        {
            // 相同服务类型的， 每个节点包含的数据服务sock
            for (auto& profile : node.profiles)
            {
                // 是否是 给定类型所需 连接的服务sock
                if (profile.socketType == info.socketType)
                {
                    requiredServices.push_back(profile);
                }
            }
        }
    }
    
    return requiredServices;
}

void CPGMaster::addServiceNode(const ServiceNode& node, int serviceType)
{
    auto& nodes = services_[serviceType];
    auto iter = std::find_if(nodes.begin(), nodes.end(),
                             [&](const ServiceNode& sn)
    {
        std::cout << "uuid:  ==="  << sn.uuid << "   " << node.uuid << std::endl;
        return sn.uuid == node.uuid;
    });
    
    if (iter != nodes.end())
    {
        nodes.erase(iter);
    }
    nodes.push_back(node);
    
    std::cout << "size: " << services_[serviceType].size() << std::endl;
}




//
//  CPGMaster.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGMaster.hpp"


void CPGMaster::messageHandle(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    zmsg_print(msg);
    
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
}

void CPGMaster::parseData(const std::string& source,
               const PacketHead& head,
               char* data, size_t len)
{
    PacketCommand command = head.command;
    switch (command.subCmdID) {
        case kSeviceRegisterRQ:
            registerService(source, data, len);
            break;
        case kServiceHeartMsg:
            
        default:
            break;
    }
}

// gateWay需要连接的 services
void CPGMaster::sendGateWayConnectors(const std::string& source)
{
    CPG::ServiceRegisterRS rs;
    
    auto services = requiredConnectService(kGateWay);
    for (auto& service : services)
    {
        auto profile = rs.add_connectservices();
        profile->set_servicetype(service.serviceType);
        profile->set_addr(service.addr);
    }
    
    requiredConnectService(kGateWay);
    
    PacketHead head;
    head.info = {0, 0, static_cast<unsigned int>(rs.ByteSize())};
    head.command = {kMaster, kSeviceRegisterRS};
    
    google::protobuf::uint8* szBuf = new google::protobuf::uint8[rs.ByteSize()];
    rs.SerializeWithCachedSizesToArray(szBuf);
    
    zmsg_t* msg = zmsg_new();
    zmsg_addmem(msg, source.data(), source.size());
    zmsg_addmem(msg, &head, sizeof(head));
    zmsg_addmem(msg, szBuf, head.info.packetSize);
    
    // send
    
    //        zmsg_destroy(&msg);
    delete []szBuf;
}

void CPGMaster::registerService(const std::string& source,
                                const char* data, size_t len)
{
    CPG::ServiceRegisterRQ rq;
    rq.ParseFromArray(data, (int)len);
    
    rq.PrintDebugString();
    
    auto& service = rq.service();
    
    ServiceProfile profile{service.servicetype(), ZMQ_DEALER,service.addr()};
    // cal
    
    sendGateWayConnectors(source);
}

void CPGMaster::serviceHeart(const std::string& uuid, const char* data, size_t len)
{
    
    int type = kGateWay;
    bool handle = false;
    
    auto iter = services_.find(type);
    if (iter != services_.end())
    {
        for (auto& node : iter->second)
        {
            if (node.uuid == uuid)
            {
                node.heart = time(NULL);
                handle = true;
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
void CPGMaster::publishNewService(const std::vector<ServiceProfile>& profile)
{
    
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





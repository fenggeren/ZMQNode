//
//  CPGMaster.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGMaster.hpp"


void CPGMaster::messageIn(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    size_t count = zmsg_size(msg);
    zframe_t* source = zmsg_first(msg);
    printf("server receive begin\n");
    
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
    printf("\nreceive end\n");
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

std::list<ServiceProfile>
CPGMaster::requiredConnectService(int serviceType)
{
    
    std::list<ServiceProfile> profiles;
    
    switch (serviceType) {
        case kGateWay:
        {
            
        }
            break;
            
        default:
            break;
    }
    
    return {
        {kLoginServer, ZMQ_ROUTER,"172.0.0.1:5558"},
        {kMatchServer, ZMQ_ROUTER, "172.0.0.1:5558"},
        {kMatchManager, ZMQ_ROUTER,"172.0.0.1:5558"}
    };
    }

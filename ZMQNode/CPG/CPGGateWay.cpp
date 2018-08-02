//
//  CPGGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGGateWay.hpp"

int CPGGateWay::count = 0;

void CPGGateWay::registerMaster()
{
    PacketHead head = {static_cast<unsigned short>(serviceType_), kSeviceRegisterRQ};
    
    CPG::ServiceRegisterRQ rq;
    rq.set_servicetype(serviceType_);
    google::protobuf::uint8* szBuf =
    new google::protobuf::uint8[rq.ByteSize()];
    rq.SerializeWithCachedSizesToArray(szBuf);
    
    zmsg_t* msg = zmsg_new();
    
    zmsg_addmem(msg, &head, sizeof(head));
    zmsg_addmem(msg, szBuf, rq.ByteSize());
    
    delete []szBuf;
}

void CPGGateWay::start()
{
    int connectService = 0;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        connectService |= service.serviceType;
    }
    masterClient_.connect(connectService);
    reactor_.addSocket(masterClient_.dealer(), std::bind(&CPGGateWay::clientMessageRead, this, std::placeholders::_1));
    reactor_.addSocket(masterClient_.sub(), std::bind(&CPGGateWay::clientMessageRead, this, std::placeholders::_1));
    reactor_.addTimer(kHeartbeatDuration, 0, std::bind(&CPGZMQMasterClient::sendHeartbeat, masterClient_, serviceType_));
    
    reactor_.asyncLoop();
}

void CPGGateWay::clientMessageRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    size_t count = zmsg_size(msg);
    printf("client receive begin\n");
    
    for (int i = 0; i < count - 1; i+=2)
    {
        zframe_t* headFrame = zmsg_next(msg);
        PacketHead head;
        memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
        
        
        zframe_t* dataFrame = zmsg_next(msg);
        char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
        size_t size = zframe_size(dataFrame);
        parseClientData(head, data, size);
    }
    printf("\nreceive end\n");
}

void CPGGateWay::parseClientData(const PacketHead& head,
                     char* data, size_t len)
{
    auto& command = head;
    switch (command.mainCmdID) {
        case kMaster:
        {
            switch (command.subCmdID) {
                case kSeviceRegisterRS:
                    registerServiceCallback(head, data, len);
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        default:
            break;
    }
}

void CPGGateWay::registerServiceCallback(const PacketHead& head, char* data, size_t len)
{
    CPG::ServiceRegisterRS rs;
    rs.ParseFromArray(data, len);
    
    rs.PrintDebugString();
}

 


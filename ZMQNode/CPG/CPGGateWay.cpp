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
    CPG::ServiceRegisterRQ rq;
    rq.set_servicetype(serviceType_);
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kSeviceRegisterRQ, rq);
    zmsg_send(&msg, masterClient_.dealer());
    zmsg_destroy(&msg);
}

void CPGGateWay::start()
{
    int connectService = 0;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        connectService |= service.serviceType;
    }
    masterClient_.connect(connectService, uuid);
    reactor_.addSocket(masterClient_.dealer(), std::bind(&CPGGateWay::clientMessageRead, this, std::placeholders::_1));
    reactor_.addSocket(masterClient_.sub(), std::bind(&CPGGateWay::clientMessageRead, this, std::placeholders::_1));
    reactor_.addTimer(kHeartbeatDuration * kSecondPerMilli, 0, std::bind(&CPGZMQMasterClient::sendHeartbeat, masterClient_, serviceType_));
    
    reactor_.asyncLoop();
}

void CPGGateWay::clientMessageRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    
    zmsg_print(msg);
    
    size_t count = zmsg_size(msg);
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
    zmsg_destroy(&msg);
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
    rs.ParseFromArray(data, (int)len);
    
    rs.PrintDebugString();
}








 


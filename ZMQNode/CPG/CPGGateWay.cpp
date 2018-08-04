//
//  CPGGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGGateWay.hpp"

int CPGGateWay::count = 0;


void CPGGateWay::start()
{
    std::set<std::string> subids;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        subids.insert(std::to_string(service.serviceType));
    }
    masterClient_->connect(subids, uuid);
    
    masterClient_->setNewServiceProfile(std::bind(&CPGGateWay::newServiceProfile, this, std::placeholders::_1));
    reactor_->asyncLoop();
    
    masterClient_->registerMaster({});
}

void CPGGateWay::newServiceProfile(const std::list<ServiceProfile>& services)
{
    for(auto& profile : services)
    {
        if (profile.serviceType == kLoginServer)
        {
            
        }
        else if (profile.serviceType == kMatchServer)
        {
            
        }
        else if (profile.serviceType == kMatchManager)
        {
            
        }
    }
}

void CPGGateWay::messageRead(zsock_t* sock)
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








 


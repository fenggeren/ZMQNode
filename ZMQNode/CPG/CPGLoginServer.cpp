//
//  CPGLoginServer.cpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGLoginServer.hpp"

int CPGLoginServer::count = 0;


void CPGLoginServer::start()
{
    std::set<std::string> subids;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        subids.insert(std::to_string(service.serviceType));
    }
    masterClient_->connect(subids, uuid);
    
    masterClient_->setNewServiceProfile(std::bind(&CPGLoginServer::newServiceProfile, this, std::placeholders::_1));
    
    int port = zsock_bind(server_.service_, "tcp://*:*");
    std::string routerId = uuid + ":" + std::to_string(port);
    zsock_set_identity(server_.service_, routerId.data());
    reactor_->addSocket(server_.service_, std::bind(&CPGLoginServer::messageRead, this, std::placeholders::_1));
    server_.addr = "tcp://" + CPGFuncHelper::localIP() + ":" + std::to_string(port);
    
    reactor_->asyncLoop();
    
    masterClient_->registerMaster({{serviceType_, ZMQ_ROUTER, server_.addr}});
}

void CPGLoginServer::newServiceProfile(const std::list<ServiceProfile>& services)
{
    
}


void CPGLoginServer::messageRead(zsock_t* sock)
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

void CPGLoginServer::parseClientData(const PacketHead& head,
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

void CPGLoginServer::registerServiceCallback(const PacketHead& head, char* data, size_t len)
{
    CPG::ServiceRegisterRS rs;
    rs.ParseFromArray(data, (int)len);
    
    rs.PrintDebugString();
}

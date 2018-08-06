//
//  CPGLoginServer.cpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGLoginServer.hpp"
 
void CPGLoginServer::newServiceProfile(const std::list<ServiceProfile>& services)
{
    
}

 
void CPGLoginServer::handleData(const PacketHead& head,
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

std::list<ServiceProfile> CPGLoginServer::allServiceProfiles()
{
    int port = zsock_bind(server_.service_, "tcp://*:*");
    std::string routerId = uuid + ":" + std::to_string(port);
    zsock_set_identity(server_.service_, routerId.data());
    reactor_->addSocket(server_.service_, std::bind(&CPGLoginServer::messageRead<ZMQ_ROUTER>, this, std::placeholders::_1));
    server_.addr = "tcp://" + CPGFuncHelper::localIP() + ":" + std::to_string(port);
    
    return {{server_.serviceType, zsock_type(server_.service_), server_.addr}};
}



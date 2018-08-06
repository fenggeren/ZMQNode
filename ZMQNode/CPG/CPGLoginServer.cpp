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
    // no.
}

 
void CPGLoginServer::handleData(const PacketHead& head,
                                 char* data, size_t len)
{
 
}

std::list<ServiceProfile> CPGLoginServer::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);    
    return {{serviceType_, ZMQ_ROUTER, std::get<1>(tuple)}};
}



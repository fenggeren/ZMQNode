//
//  CPGMatchServer.cpp
//  ZMQNode
//
//  Created by fgr on 2018/8/7.
//  Copyright © 2018年 fgr. All rights reserved.
//

#include "CPGMatchServer.hpp"

// 抽取出来单独的组件
void CPGMatchServer::newServiceProfile(const std::list<ServiceProfile>& services)
{
    for(auto& profile : services)
    {
        std::cout << "new service: "
        << "type: " << profile.serviceType
        << " socketType:" << profile.socketType
        << " addr: " << profile.addr;
        // 新的 matchManager注册到系统， matchServer需要连接到该服务
        if (profile.serviceType == kMatchManager)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                zsock_connect(matchManagerDealer(), "%s", profile.addr.data());
            }
        }
    }
}


void CPGMatchServer::handleData(const PacketHead& head,
                     char* data, size_t len)
{

}
 

std::list<ServiceProfile> CPGMatchServer::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);   

    auto pub = createServiceSocket<ZMQ_PUB>();
    pub_ = std::get<0>(tuple);

    return {{serviceType_, ZMQ_ROUTER, std::get<1>(tuple)}
            {serviceType_, ZMQ_SUB, std::get<1>(tuple)}};
}

zsock_t* CPGMatchServer::matchManagerDealer()
{
    if (!matchManagerDealer_)
    {
        matchManagerDealer_ = zsock_new(ZMQ_DEALER);
        reactor_->addSocket(matchManagerDealer_, std::bind(&CPGLoginServer::messageRead<ZMQ_DEALER>, this, std::placeholders::_1));
    }
    return matchManagerDealer_;
}

//
//  CPGMatchManager.cpp
//  ZMQNode
//
//  Created by fgr on 2018/8/7.
//  Copyright © 2018年 fgr. All rights reserved.
//

#include "CPGMatchManager.hpp"

// 抽取出来单独的组件
void CPGMatchManager::newServiceProfile(const std::list<ServiceProfile>& services)
{
    for(auto& profile : services)
    {
        std::cout << "new service: "
        << "type: " << profile.serviceType
        << " socketType:" << profile.socketType
        << " addr: " << profile.addr;
        // 新的 matchManager注册到系统， matchServer需要连接到该服务
        // 暂无
    }
}


void CPGMatchManager::handleData(const PacketHead& head,
                     char* data, size_t len)
{
    
}
 

std::list<ServiceProfile> 
CPGMatchManager::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);   

    auto pub = createServiceSocket<ZMQ_PUB>();
    pub_ = std::get<0>(tuple);

    return {{serviceType_, ZMQ_ROUTER, std::get<1>(tuple)}
            {serviceType_, ZMQ_SUB, std::get<1>(tuple)}};
}

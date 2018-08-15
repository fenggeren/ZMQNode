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

std::list<ServiceProfile> CPGMatchServer::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);   

    auto pub = createServiceSocket<ZMQ_PUB>();
    pub_ = std::get<0>(tuple);

    return {{serviceType_, ZMQ_ROUTER, std::get<1>(tuple)},
            {serviceType_, ZMQ_SUB, std::get<1>(tuple)}};
}

zsock_t* CPGMatchServer::matchManagerDealer()
{
    if (!matchManagerDealer_)
    {
        matchManagerDealer_ = zsock_new(ZMQ_DEALER);
        reactor_->addSocket(matchManagerDealer_, std::bind(&CPGMatchServer::messageRead<ZMQ_DEALER>, this, std::placeholders::_1));
    }
    return matchManagerDealer_;
}

void CPGMatchServer::configMessageHandlers()
{
    messageHandlers_[kServiceJoinRQ] =
    std::bind(&CPGMatchServer::handlerMatchJoinRQ, this, _1, _2, _3);
    messageHandlers_[kServiceUnjoinRQ] =
    std::bind(&CPGMatchServer::handlerMatchUnjoinRQ, this, _1, _2, _3);
}


/////

void CPGMatchServer::handleData(const PacketHead& head,
                                const std::string& extra,
                                const char* data, size_t len)
{
    
}

void CPGMatchServer::handlerMatchJoinRQ(const char* data, size_t len,
                        const std::string& extra)
{
    CPG::JoinMatchRQ rq;
    rq.ParseFromArray(data, len);
    rq.PrintDebugString();
    
    CPG::JoinMatchRS rs;
    rs.set_status(200);
    rs.set_userid(rq.userid());
    rs.set_matchid(rq.matchid());
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, extra.data());
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceJoinRS, rs);
    send(msg, router_);
}
void CPGMatchServer::handlerMatchUnjoinRQ(const char* data, size_t len,
                          const std::string& extra)
{
    CPG::UnjoinMatchRQ rq;
    rq.ParseFromArray(data, len);
    rq.PrintDebugString();
    
    CPG::UnjoinMatchRS rs;
    rs.set_status(200);
    rs.set_userid(rq.userid());
    rs.set_matchid(rq.matchid());
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, extra.data());
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceUnjoinRS, rs);
    send(msg, router_); 
}

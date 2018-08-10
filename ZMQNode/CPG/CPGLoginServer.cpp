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
    // login 暂无 动态连接的服务
}

 
void CPGLoginServer::handleData(const PacketHead& head,
                                const std::string& extra,
                                 const char* data, size_t len)
{

}

std::list<ServiceProfile>
CPGLoginServer::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);    
    return {{serviceType_, ZMQ_ROUTER, std::get<1>(tuple)}};
}

void CPGLoginServer::handlerLoginRQ(const char* data, size_t len,
                                    const std::string& extra)
{
    CPG::LoginRQ rq;
    rq.ParseFromArray(data, len);
    rq.PrintDebugString();
    
    CPG::LoginRS rs;
    rs.set_userid(rq.userid());
    rs.set_status(200);
    
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, extra.data());
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceLoginRS, rs);
    zmsg_send(&msg, router_);
    zmsg_destroy(&msg);
}

void CPGLoginServer::configMessageHandlers()
{
    messageHandlers_[kServiceLoginRQ] =
    std::bind(&CPGLoginServer::handlerLoginRQ, this, _1, _2, _3);
}


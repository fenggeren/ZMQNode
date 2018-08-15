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
                                 const std::string& extra,
                                 const char* data, size_t len)
{
    
}
 

std::list<ServiceProfile> 
CPGMatchManager::allServiceProfiles()
{
    auto tuple = createServiceSocket<ZMQ_ROUTER>();
    router_ = std::get<0>(tuple);   

    auto pub = createServiceSocket<ZMQ_PUB>();
    pub_ = std::get<0>(tuple);

    return {
            {serviceType_, ZMQ_ROUTER, std::get<1>(tuple)},
            {serviceType_, ZMQ_PUB, std::get<1>(tuple)}
           };
}

void CPGMatchManager::configMessageHandlers()
{
    messageHandlers_[kServiceMatchListRQ] =
    std::bind(&CPGMatchManager::handlerMatchListRQ, this, _1, _2, _3);
}

void CPGMatchManager::handlerMatchListRQ(const char* data, size_t len,
                        const std::string& extra)
{
    CPG::MatchListInfoRQ rq;
    rq.ParseFromArray(data, len);
    rq.PrintDebugString();
    
    
    CPG::MatchListInfoRS rs;
    rs.set_status(200);
    rs.set_userid(rq.userid());
    for(int idx : {1,2,3,4,5})
    {
        auto info = rs.add_infos();
        info->set_matchid(idx);
        info->set_entryfee(40000);
        info->set_matchname("测试资格赛");
    }
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, extra.data());
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceMatchListRS, rs);
    send(msg, router_);
}

void CPGMatchManager::pubMatchList()
{
    CPG::MatchListInfoRS rs;
    rs.set_status(200);
    rs.set_userid(0);
    for(int idx : {1,2,3,4,5})
    {
        auto info = rs.add_infos();
        info->set_matchid(idx);
        info->set_entryfee(40000);
        info->set_matchname("测试资格赛");
    }
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, "");
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceMatchListRS, rs);
    send(msg, pub_); 
}








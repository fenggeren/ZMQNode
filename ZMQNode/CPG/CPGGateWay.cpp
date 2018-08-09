//
//  CPGGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGGateWay.hpp"

// 抽取出来单独的组件
void CPGGateWay::newServiceProfile(const std::list<ServiceProfile>& services)
{
    for(auto& profile : services)
    {
        std::cout << "new service: "
        << "type: " << profile.serviceType
        << " socketType:" << profile.socketType
        << " addr: " << profile.addr;
        
        if (profile.serviceType == kLoginServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                zsock_connect(loginDealer(), "%s", profile.addr.data());
            }
        }
        else if (profile.serviceType == kMatchServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                if (matchServices_.insert(profile).second)
                {
                    zsock_t* dealer = zsock_new(ZMQ_DEALER);
                    zsock_connect(dealer, "%s", profile.addr.data());
                    reactor_->addSocket(loginDealer_, std::bind(&CPGGateWay::messageRead<ZMQ_DEALER>, 
                                                                    this, std::placeholders::_1));
                    matchDealers_.push_back(dealer);
                }
            }
            else if (profile.socketType == ZMQ_PUB)
            {
                zsock_connect(matchSub(), "%s", profile.addr.data());
            }
        }
        else if (profile.serviceType == kMatchManager)
        {
            if (profile.socketType == ZMQ_PUB)
            {
                zsock_connect(matchSub(), "%s", profile.addr.data());
            }
            else if (profile.socketType == ZMQ_ROUTER)
            {
                zsock_connect(matchManagerDealer(), "%s", profile.addr.data());
            }
        }
    }
}


void CPGGateWay::handleData(const PacketHead& head,
                     char* data, size_t len)
{
    
}


zsock_t* CPGGateWay::loginDealer()
{
    if (!loginDealer_)
    {
        loginDealer_ = zsock_new(ZMQ_DEALER);
        reactor_->addSocket(loginDealer_, std::bind(&CPGGateWay::messageRead<ZMQ_DEALER>, this, std::placeholders::_1));
    }
    return loginDealer_;
}
zsock_t* CPGGateWay::matchSub()
{
    if (!matchSub_)
    {
        matchSub_ = zsock_new(ZMQ_SUB);
        reactor_->addSocket(matchSub_, std::bind(&CPGGateWay::messageRead<ZMQ_SUB>, this, std::placeholders::_1));
    }
    return matchSub_;
}
zsock_t* CPGGateWay::matchManagerDealer()
{
    if (!matchManagerDealer_)
    {
        matchManagerDealer_ = zsock_new(ZMQ_DEALER);
        reactor_->addSocket(matchManagerDealer_, std::bind(&CPGGateWay::messageRead<ZMQ_DEALER>, this, std::placeholders::_1));
    }
    return matchManagerDealer_;
}


#pragma mark -

void CPGGateWay::sendLoginRQ(int uid, const std::string& token)
{
    CPG::LoginRQ rq;
    rq.set_userid(uid);
    rq.set_token(token);

    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceLoginRQ, rq);
    zmsg_send(&msg, loginDealer());
    zmsg_destroy(&msg);
}

void CPGGateWay::sendMatchListRQ(int uid)
{
    CPG::MatchListInfoRQ rq;
    rq.set_userid(uid);
    
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceMatchListRQ, rq);
    zmsg_send(&msg, loginDealer());
    zmsg_destroy(&msg);
}



#mark -

void CPGGateWay::handlerLoginRS(const char* data, size_t len)
{
    
}
void CPGGateWay::handlerMatchListRS(const char* data, size_t len)
{
    
}

void CPGGateWay::configMessageHandlers()
{
    messageHandlers_[kServiceLoginRS] = std::bind(&ZMQServerNodeBase::handlerLoginRS, this, std::placeholders::_1, std::placeholders::_2);
    messageHandlers_[kServiceMatchListRS] = std::bind(&ZMQServerNodeBase::handlerMatchListRS, this, std::placeholders::_1, std::placeholders::_2);
}



 


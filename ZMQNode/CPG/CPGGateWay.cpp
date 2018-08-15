//
//  CPGGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGGateWay.hpp" 

void CPGGateWay::startInit()
{
//    loginDealer();
//    matchSub();
//    matchManagerDealer();
}

// 抽取出来单独的组件
void CPGGateWay::newServiceProfile(const std::list<ServiceProfile>& services)
{
    printf("newServiceProfile uuid: %s\n", uuid.data());
    for(auto& profile : services)
    {
        if (profile.serviceType == kLoginServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                int rc = zsock_connect(loginDealer(), "%s", profile.addr.data());
                assert(rc == 0);
            }
        }
        else if (profile.serviceType == kMatchServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                // TODO!  创建sock，需要在主线程创建!
                if (matchServices_.insert(profile).second)
                {
                    zsock_t* dealer = createClientSocket<ZMQ_DEALER>();
                    zsock_connect(dealer, "%s", profile.addr.data());
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
                            const std::string& extra,
                            const char* data, size_t len)
{
    
}


zsock_t* CPGGateWay::loginDealer()
{
    if (!loginDealer_)
    {
        loginDealer_ = createClientSocket<ZMQ_DEALER>();
    }
    return loginDealer_;
}
zsock_t* CPGGateWay::matchSub()
{
    if (!matchSub_)
    {
        matchSub_ = createClientSocket<ZMQ_SUB>();
        zsock_set_subscribe(matchSub_, "");
    }
    return matchSub_;
}
zsock_t* CPGGateWay::matchManagerDealer()
{
    if (!matchManagerDealer_)
    {
        matchManagerDealer_ = createClientSocket<ZMQ_DEALER>();
    }
    return matchManagerDealer_;
}


#pragma mark - 
void CPGGateWay::sendLoginRQ(int uid, const std::string& token)
{
    assert(loginDealer_);
    CPG::LoginRQ rq;
    rq.set_userid(uid);
    rq.set_token(token);

    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceLoginRQ, rq);
    send(msg, loginDealer_);
}
 
void CPGGateWay::sendMatchListRQ(int uid)
{
    CPG::MatchListInfoRQ rq;
    rq.set_userid(uid);
    
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceMatchListRQ, rq);
    send(msg, matchManagerDealer_);
}

void CPGGateWay::sendMatchJoinRQ(int uid, int mid)
{
    CPG::JoinMatchRQ rq;
    rq.set_userid(uid);
    rq.set_matchid(mid);
    
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceJoinRQ, rq);
    send(msg, *matchDealers_.begin());
}
void CPGGateWay::sendMatchUnjoinRQ(int uid, int mid)
{
    CPG::UnjoinMatchRQ rq;
    rq.set_userid(uid);
    rq.set_matchid(mid);
    
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceUnjoinRQ, rq);
    send(msg, *matchDealers_.begin()); 
}

////////////////
 
void CPGGateWay::handlerLoginRS(const char* data, size_t len,
                                const std::string& extra)
{
    CPG::MatchListInfoRS rs;
    rs.ParseFromArray(data, len);
    rs.PrintDebugString();
}
void CPGGateWay::handlerMatchListRS(const char* data, size_t len,
                                    const std::string& extra)
{
    CPG::MatchListInfoRS rs;
    rs.ParseFromArray(data, len);
    rs.PrintDebugString();
}

void CPGGateWay::handlerMatchJoinRS(const char* data, size_t len,
                     const std::string& extra)
{
    CPG::JoinMatchRS rs;
    rs.ParseFromArray(data, len);
    rs.PrintDebugString();
}

void CPGGateWay::handlerMatchUnjoinRS(const char* data, size_t len,
                       const std::string& extra)
{
    CPG::UnjoinMatchRS rs;
    rs.ParseFromArray(data, len);
    rs.PrintDebugString();
}

///

void CPGGateWay::configMessageHandlers()
{
    messageHandlers_[kServiceLoginRS] =
        std::bind(&CPGGateWay::handlerLoginRS, this, _1, _2, _3);
    messageHandlers_[kServiceMatchListRS] =         std::bind(&CPGGateWay::handlerMatchListRS, this, _1, _2, _3);
    messageHandlers_[kServiceJoinRS] =         std::bind(&CPGGateWay::handlerMatchJoinRS, this, _1, _2, _3);
    messageHandlers_[kServiceUnjoinRS] =         std::bind(&CPGGateWay::handlerMatchUnjoinRS, this, _1, _2, _3);
}



 


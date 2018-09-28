//
//  ZMQGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/9/28.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "ZMQGateWay.hpp"
#include "Packet.h"
#include "server.pb.h"
#include "CPGFuncHelper.hpp"

namespace ZGW
{
    void ZMQGateWay::sendLoginRQ(int uid, const std::string& token)
    {
        CPG::LoginRQ rq;
        rq.set_userid(uid);
        rq.set_token(token);
        
        zmsg_t* msg = zmsg_new();
        CPGFuncHelper::appendZMsg(msg, kGateWay, kServiceLoginRQ, rq);
        zmsg_send(&msg, dealer_);
        zmsg_destroy(&msg);
    }
    
    void ZMQGateWay::sendMatchListRQ(int uid)
    {
        CPG::MatchListInfoRQ rq;
        rq.set_userid(uid);
        
        zmsg_t* msg = zmsg_new();
        zmsg_addstr(msg, PUBLIST_CHANNEL_MATCHMANAGER);
        CPGFuncHelper::appendZMsg(msg, kGateWay, kServiceMatchListRQ, rq);
        zmsg_send(&msg, pub_);
        zmsg_destroy(&msg);
    }
    
    void ZMQGateWay::sendMatchJoinRQ(int uid, int mid)
    {
        CPG::JoinMatchRQ rq;
        rq.set_userid(uid);
        rq.set_matchid(mid);
        
        zmsg_t* msg = zmsg_new();
        zmsg_addstrf(msg, "%d", mid);
        CPGFuncHelper::appendZMsg(msg, kGateWay, kServiceJoinRQ, rq);
        zmsg_send(&msg, pub_);
        zmsg_destroy(&msg);
    }
    
    void ZMQGateWay::sendMatchUnjoinRQ(int uid, int mid)
    {
        CPG::UnjoinMatchRQ rq;
        rq.set_userid(uid);
        rq.set_matchid(mid);
        
        zmsg_t* msg = zmsg_new();
        zmsg_addstrf(msg, "%d", mid);
        CPGFuncHelper::appendZMsg(msg, kGateWay, kServiceUnjoinRQ, rq);
        zmsg_send(&msg, pub_);
        zmsg_destroy(&msg);
    }
}

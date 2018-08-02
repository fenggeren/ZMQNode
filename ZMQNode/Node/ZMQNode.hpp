//
//  ZMQNode.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <czmq.h>
#include <string>
#include "Defines.h"
#include "Packet.h"
#include "server.pb.h"
#include "CPGFuncHelper.hpp"

class CPGZMQMasterClient
{
public:
    
    CPGZMQMasterClient()
    : dealer_(zsock_new(ZMQ_DEALER))
    , sub_(zsock_new(ZMQ_SUB))
    {

    }
    
    int connect(int subServiceValue, const std::string& uuid)
    {
        zsock_set_identity(dealer_, uuid.data());
        return connects(MASTER_ROUTER_ENDPOINT, MASTER_PUB_ENDPOINT, subServiceValue);
    }
    
    int connects(const std::string& masterRouterEndpoint,
                  const std::string& masterPubEndpoint,
                  int subValue)
    {
        int rc = zsock_connect(dealer_, "%s", masterRouterEndpoint.data());
        assert(!rc);
        rc = zsock_connect(sub_, "%s", masterPubEndpoint.data());
        assert(!rc);
        std::string subValueStr(std::to_string(subValue));
        zsock_set_subscribe(sub_, subValueStr.data());
        return rc;
    }
    
    void sendHeartbeat(int serviceType)
    {
        CPG::ServiceHeartbeatMsg hbMsg;
        hbMsg.set_servicetype(serviceType);
        zmsg_t* msg = zmsg_new();
        CPGFuncHelper::appendZMsg(msg, serviceType, kServiceHeartMsg, hbMsg);
        zmsg_send(&msg, dealer_);
    }
    
    zsock_t* dealer() { return dealer_; };
    zsock_t* sub() {return sub_;}
    
    void setHeartbeatTimerID(int timerID) { heartbeatTimerID_ = timerID; }
    int heartbeatTimerID() const { return heartbeatTimerID_; }
private:
    // 用于注册dealer，和发送心跳包
    zsock_t* dealer_;
    // 订阅master， 用于系统加入其他新服务，服务节点订阅关心的需要连接的其他服务节点
    zsock_t* sub_;
    
    int heartbeatTimerID_;
};










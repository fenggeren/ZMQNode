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

class CPGZMQMasterClient
{
public:
    
    CPGZMQMasterClient(const std::string& masterRouterEndpoint,
                       const std::string& masterPubEndpoint,
                       int subValue)
    : dealer_(zsock_new(ZMQ_DEALER))
    , sub_(zsock_new(ZMQ_SUB))
    {
        
    }
    
    void connects(const std::string& masterRouterEndpoint,
                  const std::string& masterPubEndpoint,
                  int subValue)
    {
        zmq_connect(dealer_, masterRouterEndpoint.data());
        zmq_connect(sub_, masterPubEndpoint.data());
        
        std::string subValueStr(std::to_string(subValue));
        zmq_setsockopt(sub_, ZMQ_SUBSCRIBE, subValueStr.data(), subValueStr.size());
    }
    
private:
    // 用于注册dealer，和发送心跳包
    zsock_t* dealer_;
    // 订阅master， 用于系统加入其他新服务，服务节点订阅关心的需要连接的其他服务节点
    zsock_t* sub_;
};










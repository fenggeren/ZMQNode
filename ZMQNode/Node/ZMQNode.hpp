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
#include <set>
#include "Defines.h"
#include "Packet.h"
#include "server.pb.h"
#include "CPGFuncHelper.hpp"
#include "ZMQReactor.hpp"

class CPGZMQMasterClient
{
public:
    using NewServiceProfileCallback =
    std::function<void(const std::list<ServiceProfile>&)>;
    
    CPGZMQMasterClient(std::shared_ptr<ZMQReactor> reactor ,CPGServerType type);
     
    int connect(const std::set<std::string>& subids, const std::string& uuid);
    
    void sendHeartbeat(int serviceType);
    zsock_t* dealer() { return dealer_; };
    zsock_t* sub() {return sub_;}
    
    void setHeartbeatTimerID(int timerID) { heartbeatTimerID_ = timerID; }
    int heartbeatTimerID() const { return heartbeatTimerID_; }
    
    void setNewServiceProfile(const NewServiceProfileCallback& cb)
    {
        newServicesCallback_ = cb;
    }
    
    void registerMaster(std::list<ServiceProfile>&& profiles);
    
private:

    void masterMessageRead(zsock_t* sock);
    
    void handleNewServices(const PacketHead& head ,
                           const char* data, int len);
private:
    // 用于注册dealer，和发送心跳包
    zsock_t* dealer_;
    // 订阅master， 用于系统加入其他新服务，服务节点订阅关心的需要连接的其他服务节点
    zsock_t* sub_;
    
    CPGServerType serviceType_;
    
    int heartbeatTimerID_{0};
    
    std::shared_ptr<ZMQReactor> reactor_;
    
    NewServiceProfileCallback newServicesCallback_;
    
    // 存储当前节点 所有的服务
    std::list<ServiceProfile> serverProfiles_;
};










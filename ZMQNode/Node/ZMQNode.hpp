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

class ZMQMasterClient
{
public:
    using NewServiceProfileCallback =
    std::function<void(const std::list<ServiceProfile>&)>;
    
    ZMQMasterClient(std::shared_ptr<ZMQReactor> reactor ,CPGServerType type);
     
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
    
    void messageRead(zsock_t* sock);
    void pubMessageRead(zsock_t* sock);
    void readHandleChunkData(zmsg_t* msg);
    
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

// 服务/  Router/PUSH/PUB等
class ZMQService
{
public:
    
    ZMQService(int type)
    :sockType_(type)
    {
        service_ = zsock_new(type);
    }
    
    void bind(const std::string& uuid)
    {
        int port = zsock_bind(service_, "tcp://*:*");
        std::string identity = uuid + ":" + std::to_string(port);
        if (sockType_ == ZMQ_REQ ||
            sockType_ == ZMQ_REP ||
            sockType_ == ZMQ_DEALER ||
            sockType_ == ZMQ_ROUTER)
        {
            zsock_set_identity(service_, identity.data());
        }
        addr_ = "tcp://" + CPGFuncHelper::localIP() + ":" + std::to_string(port);
    }
    
    void sendMessage(const char* more,
                    const char* data, size_t len)
    {
        assert(sockType_ == ZMQ_PUB);
        
        zmsg_t* msg = zmsg_new();
        zmsg_addstr(msg, more);
        zmsg_addmem(msg, data, len);
        zmsg_send(&msg, service_);
        zmsg_destroy(&msg);
    }
 
    int sockType() const { return sockType_; }
    zsock_t* serviceSock() const { return service_; }
    const std::string& addr() const { return addr_; }
    
private:
    int sockType_;
    zsock_t* service_;
    std::string addr_;
};

class ZMQPubService : public ZMQService
{
public:
    
    void pubMessage(const std::string& channel,
                    const char* data, size_t len)
    {
        
    }
    
private:
    
};


// 过滤器
class ZMQMessageFilter
{
public:
    
    void operator()(zsock_t* sock)
    {
        int type = zsock_type(sock);
        zmsg_t* msg = zmsg_recv(sock);
        
        if (type == ZMQ_SUB)
        {
            
        }
    }
    
private:
    std::function<void()> callback_;
};







//
//  ZMQServerNodeBase.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/4.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <unordered_map>
#include <czmq.h>
#include <set>
#include <map>
#include <tuple>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "ZMQNode.hpp"
#include "CPGFuncHelper.hpp"

/*
 有效组合
 PUB and SUB
 REQ and REP
 REQ and ROUTER (take care, REQ inserts an extra null frame)
 DEALER and REP (take care, REP assumes a null frame)
 DEALER and ROUTER
 DEALER and DEALER
 ROUTER and ROUTER
 PUSH and PULL
 PAIR and PAIR
 */

using namespace std::placeholders;
class ZMQServerNodeBase
{
public:
    ZMQServerNodeBase(CPGServerType serviceType)
    : serviceType_(serviceType)
    {
        reactor_ = std::make_shared<ZMQReactor>();
        masterClient_ = std::make_shared<ZMQMasterClient>(reactor_, serviceType_);
    }
    
    virtual ~ZMQServerNodeBase() {}
    
    void start();
    
    // PUB -> SUB |  ROUTER -> DEALER | REQ -> ROUTER 
    // all has a extra first frame
    template<int TYPE>
    void messageRead(zsock_t* sock)
    {
        int sourceType = zsock_type(sock);
        zmsg_t* msg = zmsg_recv(sock);
        
        std::string extra;
        if (TYPE == ZMQ_SUB || TYPE == ZMQ_ROUTER ||
            (TYPE == ZMQ_REQ &&  sourceType != ZMQ_REP))
        {
            zframe_t* first = zmsg_first(msg);
            extra = std::string(reinterpret_cast<char*>(zframe_data(first)),
                                  zframe_size(first));
        }
        readData(extra, msg);
        zmsg_destroy(&msg);
    }
    
    
    void send(zmsg_t** msg, zsock_t* sock);
    
protected:
    
    virtual void readData(const std::string& extra, zmsg_t* msg);
    
    virtual void handleData(const PacketHead& head,
                            const std::string& extra,
                             const char* data, size_t len);
    

    virtual void newServiceProfile(const std::list<ServiceProfile>& services);

    virtual std::list<ServiceProfile> allServiceProfiles() { return {}; };
    
    virtual void configMessageHandlers() {};
    
    virtual void startInit() {};
private:

    // 统一处理相同的 命令，其余转发
    void handleCommonData(const PacketHead& head,
                          const std::string& extra,
                          const char* data, size_t len);

protected: // 统一回调命令处理
    
    void registerServiceCallback(const char* data, size_t len,
                                 const std::string& extra);
    
    // 创建服务端 socket
    template <int TYPE>
    std::tuple<zsock_t*, std::string> 
    createServiceSocket()
    {
        zsock_t* sock = zsock_new(TYPE);
        int port = zsock_bind(sock, "tcp://*:*");
        std::string identity = uuid + ":" + std::to_string(port);
        if (TYPE == ZMQ_REQ || TYPE == ZMQ_REP ||
            TYPE == ZMQ_DEALER || TYPE == ZMQ_ROUTER)
        {
            zsock_set_identity(sock, identity.data());
        }
        reactor_->addSocket(sock, std::bind(&ZMQServerNodeBase::messageRead<TYPE>, this, std::placeholders::_1));
        std::string addr = CPGFuncHelper::connectTCPAddress(port);
        return {sock, addr};
    }
    
    template <int TYPE>
    zsock_t* createClientSocket()
    {
        clientCount++;
        zsock_t* sock = zsock_new(TYPE);
        std::string identity = uuid + ":" + std::to_string(clientCount);
        if (TYPE == ZMQ_REQ || TYPE == ZMQ_REP ||
            TYPE == ZMQ_DEALER || TYPE == ZMQ_ROUTER)
        {
            zsock_set_identity(sock, identity.data());
        }
        reactor_->addSocket(sock, std::bind(&ZMQServerNodeBase::messageRead<TYPE>, this, std::placeholders::_1));
        return sock;
    }
    
    virtual std::string uuidPrefix()
    {
        return "";
    }
    
    static int validCount()
    {
        return clientCount++;
    }
protected:
    using MessageHandler = std::function<void(const char*, size_t,
                                              const std::string&)>;
    
    CPGServerType serviceType_;
    std::shared_ptr<ZMQReactor> reactor_;
    std::shared_ptr<ZMQMasterClient> masterClient_;
    std::string uuid;
    
    std::unordered_map<int, MessageHandler> messageHandlers_;
    
    static int clientCount;
};



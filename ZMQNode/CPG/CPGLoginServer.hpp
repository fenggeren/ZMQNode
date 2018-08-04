//
//  CPGLoginServer.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include "ZMQNode.hpp" 
#include <czmq.h>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "CPGFuncHelper.hpp"

struct ServerProfile
{
    int serviceType{0};
    zsock_t* service_;
    std::string addr;
};

class CPGLoginServer
{
public:
    
    CPGLoginServer()
    : serviceType_(kLoginServer)
    {
        count++;
        uuid = std::string("LS-") + CPGFuncHelper::localIP() + "-" + std::to_string(count);
        server_.service_ = zsock_new(ZMQ_ROUTER);
        server_.serviceType = serviceType_;
        
        reactor_ = std::make_shared<ZMQReactor>();
        masterClient_ = std::make_shared<ZMQMasterClient>(reactor_, serviceType_);
    }
     
    
    void start();
    
    void parseClientData(const PacketHead& head,
                         char* data, size_t len);
    
    void registerServiceCallback(const PacketHead& head,
                                 char* data, size_t len);
private:
    
    void messageRead(zsock_t* sock);
    
    void newServiceProfile(const std::list<ServiceProfile>& services);
private:
    
    std::shared_ptr<ZMQMasterClient> masterClient_;
    std::shared_ptr<ZMQReactor> reactor_;
    zsock_t* router_;
    ServerProfile server_;
    CPGServerType serviceType_;
    std::string uuid;
    static int count;
};



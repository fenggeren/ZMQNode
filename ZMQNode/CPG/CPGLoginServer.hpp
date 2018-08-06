//
//  CPGLoginServer.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include "ZMQServerNodeBase.hpp"

struct ServerProfile
{
    int serviceType{0};
    zsock_t* service_;
    std::string addr;
};

class CPGLoginServer : public ZMQServerNodeBase
{
public:
    
    CPGLoginServer()
    : ZMQServerNodeBase(kLoginServer)
    {
        uuid = std::string("LS-") + CPGFuncHelper::localIP();
        server_.service_ = zsock_new(ZMQ_ROUTER);
        server_.serviceType = serviceType_;
    }
     
    
private:
    
    virtual void handleData(const PacketHead& head,
                         char* data, size_t len) override;
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual std::list<ServiceProfile> allServiceProfiles() override; 
private:
    zsock_t* router_;
    ServerProfile server_; 
};



//
//  CPGLoginServer.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include "ZMQServerNodeBase.hpp"


class CPGLoginServer : public ZMQServerNodeBase
{
public:
    
    CPGLoginServer()
    : ZMQServerNodeBase(kLoginServer)
    {
    }
 
private:
    void handlerLoginRQ(const char* data, size_t len, const std::string& extra); 
    
private:
    
    virtual void handleData(const PacketHead& head,
                            const std::string& extra,
                         const char* data, size_t len) override;
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual std::list<ServiceProfile> allServiceProfiles() override;
    virtual void configMessageHandlers() override;
    virtual std::string uuidPrefix() override
    {
        return "LS-";
    }
private:
    zsock_t* router_; 
};



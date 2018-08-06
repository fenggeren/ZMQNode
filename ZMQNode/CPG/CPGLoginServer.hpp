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
        uuid = std::string("LS-") + CPGFuncHelper::localIP(); 
    }
     
    
private:
    
    virtual void handleData(const PacketHead& head,
                         char* data, size_t len) override;
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual std::list<ServiceProfile> allServiceProfiles() override; 
private:
    zsock_t* router_; 
};



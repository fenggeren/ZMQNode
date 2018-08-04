//
//  CPGGateWay.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <czmq.h>
#include <set>
#include <map>
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"
#include "ZMQNode.hpp"
#include "CPGFuncHelper.hpp"

class CPGGateWay
{
public:
    
    CPGGateWay()
    : serviceType_(kGateWay)
    {
        count++;
        uuid = std::string("GS-") + CPGFuncHelper::localIP() + "-" + std::to_string(count);
        reactor_ = std::make_shared<ZMQReactor>();
        masterClient_ = std::make_shared<ZMQMasterClient>(reactor_, serviceType_);
    }
     
    
    void start();
 
    void registerServiceCallback(const PacketHead& head,
                                 char* data, size_t len);
    
    
private: 
    
    void messageRead(zsock_t* sock);
    void messageSubRead(zsock_t* sock);
    void readData(zmsg_t* msg);
    
    void handleData(const PacketHead& head,
                         char* data, size_t len);
    void newServiceProfile(const std::list<ServiceProfile>& services);
    
private:
    
    struct CompServiceProfile
    {
        bool operator()(const ServiceProfile& p1,
                        const ServiceProfile& p2)
        {
            return p1.addr < p2.addr;
        }
    };
    
    zsock_t* loginDealer();
    zsock_t* matchSub();
private:
    zsock_t* loginDealer_;
    zsock_t* matchSub_;
    
    CPGServerType serviceType_;
    std::shared_ptr<ZMQReactor> reactor_;
    std::shared_ptr<ZMQMasterClient> masterClient_;
    
    // <mid,dealer>
    std::map<int, zsock_t*> matchDealerMap_;
    // dealers
    std::list<zsock_t*> matchDealers_;
    std::set<ServiceProfile,CompServiceProfile> matchServices_;
    std::string uuid;
    static int count;
};






















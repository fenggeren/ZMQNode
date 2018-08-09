//
//  CPGGateWay.hpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//
#pragma once

#include "ZMQServerNodeBase.hpp"

class CPGGateWay : public ZMQServerNodeBase
{
public:
    
    CPGGateWay()
    : ZMQServerNodeBase(kGateWay)
    {
        uuid = std::string("GS-") + CPGFuncHelper::localIP();
    }
    
public:

    void sendLoginRQ(int uid, const std::string& token);
    void sendMatchListRQ(int uid);
    
    
private:
    void handlerLoginRS(const char* data, size_t len);
    void handlerMatchListRS(const char* data, size_t len);
    
private: 
    
    virtual void handleData(const PacketHead& head,
                            char* data, size_t len) override;
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual void configMessageHandlers() override;
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
    zsock_t* matchManagerDealer();
private:
    zsock_t* loginDealer_;
    zsock_t* matchManagerDealer_;
    zsock_t* matchSub_;
    
    // <mid,dealer>  用于通过比赛id，传递消息给指定matchServer, 
    // 应该由 matchManager负责分配matchServer运行的比赛.
    std::map<int, zsock_t*> matchDealerMap_;
    // dealers, 每个dealer连接一个matchServer
    std::list<zsock_t*> matchDealers_;
    std::set<ServiceProfile,CompServiceProfile> matchServices_;
};






















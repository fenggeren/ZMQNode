//
//  CPGGateWay.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGGateWay.hpp"

int CPGGateWay::count = 0;


void CPGGateWay::start()
{
    std::set<std::string> subids;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        subids.insert(std::to_string(service.serviceType));
    }
    masterClient_->connect(subids, uuid);
    
    masterClient_->setNewServiceProfile(std::bind(&CPGGateWay::newServiceProfile, this, std::placeholders::_1));
    reactor_->asyncLoop();
    
    masterClient_->registerMaster({});
}

// 抽取出来单独的组件
void CPGGateWay::newServiceProfile(const std::list<ServiceProfile>& services)
{
    for(auto& profile : services)
    {
        std::cout << "new service: "
        << "type: " << profile.serviceType
        << " socketType:" << profile.socketType
        << " addr: " << profile.addr;
        
        if (profile.serviceType == kLoginServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                zsock_connect(loginDealer(), "%s", profile.addr.data());
            }
        }
        else if (profile.serviceType == kMatchServer)
        {
            if (profile.socketType == ZMQ_ROUTER)
            {
                if (matchServices_.insert(profile).second)
                {
                    zsock_t* dealer = zsock_new(ZMQ_DEALER);
                    zsock_connect(dealer, "%s", profile.addr.data());
                    reactor_->addSocket(loginDealer_, std::bind(&CPGGateWay::messageRead, this, std::placeholders::_1));
                    matchDealers_.push_back(dealer);
                }
            }
            else if (profile.socketType == ZMQ_PUB)
            {
                zsock_connect(matchSub(), "%s", profile.addr.data());
            }
        }
        else if (profile.serviceType == kMatchManager)
        {
            zsock_connect(matchSub(), "%s", profile.addr.data());
        }
    }
}

void CPGGateWay::messageRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    zmsg_print(msg);
    size_t count = zmsg_size(msg);
    for (int i = 0; i < count - 1; i+=2)
    {
        readData(msg);
    }
    zmsg_destroy(&msg);
}
void CPGGateWay::messageSubRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    
    zframe_t* channel = zmsg_first(msg);
    zframe_print(channel, "SUB IDENTITY: ");
    readData(msg);
    zmsg_destroy(&msg);
}

void CPGGateWay::readData(zmsg_t* msg)
{
    zframe_t* headFrame = zmsg_next(msg);
    PacketHead head;
    memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
    
    zframe_t* dataFrame = zmsg_next(msg);
    char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
    size_t size = zframe_size(dataFrame);
    handleData(head, data, size);
}

void CPGGateWay::handleData(const PacketHead& head,
                     char* data, size_t len)
{
    auto& command = head;
    switch (command.mainCmdID) {
        case kMaster:
        {
            switch (command.subCmdID) {
                case kSeviceRegisterRS:
                    registerServiceCallback(head, data, len);
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        default:
            break;
    }
}

void CPGGateWay::registerServiceCallback(const PacketHead& head, char* data, size_t len)
{
    CPG::ServiceRegisterRS rs;
    rs.ParseFromArray(data, (int)len);
    
    rs.PrintDebugString();
}

zsock_t* CPGGateWay::loginDealer()
{
    if (!loginDealer_)
    {
        loginDealer_ = zsock_new(ZMQ_DEALER);
        reactor_->addSocket(loginDealer_, std::bind(&CPGGateWay::messageRead, this, std::placeholders::_1));
    }
    return loginDealer_;
}
zsock_t* CPGGateWay::matchSub()
{
    if (!matchSub_)
    {
        matchSub_ = zsock_new(ZMQ_SUB);
        reactor_->addSocket(matchSub_, std::bind(&CPGGateWay::messageSubRead, this, std::placeholders::_1));
    }
    return matchSub_;
}






 


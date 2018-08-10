//
//  ZMQNode.cpp
//  ZMQNode
//
//  Created by huanao on 2018/7/31.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "ZMQNode.hpp"

ZMQMasterClient::ZMQMasterClient(std::shared_ptr<ZMQReactor> reactor
                                       ,CPGServerType type)
: serviceType_(type)
, reactor_(reactor)
, dealer_(zsock_new(ZMQ_DEALER))
, sub_(zsock_new(ZMQ_SUB))
{
    
}

void ZMQMasterClient::registerMaster(std::list<ServiceProfile>&& profiles)
{
    CPG::ServiceRegisterRQ rq;
    rq.set_servicetype(serviceType_);
    
    for (auto& profile : profiles)
    {
        auto service = rq.add_services();
        service->set_addr(profile.addr);
        service->set_sockettype(profile.socketType);
        service->set_servicetype(profile.serviceType);
    }

    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType_, kServiceRegisterRQ, rq);
    zmsg_send(&msg, dealer_);
    zmsg_destroy(&msg);
    
    serverProfiles_.swap(profiles);
}

int ZMQMasterClient::connect(const std::set<std::string>& subids,
                                const std::string& uuid)
{
    zsock_set_identity(dealer_, uuid.data());
    int rc = zsock_connect(dealer_, "%s", MASTER_ROUTER_ENDPOINT.data());
    assert(!rc);
    
    rc = zsock_connect(sub_, "%s", MASTER_PUB_ENDPOINT.data());
    assert(!rc);
    for (auto& subid : subids)
    {
        zsock_set_subscribe(sub_, subid.data());
    }
    
    heartbeatTimerID_ = reactor_->addTimer(kHeartbeatDuration * kSecondPerMilli, 0, std::bind(&ZMQMasterClient::sendHeartbeat, this, serviceType_));
    
    reactor_->addSocket(sub_, std::bind(&ZMQMasterClient::pubMessageRead, this, std::placeholders::_1));
    reactor_->addSocket(dealer_, std::bind(&ZMQMasterClient::messageRead, this, std::placeholders::_1));
 
    
    return rc;
}
 
void ZMQMasterClient::sendHeartbeat(int serviceType)
{
    CPG::ServiceHeartbeatMsg hbMsg;
    hbMsg.set_servicetype(serviceType);
    zmsg_t* msg = zmsg_new();
    CPGFuncHelper::appendZMsg(msg, serviceType, kServiceHeartMsg, hbMsg);
    zmsg_send(&msg, dealer_);
}

void ZMQMasterClient::messageRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    
    size_t count = zmsg_size(msg);
    for (int i = 0; i < count - 1; i+=2)
    {
        readHandleChunkData(msg);
    }
    zmsg_destroy(&msg);
}

void ZMQMasterClient::pubMessageRead(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    
    zframe_t* channel = zmsg_first(msg);
    zframe_print(channel, "SUB IDENTITY: ");
    
    readHandleChunkData(msg);
    zmsg_destroy(&msg);
}

void ZMQMasterClient::readHandleChunkData(zmsg_t* msg)
{
    zframe_t* headFrame = zmsg_next(msg);
    PacketHead head;
    memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
    
    zframe_t* dataFrame = zmsg_next(msg);
    char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
    size_t size = zframe_size(dataFrame);
    
    handleNewServices(head, data, (int)size);
}

void ZMQMasterClient::handleNewServices(const PacketHead& head ,
                                           const char* data, int len)
{
    if (head.mainCmdID != kMaster)
    {
        // ERROR!
        // 接收到非master的消息
    }
    printf("mainID: %d, subID: %d\n", head.mainCmdID, head.subCmdID);
    std::list<ServiceProfile> services;
    
    if (head.subCmdID == kServiceRegisterRS)
    {
        CPG::ServiceRegisterRS rs;
        rs.ParseFromArray(data, len);
        for(auto& s : rs.connectservices())
        {
            services.push_back({s.servicetype(), s.sockettype(), s.addr()});
        }
    }
    else if (head.subCmdID == kServicePublishNewServicesMsg)
    {
        CPG::ServicePublishNewServicesMsg msg;
        msg.ParseFromArray(data, len);
        for(auto& s : msg.newservices())
        {
            services.push_back({s.servicetype(), s.sockettype(), s.addr()});
        }
    }
    if (newServicesCallback_)
    {
        newServicesCallback_(services);
    }
}








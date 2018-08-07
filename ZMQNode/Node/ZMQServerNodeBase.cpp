//
//  ZMQServerNodeBase.cpp
//  ZMQNode
//
//  Created by huanao on 2018/8/4.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "ZMQServerNodeBase.hpp"

void ZMQServerNodeBase::start()
{
    std::set<std::string> subids;
    for (auto& service : serviceConnectMap[serviceType_])
    {
        subids.insert(std::to_string(service.serviceType));
    }
    masterClient_->connect(subids, uuid);
    
    masterClient_->setNewServiceProfile(
                std::bind(&ZMQServerNodeBase::newServiceProfile,
                          this, std::placeholders::_1));
    reactor_->asyncLoop();
    
    masterClient_->registerMaster(allServiceProfiles());
}

void ZMQServerNodeBase::readData(const std::string& extra, zmsg_t* msg)
{
    zframe_t* headFrame = zmsg_next(msg);
    PacketHead head;
    memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
    
    zframe_t* dataFrame = zmsg_next(msg);
    char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
    size_t size = zframe_size(dataFrame);
    handleCommonData(head, data, size);
}

void ZMQServerNodeBase::handleCommonData(const PacketHead& head,
                        char* data, size_t len)
{
    switch (head.subCmdID) 
    {
    case kSeviceRegisterRS:
        registerServiceCallback(data, len);
        break;
    default:
        handleData(head, data, len);
    }
}

// 子类调用
void ZMQServerNodeBase::handleData(const PacketHead& head,
                char* data, size_t len)
{

}

void ZMQServerNodeBase::newServiceProfile(const std::list<ServiceProfile>& services)
{
    
}

void ZMQServerNodeBase::registerServiceCallback(char* data, size_t len)
{
    CPG::ServiceRegisterRS rs;
    rs.ParseFromArray(data, (int)len);
    
    std::list<ServiceProfile> services;
    for(auto& s : rs.connectservices())
    {
        services.push_back({s.servicetype(), s.sockettype(), s.addr()});
    }
    
    newServiceProfile(services);
}






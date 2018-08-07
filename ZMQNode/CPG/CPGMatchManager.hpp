#pragma once


#include "ZMQServerNodeBase.hpp"

class CPGMatchManager : public ZMQServerNodeBase
{
public:
    CPGMatchManager()
    : ZMQServerNodeBase(kMatchServer)
    {
        uuid = std::string("MM-") + CPGFuncHelper::localIP();
    }

private:

    virtual void handleData(const PacketHead& head,
                            char* data, size_t len) override;
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual std::list<ServiceProfile> allServiceProfiles() override; 

private:
    zsock_t* pub_;
    zsock_t* router_;
};

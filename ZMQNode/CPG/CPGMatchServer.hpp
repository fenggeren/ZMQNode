#pragma once

#include "ZMQServerNodeBase.hpp"

class CPGMatchServer : public ZMQServerNodeBase
{
public:

    CPGMatchServer()
    : ZMQServerNodeBase(kMatchServer)
    {
    }
    
    
private:
    
    void handlerMatchJoinRQ(const char* data, size_t len,
                            const std::string& extra);
    void handlerMatchUnjoinRQ(const char* data, size_t len,
                            const std::string& extra);

private:

    virtual void handleData(const PacketHead& head,
                            const std::string& extra,
                            const char* data, size_t len) override;
    
    virtual void newServiceProfile(const std::list<ServiceProfile>& services) override;
    
    virtual std::list<ServiceProfile> allServiceProfiles() override; 
    virtual void configMessageHandlers() override;
    
    virtual std::string uuidPrefix() override
    {
        return "MS-";
    }
    
private:
    zsock_t* matchManagerDealer();
private:

    zsock_t* pub_;
    zsock_t* router_;

    // 连接matchManager
//    std::list<zsock_t*> matchManagerDealers_;
    zsock_t* matchManagerDealer_;

};















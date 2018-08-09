#pragma once


#include "ZMQServerNodeBase.hpp"


// 默认只有一个
// 或者只有一个主manager， 其他都是从
// matchserver只与主manager交互，
// 其他从manager pub 主manager，更新变化？
// 没必要， 还是只有一个吧!
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

//
//  CPGFuncHelper.hpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include <string>

namespace google { namespace protobuf { class Message; } }


namespace CPGFuncHelper
{
    void appendZMsg(void* msg, unsigned short mainCmdID,
                    unsigned short subCmdID,
                    const google::protobuf::Message& pMsg);
    
    // tcp://*:3212  从zmq绑定中 获取端口号
    unsigned short getPort(const std::string& bindAddr);

    std::string connectTCPAddress(unsigned short port);
    // 本机ip地址
    std::string localIP();
}



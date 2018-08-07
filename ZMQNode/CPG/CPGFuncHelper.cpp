//
//  CPGFuncHelper.cpp
//  ZMQNode
//
//  Created by huanao on 2018/8/2.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "CPGFuncHelper.hpp"
#include "server.pb.h"
#include "Defines.h"
#include "Packet.h"
#include <czmq.h>
#include <unistd.h>
#include <netdb.h>  //gethostbyname
#include <arpa/inet.h>  //ntohl
#include <iostream>

namespace CPGFuncHelper
{
    void appendZMsg(void* msg,
                   unsigned short mainCmdID, unsigned short subCmdID,
                const google::protobuf::Message& pMsg)
    {
        zmsg_t* zmsg = static_cast<zmsg_t*>(msg);
        PacketHead head = {mainCmdID, subCmdID};
        google::protobuf::uint8* szBuf = new google::protobuf::uint8[pMsg.ByteSize()];
        pMsg.SerializeWithCachedSizesToArray(szBuf);
        zmsg_addmem(zmsg, &head, sizeof(head));
        zmsg_addmem(zmsg, szBuf, pMsg.ByteSize());
        
        delete [] szBuf;
    }
    
    std::string connectTCPAddress(unsigned short port)
    {
        return "tcp://" + CPGFuncHelper::localIP() + ":" + std::to_string(port);
    }

    unsigned short getPort(const std::string& bindAddr)
    {
        auto pos = bindAddr.find_last_of(":"); 
        assert(pos < bindAddr.size()-1);
        auto port = bindAddr.substr(pos+1, bindAddr.size());
        return std::atoi(port.data());
    }

    std::string localIP()
    {
        char hname[128];
        struct hostent *hent;
        
        gethostname(hname, sizeof(hname));
        
        //hent = gethostent();
        hent = gethostbyname(hname); 
        
        std::string lip = "";
        if (hent->h_length > 0)
        {
            lip = inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
        }
        return lip;
    }
 
}


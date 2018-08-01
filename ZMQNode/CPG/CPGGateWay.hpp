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
#include "Packet.h"
#include "ZMQReactor.hpp"
#include "server.pb.h"

class CPGGateWay
{
public:
    
    CPGGateWay()
    : reactor_()
    {
    }
    
    void registerMaster()
    {
        masterDealer_ = zsock_new(ZMQ_DEALER);
        zsock_connect(masterDealer_, "%s", MASTER.data());
        reactor_.addSocket(masterDealer_, [](zsock_t* sock){
            
        });
        reactor_.asyncLoop();
    }
    
    void sendRegisterService()
    {
        CPG::ServiceRegisterRQ rq;
        auto service = rq.mutable_service();
        service->set_addr("127.0.0.1:2279");
        service->set_servicetype(kGateWay);
        
        PacketHead head;
        head.info = {0, 0, static_cast<unsigned int>(rq.ByteSize())};
        head.command = {kGateWay, kSeviceRegisterRQ};
        
        google::protobuf::uint8* szBuf =
        new google::protobuf::uint8[rq.ByteSize()];
        rq.SerializeWithCachedSizesToArray(szBuf);
        
        zmsg_t* msg = zmsg_new();
        
        zmsg_addmem(msg, &head, sizeof(head));
        zmsg_addmem(msg, szBuf, head.info.packetSize);
        //
        
        //        zmsg_destroy(&msg);
        
        delete []szBuf;
    }
    
    void clientMessageRead(zsock_t* sock)
    {
        zmsg_t* msg = zmsg_recv(sock);
        size_t count = zmsg_size(msg);
        printf("client receive begin\n");
        
        for (int i = 0; i < count - 1; i+=2)
        {
            zframe_t* headFrame = zmsg_next(msg);
            PacketHead head;
            memcpy(&head, zframe_data(headFrame), zframe_size(headFrame));
            
            
            zframe_t* dataFrame = zmsg_next(msg);
            char* data = reinterpret_cast<char*>(zframe_data(dataFrame));
            size_t size = zframe_size(dataFrame);
            parseClientData(head, data, size);
        }
        printf("\nreceive end\n");
    }
    
    void parseClientData(const PacketHead& head,
                         char* data, size_t len)
    {
        auto& command = head.command;
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
    
    void registerServiceCallback(const PacketHead& head, char* data, size_t len)
    {
        CPG::ServiceRegisterRS rs;
        rs.ParseFromArray(data, len);
        
        rs.PrintDebugString();
    }
    
private:
    zsock_t* masterDealer_;
    zsock_t* loginDealer_;
    zsock_t* matchPull_;
    ZMQReactor reactor_;
    std::unordered_multimap<int, zsock_t*> matchDealers_;
};






















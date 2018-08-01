//
//  ZMQReactor.hpp
//  ZeroMQDemo
//
//  Created by huanao on 2018/7/26.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <list>
#include <czmq.h>
#include <thread>


class ZMQReactor
{
    class WakeupPair
    {
    public:
        
        WakeupPair()
        : sender_(zsock_new(ZMQ_PAIR))
        , receiver_(zsock_new(ZMQ_PAIR))
        {
            init();
        }
        
        void init()
        {
            zsock_bind(receiver_, "inproc://wakup");
            zsock_connect(sender_, "inproc://wakup");
        }
        
        void notify()
        {
            std::cout << "notify" << std::endl;
            zmsg_t* msg = zmsg_new();
            zmsg_send(&msg, sender_);
        }
        
        void handleRead()
        {
            std::cout << "handleRead" << std::endl;
            zmsg_t* msg = zmsg_recv(receiver_);
            zmsg_destroy(&msg);
        }
        
        zsock_t* receiver() { return receiver_; }
        
    private:
        zsock_t* sender_;
        zsock_t* receiver_;
    };
    
    struct ZMQCallback
    {
        zsock_t* socket;
        std::function<void(zsock_t*)> callback_;
        
        void operator()()
        {
            callback_(socket);
        }
    };
    
public:
    using ZMessageCallback = std::function<void(zsock_t*)>;
    
    ZMQReactor(zloop_t* loop = NULL)
    :  wakup_()
    , startedLoop_(false)
    , loop_(loop ?: zloop_new())
    {
        addSocket(wakup_.receiver(), [&](zsock_t* sock)
        {
            wakup_.handleRead();
        });
    }
    
    ~ZMQReactor()
    {
        if (thread_)
        {
            thread_->join();
        }
    }
    
    void addSocket(zsock_t* sock, const ZMessageCallback& cb)
    {
        zloop_reader(loop_, sock, &ZMQReactor::messageIn, this);
        if (startedLoop_)
        {
            wakup_.notify();
        }
        messageCallbacks_[sock] = cb;
    }
    
    void removeSocket(zsock_t* sock)
    {
        zloop_reader_end(loop_, sock);
        messageCallbacks_.erase(sock);
    }
    
    int loop()
    {
        startedLoop_ = true;
        return zloop_start(loop_);
    }
    
    void asyncLoop()
    {
        startedLoop_ = true;
        thread_ = std::make_shared<std::thread>([&]{
            zloop_start(loop_);
        });
    }
    
    void stop()
    {
        // TODO.
    }
    
private:
    
    static int messageIn(zloop_t *loop, zsock_t *reader, void *arg)
    {
        ZMQReactor* reactor = static_cast<ZMQReactor*>(arg);
        auto iter = reactor->messageCallbacks_.find(reader);
        if (iter != reactor->messageCallbacks_.end())
        {
            iter->second(reader);
        }
        return 1;
    }
    
    
private:
    bool startedLoop_;
    zloop_t* loop_;
    WakeupPair wakup_;
    std::unordered_map<zsock_t*, ZMessageCallback> messageCallbacks_;
    std::vector<zmq_pollitem_t> items_;
    std::list<zsock_t*> willRemovedSocks_;
    ZMessageCallback messageCallback_;
    std::shared_ptr<std::thread> thread_;
};







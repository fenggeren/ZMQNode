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
    using ZTimerCallback = std::function<void()>;
    
    ZMQReactor(zloop_t* loop = NULL)
    :  wakup_()
    , startedLoop_(false)
    , loop_(loop ?: zloop_new())
    , messageCallbacks_()
    , timerCallbacks_()
    {
        addSocket(wakup_.receiver(), [&](zsock_t* sock)
        {
            wakup_.handleRead();
        });
    }
    
    ~ZMQReactor()
    {
        for (auto& pair : messageCallbacks_)
        {
            zloop_reader_end(loop_, pair.first);
        }
        for (auto& pair : timerCallbacks_)
        {
            zloop_timer_end(loop_, pair.first);
        }
        messageCallbacks_.clear();
        timerCallbacks_.clear();
        
        if (thread_)
        {
            thread_->join();
        }
    }
    
    void addSocket(zsock_t* sock, const ZMessageCallback& cb)
    {
        zloop_reader(loop_, sock, &ZMQReactor::messageHandle, this);
        messageCallbacks_[sock] = cb;
        if (startedLoop_)
        {
            wakup_.notify();
        }
    }
    void removeSocket(zsock_t* sock)
    {
        zloop_reader_end(loop_, sock);
        messageCallbacks_.erase(sock);
    }
    
    int addTimer(size_t delay, size_t times, const ZTimerCallback& cb)
    {
        int timerID = zloop_timer(loop_, delay, times, &ZMQReactor::timerHandle, this);
        timerCallbacks_[timerID] = cb;
        if (startedLoop_)
        {
            wakup_.notify();
        }
        return timerID;
    }
    void cancelTimer(int timerID)
    {
        zloop_timer_end(loop_, timerID);
        timerCallbacks_.erase(timerID);
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
    
    static int messageHandle(zloop_t *loop, zsock_t *reader, void *arg)
    {
        ZMQReactor* reactor = static_cast<ZMQReactor*>(arg);
        auto iter = reactor->messageCallbacks_.find(reader);
        if (iter != reactor->messageCallbacks_.end())
        {
            iter->second(reader);
        }
        return 0;
    }
    
    static int timerHandle(zloop_t *loop, int timer_id, void *arg)
    {
        ZMQReactor* reactor = static_cast<ZMQReactor*>(arg);
        auto iter = reactor->timerCallbacks_.find(timer_id);
        if (iter != reactor->timerCallbacks_.end())
        {
            iter->second();
        }
        return 0;
    }
    
    
private:
    bool startedLoop_;
    zloop_t* loop_;
    WakeupPair wakup_;
    std::unordered_map<zsock_t*, ZMessageCallback> messageCallbacks_;
    std::unordered_map<int, ZTimerCallback> timerCallbacks_;
    std::shared_ptr<std::thread> thread_;
};








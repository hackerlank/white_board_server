//
//  server.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "server.h"

#include <assert.h>

#include "server_message_codes.h"
#include "logging.h"
#include "scope_guard.h"

Server::Server() : thread_(nullptr), running_(false) {
}

Server::~Server() {
    if (thread_) {
        delete thread_;
        thread_ = nullptr;
    }

    if (loop_) {
        uv_loop_delete(loop_);
        loop_ = nullptr;
    }
    
    if (async_) {
        free(async_);
        async_ = nullptr;
    }
    
    if (async_messages_queue_) {
        delete async_messages_queue_;
        async_messages_queue_ = nullptr;
    }
}

bool Server::Init() {
    if (!loop_) {
        loop_ = uv_loop_new();
    }
    
    if (!async_) {
        async_ = static_cast<uv_async_t*>(calloc(1, sizeof(uv_async_t)));
    }
    
    if (!async_messages_queue_) 
	{
        async_messages_queue_ = new DoubleBufferQueue<async_message_t*>();

    }
    
    return Async::Init();
}

bool Server::Start() {
    thread_ = new std::thread([this] {
        this->tid_ = std::this_thread::get_id();
        uv_run(this->loop_, UV_RUN_DEFAULT);
        LOG(INFO) << "server thread quit...";
    });

    running_ = true;
    
    return true;
}

bool Server::Stop() {
    if (!thread_) {
        return true;
    }

    if (!IsInAsyncThread()) {
        auto msg = CreateMessage<Message>(SERVER_MESSAGE_STOP);
        PostMessage(msg);
        WaitForStop();
        return true;
    }

    uv_stop(loop_);
    
    running_ = false;

    return true;
}

bool Server::IsRunning() {
    return running_;
}

void Server::WaitForStop() {
    if (!thread_) {
        return;
    }
    
    if (thread_->joinable()) {
        thread_->join();
    }
}

void Server::OnMessage(Message* msg) {
    ON_SCOPE_EXIT([&] { DestroyMessage(msg); });
    
    switch (msg->code) {
        case SERVER_MESSAGE_STOP:
            Stop();
            break;
        default:
            break;
    }
}

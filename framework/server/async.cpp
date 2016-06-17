//
//  async.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "async.h"

#include <assert.h>

#include "logging.h"

class AsyncCallback {
public:
    static void async_callback(uv_async_t* handle, int status) {
        Async* async = static_cast<Async*>(handle->data);
        assert(async);
        
        if (!async) {
            LOG(ERROR) << "handle->data in async_callback is not valid Async*";
            return;
        }

        auto queue = async->async_messages_queue_->swap();

        while (!queue->empty()) {
            async_message_t *req = nullptr;
            if (!queue->pop(&req)) {
                break;
            }
            assert(req);
            assert(req->async);
            assert(req->msg);

			req->async->OnMessage(req->msg);
            
            free(req);
        }
    }
};

Async::Async() : loop_(nullptr), async_(nullptr), async_messages_queue_(nullptr){
}

Async::~Async() {
}

Async& Async::Copy(const Async &src) {
    loop_ = src.loop_;
    tid_ = src.tid_;
    async_ = src.async_;
    async_messages_queue_ = src.async_messages_queue_;
    return *this;
}

std::thread::id Async::tid() {
    return tid_;
}

bool Async::Init() {
    assert(loop_);
    assert(async_);
    
    uv_async_init(loop_, async_, &AsyncCallback::async_callback);
    async_->data = this;

    return true;
}

void Async::PostMessage(const Message* msg) {
    assert(loop_);
    
    if (!loop_) {
        LOG(FATAL) << "Async require loop_ was initialized.";
        return;
    }
    
    if (!IsInAsyncThread()) {
        async_message_t* req = static_cast<async_message_t*>(calloc(1, sizeof(async_message_t)));
        req->msg = const_cast<Message*>(msg);
        req->async = this;
        async_messages_queue_->push(req);
        uv_async_send(async_);
        return;
    }

    OnMessage(const_cast<Message*>(msg));
}

bool Async::IsInAsyncThread() const {
    return std::this_thread::get_id() == tid_;
}

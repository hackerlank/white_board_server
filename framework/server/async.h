//
//  async.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_ASYNC_H_
#define FRAMEWORK_SERVER_ASYNC_H_

#include <stdint.h>
#include <stdlib.h>
#include <thread>

#include <uv.h>

#include "double_buffer_queue.h"

class Async;

typedef struct {
    int32_t code;
} Message;

typedef struct {
    Async*              async;
    Message*            msg;
} async_message_t;

template <class T>
T* CreateMessage(int32_t code) {
    T* msg = static_cast<T*>(calloc(1, sizeof(T)));
    msg->code = code;
    return msg;
}

template <class T>
void DestroyMessage(T* msg) {
    if (msg) {
        free(msg);
    }
}

class Async {
public:
    Async();
    virtual ~Async();
    
    Async& Copy(const Async& src);
    
    virtual bool Init();
    virtual void PostMessage(const Message* msg);
    virtual bool IsInAsyncThread() const;

public:
    std::thread::id tid();

protected:
    virtual void OnMessage(Message* msg) = 0;

protected:
    uv_loop_t*      loop_;
    uv_async_t*     async_;
    std::thread::id tid_;
    DoubleBufferQueue<async_message_t*>* async_messages_queue_;

private: // noncopyable
    Async(const Async &) = delete;
    Async & operator=(const Async &) = delete;

private:
    friend class AsyncCallback;
};

#endif  // FRAMEWORK_SERVER_ASYNC_H_

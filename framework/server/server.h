//
//  server.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_SERVER_H_
#define FRAMEWORK_SERVER_SERVER_H_

#include <stdint.h>
#include <stdlib.h>

#include <thread>

#include <uv.h>

#include "service.h"
#include "async.h"

class Service;

class Server : public Service, public Async {
protected:
    Server();
    ~Server();

public:
    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();
    virtual void WaitForStop();
    
protected:
    virtual void OnMessage(Message* msg);
    
protected:
    std::thread*    thread_;
    bool            running_;
};


#endif  // FRAMEWORK_SERVER_SERVER_H_

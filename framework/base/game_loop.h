//
//  game_loop.h
//  gateway
//
//  Created by Fubai Zhong on 10/8/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_GAME_LOOP_H_
#define FRAMEWORK_BASE_GAME_LOOP_H_

#include "service.h"
#include "service_manager.h"
#include "tcp_server.h"
#include <rapidjson/document.h>

class GameLoop {
public:
	explicit GameLoop(const std::string &json_config);
    virtual ~GameLoop();

    bool Run();

public:
    bool IsRunning();

    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual void Destory();
    virtual void Update() = 0;

    virtual void ForceStop();

	virtual	int	 InitDeamon()
	{
		return  0; 
	}

protected:
    ServiceManager *service_manager_;
    bool isRunning_;
    rapidjson::Document config_;
    std::thread *mainThread_;
};

#endif  // FRAMEWORK_BASE_GAME_LOOP_H_

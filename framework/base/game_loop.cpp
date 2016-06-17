//
//  game_loop.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/8/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include <chrono>

#include "game_loop.h"

GameLoop::GameLoop(const std::string &json_config) : service_manager_(nullptr), isRunning_ (false), mainThread_(nullptr) {
	service_manager_ = ServiceManager::instance();
	config_.SetObject();
	config_.Parse<0>(json_config.c_str());
}

GameLoop::~GameLoop() {
	if (mainThread_ != nullptr) {
		delete mainThread_;
	}
}

bool GameLoop::Init() {
	//use service_manager to add service
	bool res = service_manager_->Init();
	//do something else;
	return res;
}

bool GameLoop::Start() {
	bool res = service_manager_->Start();
	//do something else;
	isRunning_ = true;
	return res;
}

bool GameLoop::Stop() {
	bool res = service_manager_->Stop();
	LOG(INFO) << "exiting...";
	//do something else;
	return res;
}

void GameLoop::Destory() {
	service_manager_->Destroy();
	//do something else;
}

void GameLoop::ForceStop() {
	isRunning_ = false;
}

bool GameLoop::Run() {
	if (!Init()) {
		return false;
	}

	InitDeamon(); 

	if (!Start()) {
		return false;
	}

	mainThread_ = new std::thread([&] {
		while(IsRunning()) {
			
		    std::chrono::milliseconds max_ms_per_cycle(30);
			std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
			start = std::chrono::high_resolution_clock::now();
			
			Update();

			end = std::chrono::high_resolution_clock::now();
			std::chrono::milliseconds elapsed_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			if (elapsed_duration < max_ms_per_cycle) {
			    auto sleep_duration = max_ms_per_cycle - elapsed_duration;
			    std::this_thread::sleep_for(sleep_duration);
			}
			
		}
	});
	mainThread_->join();
	Stop();
	Destory();
	return true;
}



bool GameLoop::IsRunning() {
	return isRunning_;
}

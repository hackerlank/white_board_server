//
//  service_manager.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/4/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "service_manager.h"
#include "logging.h"

ServiceManager::ServiceManager () : initialized_(false), running_(false){
}

ServiceManager::~ServiceManager() {
}

bool ServiceManager::Register(const char* name, Service* service) {
    return Register(std::string(name), service);
}

bool ServiceManager::Register(const std::string& name, Service* service) {
    auto it = services_.find(name);
    assert(it == services_.end());
    
    if (it != services_.end()) {
        LOG(ERROR) << "service {" << name << "} already registered.";
        return false;
    }
    
    services_[name] = service;
    return true;
}

Service* ServiceManager::Deregister(const char* name) {
    return Deregister(std::string(name));
}

Service* ServiceManager::Deregister(const std::string &name) {
    auto it = services_.find(name);
    if (it != services_.end()) {
        auto service = it->second;
        services_.erase(it);
        return service;
    }
    return nullptr;
}

std::list<std::string> ServiceManager::GetSort() {
	return dependencyResolver_.GetSort(services_, STOPSORT);
}

bool ServiceManager::Init() {
	// Use topSort to resolve service dependency.
	std::list<std::string> lackServices = dependencyResolver_.GetLackServices(services_);
	while (!lackServices.empty()) {
		//load and register the missing services
		for (std::list<std::string>::iterator it = lackServices.begin(); it != lackServices.end(); ++it) {
			// TODO(shshang): load the lack services
			LOG(INFO) << "lack service " << *it << ", try to load.";
		}
		return false;
		//lackService = dependencyResolver_.GetLackServices(services_);
	}
	std::list<std::string> orders = dependencyResolver_.GetSort(services_, INITSORT);
	for (std::list<std::string>::iterator it = orders.begin(); it != orders.end(); it++)
	{
		LOG(INFO) << "[ServiceManager::haveService]  " << *it;
	}
	//dependency inject
	for (std::list<std::string>::iterator it = orders.begin(); it != orders.end(); ++it) {
		Service* service = services_[*it];
		std::set<std::string> dependency = service->GetDependentServices();
		for (std::set<std::string>::iterator dep = dependency.begin(); dep != dependency.end(); ++dep) {
			if (services_.find(*dep) != services_.end()) {
				std::string serviceName = *dep;
				service->Inject(serviceName, services_[*dep]);
			}
		}
	}
	//init
	for (std::list<std::string>::iterator it = orders.begin(); it != orders.end(); ++it) {
		Service* service = services_[*it];
		bool res = service->Init();
		LOG(INFO) << "[ServiceManager::init]  " << *it;
		assert(res);
		if (!res)
			return false;
	}
    
    initialized_ = true;
    return true;
}

bool ServiceManager::Start() {
    assert(initialized_);
    if (!initialized_) {
        return false;
    }
    // Use topSort to resolve service dependency.
    std::list<std::string> orders = dependencyResolver_.GetSort(services_, STARTSORT);
    for (std::list<std::string>::iterator it = orders.begin(); it != orders.end(); ++it) {
    	bool res = services_[*it]->Start();
    	assert(res);
    	if (!res)
    		return false;
    }

    running_ = true;
    return true;
}

bool ServiceManager::Stop() {
	// Use topSort to resolve service dependency.
	std::list<std::string> orders = dependencyResolver_.GetSort(services_, STOPSORT);
	for (std::list<std::string>::reverse_iterator it = orders.rbegin(); it != orders.rend(); ++it) {
	   	if (services_[*it]->IsRunning()) {
			bool res = services_[*it]->Stop();
			assert(res);
			if (!res)
				return false;
	   	}
	}

    running_ = false;
    return true;
}

bool ServiceManager::IsRunning() {
    return running_;
}

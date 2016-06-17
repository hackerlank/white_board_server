//
//  service.h
//  gateway
//
//  Created by Fubai Zhong on 10/4/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_SERVICE_H_
#define FRAMEWORK_BASE_SERVICE_H_

#include <set>
#include <memory>
#include <string>

#define MAX_SERVICE_COUNT 256

class Service {
public:
    Service() {}
    virtual ~Service() {}
    
public:
    virtual const std::set<std::string> GetDependentServices() const;
    virtual const std::set<std::string> GetUnresolvedDependentServices() const;
    virtual bool IsDependentServicesResolved() const;
    
    virtual void Inject(std::string& name, Service* service);
    
    virtual bool Init() = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool IsRunning() = 0;
};

#endif  // FRAMEWORK_BASE_SERVICE_H_

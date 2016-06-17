//
//  service.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "service.h"

const std::set<std::string> Service::GetDependentServices() const {
    return std::set<std::string>();
}

const std::set<std::string> Service::GetUnresolvedDependentServices() const {
    return std::set<std::string>();
}

bool Service::IsDependentServicesResolved() const {
    return true;
}

void Service::Inject(std::string& name, Service* service) {
}

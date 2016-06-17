/*
 * dependencyResolver.cpp
 *
 *  Created on: Nov 2, 2013
 *      Author: vagrant
 */

#include "dependencyResolver.h"

DependencyResolver::DependencyResolver() {
}

DependencyResolver::~DependencyResolver() {
}

std::map<std::string, Service*> DependencyResolver::GetNewServices(std::map<std::string, Service*> &services) {
	std::map<std::string, Service*> newServices;
	//find new service
	for (std::map<std::string, Service*>::iterator it = services.begin(); it != services.end(); ++it) {
		if (find(orders_.begin(), orders_.end(), (*it).first) == orders_.end()) {
			newServices[it->first] = it->second;
		}
	}
	return newServices;
}

std::list<std::string> DependencyResolver::GetLackServices(std::map<std::string, Service*> &services) {
	std::map<std::string, Service*> newServices = GetNewServices(services);
	std::list<std::string> lackServices;
	//find new service
	for (std::map<std::string, Service*>::iterator it = newServices.begin(); it != newServices.end(); ++it) {
		std::set<std::string> dependency = it->second->GetDependentServices();
		for (std::set<std::string>::iterator sit = dependency.begin(); sit != dependency.end(); ++sit) {
			if (!services.count(*sit))
				lackServices.push_back(*sit);
		}
	}
	return lackServices;
}

bool DependencyResolver::TopSort(std::map<std::string, Service*> services) {
	//topSort
	int32 totalServices = services.size();
	std::vector< std::set<std::string> > vecDependency = std::vector< std::set<std::string> >();
	std::map<int32, std::string> indexMapName = std::map<int32, std::string>();
	int32 index = 0;
	for (std::map<std::string, Service*>::const_iterator it = services.begin(); it != services.end(); ++it, ++index) {
		vecDependency.push_back(it->second->GetDependentServices());
		indexMapName[index] = it->first;
	}
	int32 graph[MAX_SERVICE_COUNT][MAX_SERVICE_COUNT];
	int32 out[MAX_SERVICE_COUNT];
	memset(graph, 0, sizeof(graph));
	memset(out, 0, sizeof(out));
	for (int32 i = 0; i < totalServices; i++) {
		for (int32 j = 0; j < totalServices; j++) {
			if (i != j && vecDependency[i].find(indexMapName[j]) != vecDependency[i].end()) {
				++graph[i][j];
				++out[i];
			}
		}
	}
	for (int32 i = 0; i < totalServices; i++) {
		int32 j = 0;
		while (j < totalServices && out[j] != 0) ++j;
		if (j >= totalServices) {
			return false; //find circle, return empty list
		}
		orders_.push_back(indexMapName[j]);
		out[j] = -1;
		for (int32 k = 0; k < totalServices; k++) {
			if (graph[k][j] > 0)
				--out[k];
		}
	}
	return true;
}


std::list<std::string> DependencyResolver::LoadInitSort(std::map<std::string, Service*> &services) {
	int16_t offset = orders_.size();
	if (TopSort(GetNewServices(services))) {
		auto it = orders_.begin();
		while (offset) {
			++it;
			--offset;
		}
		return std::list<std::string>(it, orders_.end());
	} else {
		LOG(INFO) << "in service, find dependency circle ";
		return std::list<std::string>();
	}
}


std::list<std::string> DependencyResolver::GetSort(std::map<std::string, Service*> &services, SORTTYPE type) {
	if (type == INITSORT) {
		return LoadInitSort(services);
	} else if (type == STARTSORT) {
		std::list<std::string>::iterator newServiceOffset = orders_.begin();
		for (; newServiceOffset != orders_.end(); ++newServiceOffset) {
			if (!services[*newServiceOffset]->IsRunning()) break;
		}
		return std::list<std::string>(newServiceOffset, orders_.end());
	} else if (type == STOPSORT) {
		return orders_;
	} else {
		return std::list<std::string>();
	}
}


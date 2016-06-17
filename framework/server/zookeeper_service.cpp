/*
 * zookeeper_service.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#include "zookeeper_service.h"

ZkService::ZkService(const std::string &namespace_name, const std::string &service_name, const std::string &node_name, const std::string &ips)
	: namespace_name_(namespace_name)
	, name_(service_name)
    , node_name_(node_name)
	, stat_(nullptr)
	, isRunning_(false)
	, local_node_(node_name, ips)
    , client_(namespace_name, name_)
{
	stat_ = reinterpret_cast<struct Stat*>(calloc(1, sizeof(struct Stat)));
}

ZkService::~ZkService() {
	if (stat_ != nullptr) {
		free(stat_);
		stat_ = nullptr;
	}
}

bool ZkService::Init() {
	client_.InitSession(&local_node_);
	return true;
}

bool ZkService::Start() {
	bool isOk = RegisterMonitor();
	if (!isOk) {
		return false;
	} else {
		isOk = RegisterlocalNode();
		if (!isOk) {
			return false;
		} else {
			isRunning_ = true;
			return true;
		}
	}
}

bool ZkService::Stop() {
	client_.CloseSession();
	isRunning_ = false;
	return true;
}

bool ZkService::IsRunning() {
	return isRunning_;
}

const rapidjson::Document& ZkService::GetConfigure() {
	bool isChanged = client_.GetIsChanged();
	client_.SetIsChanged(false);
	if (isChanged) {
		return local_node_.MergeConfigure();
	} else {
		return local_node_.GetConfigure();
	}
}

bool ZkService::IsChanged() {
	return client_.GetIsChanged();
}

bool ZkService::RegisterlocalNode() {
	std::string path("/");
	return client_.CreateNode(path.append(namespace_name_).append("/services/").append(name_));
}

bool ZkService::RegisterMonitor() {
	std::string path("/");
	return client_.MonitorNode(path.append(namespace_name_).append("/conf/").append(name_).append("/nodes/").append(local_node_.GetName()));
}

std::string ZkService::GetGameId()
{
    return node_name_;
}
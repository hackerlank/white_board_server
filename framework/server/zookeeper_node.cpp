/*
 * zookeeper_node.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#include "zookeeper_node.h"

ZkNode::ZkNode(const std::string &name, const std::string &ips) : name_(name), ips_(ips), stat_(nullptr) {
	stat_ = reinterpret_cast<struct Stat*>(calloc(1, sizeof(struct Stat)));
	info_.SetObject();
	rapidjson::Value ips_value(ips.c_str(), info_.GetAllocator());
	info_.AddMember("ips", ips_value, info_.GetAllocator());
	mergedConfig_.SetObject();
}

ZkNode::~ZkNode() {
	ResetRoles();
	if (stat_ != nullptr) {
		free(stat_);
		stat_ = nullptr;
	}
}

void ZkNode::ResetRoles() {
	for (auto it = roles_.begin(); it != roles_.end(); ++it) {
		if ((*it) != nullptr) {
			delete (*it);
			*it = nullptr;
		}
	}
	roles_.clear();
}

void ZkNode::Update(struct Stat *stat, char* data, int32_t len) {
	if (stat != nullptr) {
		memcpy(stat_, stat, sizeof(*stat_));
		free(stat);
		LOG(INFO) << "Local node(" << name_ << ") version has been updated to " << stat_->version;
	}
	if (data != nullptr) {
		const std::string msg(data, len);
		free(data);
		tmp_for_rapidjson_.SetObject();
		tmp_for_rapidjson_.Parse<0>(msg.c_str());
		if (tmp_for_rapidjson_.HasMember("roles")) { //may be not {} object
			if (info_.HasMember("roles")) {
				info_["roles"] = tmp_for_rapidjson_["roles"];
			} else {
				info_.AddMember("roles", tmp_for_rapidjson_["roles"], info_.GetAllocator());
			}
			UpdateRoles();
		} else {
			LOG(ERROR) << "Please Check Node Info(" << msg << ") Has Member Named \"roles\"";
		}
	}
}

void ZkNode::UpdateRoles() {
	ResetRoles();
	if (!info_.HasMember("roles")) {
		LOG(ERROR) << "Please check if Node Configure(Json) has a member named 'roles'";
	} else {
		if (info_["roles"].IsArray()) {
			for (int16_t i = 0, size = info_["roles"].Size(); i < size; ++i) {
				if (info_["roles"][i].IsArray() && info_["roles"][i].Size() == 2) {
					roles_.push_back(new ZkRole(info_["roles"][i][(rapidjson::SizeType)0].GetString(), info_["roles"][i][1].GetString()));
				} else {
					LOG(ERROR) << "Please check if Node Configure(Json)'s roles is in format [[name, version], [name, version], ...], Index: " << i;
				}
			}
		} else {
			LOG(ERROR) << "Please check if Node Configure(Json)'s roles is in format [[name, version], [name, version], ...]";
		}
	}
}

std::string ZkNode::GetName() {
	return name_;
}

std::string ZkNode::GetIps() {
	return ips_;
}

std::string ZkNode::GetInfo() {
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	info_.Accept(writer);
	return std::string(buffer.GetString());
}

int32_t ZkNode::GetVersion() {
	return stat_->version;
}

std::vector<ZkRole*> ZkNode::GetRoles() {
	return roles_;
}

const rapidjson::Document& ZkNode::MergeConfigure() {
	ZkRole::merge(&mergedConfig_, roles_);
	return GetConfigure();
}

const rapidjson::Document& ZkNode::GetConfigure() {
	return mergedConfig_;
}


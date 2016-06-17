/*
 * zookeeper_role.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#include "zookeeper_role.h"

ZkRole::ZkRole(const std::string &name, const std::string &version) : name_(name), version_(version), stat_(nullptr) {
	stat_ = reinterpret_cast<struct Stat*>(calloc(1, sizeof(struct Stat)));
    if (stat_ ==nullptr)
    {
        LOG(ERROR) << "[ZkRole::ZkRole] stat_ ==nullprt";
    }
	info_.SetObject();
}

ZkRole::~ZkRole() {
	if (stat_ != nullptr) {
		free(stat_);
		stat_ = nullptr;
	}
}

std::string ZkRole::GetName() {
	return name_;
}

std::string ZkRole::GetVersion() {
	return version_;
}

void ZkRole::Update(struct Stat *stat, char* data, int32_t len) {
	if (stat != nullptr && stat_!=nullptr) {
		memcpy(stat_, stat, sizeof(*stat_));
		free(stat);
		LOG(INFO) << "Local role(" << name_ << ", " << version_ << ") version has been updated to " << stat_->version;
    }
    else{
        LOG(ERROR) << "[ZkRole::Update] stat_ ==nullprt";
    }
	if (data != nullptr) {
		const std::string msg(data, len);
		info_.Parse<0>(msg.c_str());
		//LOG(INFO) << msg;
		free(data);
	}
}

void MergeJsonValue(rapidjson::Value *res, rapidjson::Value &addition, rapidjson::Document::AllocatorType &allocator) {
	//rapidjson::StringBuffer buffer;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	//addition.Accept(writer);
	//std::string checkRes = buffer.GetString();
	//LOG(INFO) << checkRes;

	if (addition.IsObject()) {
		for (rapidjson::Value::ConstMemberIterator iter = addition.MemberBegin(); iter != addition.MemberEnd(); ++iter) {
			if ((*res).HasMember(iter->name.GetString())) {
				//MergeJsonValue(&(*res)[iter->name.GetString()], addition[iter->name.GetString()], allocator);
				(*res).RemoveMember(iter->name.GetString());
			} //else {
			std::string temp_name = iter->name.GetString();
			rapidjson::Value name_value(temp_name.c_str(), allocator);
			(*res).AddMember(name_value, addition[temp_name.c_str()], allocator);
			//}
		}
	}

	
	//rapidjson::StringBuffer buffer;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	//res->Accept(writer);
	//std::string checkRes = buffer.GetString();
	//LOG(INFO) << "-----------------------1";
	//LOG(INFO) << checkRes;
}

void ZkRole::merge(rapidjson::Document *config, std::vector<ZkRole*> &roles) {	
	for (auto it = roles.begin(); it != roles.end(); ++it) {
		MergeJsonValue(config, (*it)->info_, config->GetAllocator());
	}
}


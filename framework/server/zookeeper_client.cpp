/*
 * zookeeper_client.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#include "zookeeper_client.h"

void ZkCallBack::session_event_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
        	LOG(INFO) << "Connected to ZooKeeper Server";
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            LOG(ERROR) << "Session Expired, Try to Reconnect...";
            if (watcherCtx != nullptr) {
            	NodeWthCtx* nwc_ = reinterpret_cast<NodeWthCtx*>(watcherCtx);
            	nwc_->client->CloseSession();
            	while (!nwc_->client->InitSession(nwc_->node)) {
            		LOG(ERROR) << "Reconnect Zookeeper Server Failed, Please Check Network";
            		LOG(ERROR) << "Try to reconnect in " << INTERNAL << "s...";
            		sleep(INTERNAL);
            	}
				nwc_->client->CreateNode(std::string("/").append(nwc_->namespace_name).append("/services/").append(nwc_->service_name).c_str());
				nwc_->client->MonitorNode(std::string("/").append(nwc_->namespace_name).append("/conf/").append(nwc_->service_name).append("/nodes/").append(nwc_->node->GetName()).c_str());
            }
        }
    }
}

void ZkCallBack::on_node_created(int32_t rc, const char *path, const void *data) {
	if (path == nullptr)
		return;
	if (ZOK == rc || ZNODEEXISTS == rc) {
		LOG(INFO) << "Create Node(" << path << ") Succeed";
	} else {
		LOG(ERROR) << "Create Node(" << path << ") Failed Error Code : " << rc;
		if (data != nullptr) {
			LOG(INFO) << "Try to recreate...";
			const NodeWthCtx* nwc_ = reinterpret_cast<const NodeWthCtx*>(data);
			std::string fatherPath(path);
			nwc_->client->CreateNode(fatherPath.substr(0, fatherPath.find_last_of('/')));
		} else {
			LOG(ERROR) << "Recreate Node(" << path << ") Failed : data in on_node_created is null, Please Check it";
		}
	}
}

void ZkCallBack::on_node_updated(int32_t rc, const struct Stat *stat, const void *data) {
	if (ZOK == rc) {
		LOG(INFO) << "Update Local Node Succeed";
	} else {
		LOG(ERROR) << "Update Local Node Failed Error Code : " << rc;
	}
}

void ZkCallBack::node_change_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx) {
	if (watcherCtx != nullptr) {
		NodeWthCtx* nwc_ = reinterpret_cast<NodeWthCtx*>(watcherCtx);
		if (ZOO_CHANGED_EVENT == type || ZOO_CREATED_EVENT == type) {
			if (nwc_->client->ReadNode(path)) {
				std::vector<ZkRole*> roles = nwc_->node->GetRoles();
				std::string role_path("/");
				role_path.append(nwc_->namespace_name).append("/conf/").append(nwc_->service_name).append("/roles");
				for (auto it = roles.begin(); it != roles.end(); ++it) {
					nwc_->client->ReadRole(role_path + "/" + (*it)->GetName() + "/" + (*it)->GetVersion(), (*it));
				}
				LOG(INFO) << "Process Node(" << path << ") Change|Create Event Succeed";
				nwc_->client->SetIsChanged(true);
			} else {
				LOG(INFO) << "Process Node(" << path << ") Change|Create Event Failed";
			}
		} else if (ZOO_DELETED_EVENT == type) {
			LOG(INFO) << "Node(" << path << ") Has Been Deleted";
			nwc_->client->MonitorNode(path);
		}
	} else {
		LOG(ERROR) << "Context in Node(" << path << ") Watcher is Null, Please Check it";
	}
}

void ZkCallBack::role_change_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx) {
	if (watcherCtx != nullptr) {
		if (ZOO_CHANGED_EVENT == type) {
			RoleWthCtx* rwc = reinterpret_cast<RoleWthCtx*>(watcherCtx);
			if (rwc->client->ReadRole(path, rwc->role)) {
				rwc->client->SetIsChanged(true);
				LOG(INFO) << "Process Role(" << path << ") Change Event Succeed";
			} else {
				LOG(INFO) << "Process Role(" << path << ") Change Event Failed";
			}
			rwc->client->DelRoleWthCtx(rwc);
		} else if (ZOO_DELETED_EVENT == type) {
			LOG(INFO) << "Role(" << path << ") has been deleted";
		} else if (ZOO_CREATED_EVENT == type) {
			LOG(INFO) << "Role(" << path << ") has been created";
		}
	} else {
		LOG(ERROR) << "Context in Role(" << path << ") Watcher is Null, Please Check it";
	}
}

void ZkCallBack::on_check_exist_done(int32_t rc, const struct Stat *stat, const void *data) {

}

ZkClient::ZkClient(const std::string &namespace_name,const std::string &service_name)
	: session_(nullptr)
	, isChanged_(false)
	, service_name_(service_name)
	, namespace_name_(namespace_name)
	, nwc_(nullptr)
{
	nwc_ = reinterpret_cast<NodeWthCtx*>(calloc(1, sizeof(NodeWthCtx)));
	nwc_->client = this;
	nwc_->service_name = service_name_.c_str();
	nwc_->namespace_name = namespace_name_.c_str();
}

ZkClient::~ZkClient() {
	if (nwc_ != nullptr) {
		free(nwc_);
		nwc_ = nullptr;
	}
	for (auto it = rwcs_.begin(); it != rwcs_.end(); ++it) {
		free(*it);
	}
}

void ZkClient::SetIsChanged(bool isChanged) {
	isChanged_ = isChanged;
}

bool ZkClient::GetIsChanged() {
	return isChanged_;
}

bool ZkClient::InitSession(ZkNode* node) {
	nwc_->node = node;
	session_ = zookeeper_init(node->GetIps().c_str(), &ZkCallBack::session_event_watcher, TIMEOUT, 0, nwc_, 0);
	return true;
}

bool ZkClient::CloseSession() {
	zookeeper_close(session_);
	return true;
}

bool ZkClient::CreateNode(std::string path) const {
	int32_t rc = zoo_create(session_, path.c_str(), 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
	if (ZOK == rc || ZNODEEXISTS == rc) {
		rc = zoo_acreate(session_, path.append("/").append(nwc_->node->GetName()).c_str(), 0, 0, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, &ZkCallBack::on_node_created, nwc_);
		if (ZOK == rc || ZNODEEXISTS == rc) {
			LOG(INFO) << "Async: Try to Create Node Path " << path << " Succeed";
			return true;
		} else {
			LOG(INFO) << "Async: Try to Create Node Path " << path << " Failed" << " Error Code : " << rc;
			return false;
		}
	} else {
		LOG(ERROR) << "Sync: Create Service Path " << path << " Failed" << " Error Code : " << rc;
		LOG(ERROR) << "Please check the zookeeper server based on the error code, try to recreate in " << INTERNAL << "s...";
		sleep(INTERNAL);
		return CreateNode(path);
	}
}

//path
bool ZkClient::UpdateNode() {
	std::string message = nwc_->node->GetInfo();
	std::string path("/");
	path.append(namespace_name_).append("/services/").append(service_name_).append("/").append(nwc_->node->GetName());
	LOG(INFO) << "To Update Version:" << nwc_->node->GetVersion();
	int32_t rc = zoo_aset(session_, path.c_str(), message.c_str(), message.size(), -1, &ZkCallBack::on_node_updated, 0);
	if (ZOK == rc) {
		LOG(INFO) << "Async: Try to Update Node(" << path << ") Succeed";
		return true;
	} else {
		LOG(ERROR) << "Async: Try to Update Node(" << path << ") Failed" << " Error Code : " << rc;
		return false;
	}
}

bool ZkClient::ReadNode(const std::string &path) {
	char *data = reinterpret_cast<char*>(calloc(1, ZNODE_INFO_CEILING));
	int32_t len = ZNODE_INFO_CEILING;
	struct Stat *stat = reinterpret_cast<struct Stat*>(calloc(1, sizeof(struct Stat)));;
	int32_t rc = zoo_wget(session_, path.c_str(), &ZkCallBack::node_change_watcher, nwc_, data, &len, stat);
	if (ZOK == rc) {
		nwc_->node->Update(stat, data, len);
		this->UpdateNode();
		LOG(INFO) << "Sync: Get Node(" << path << ") Succeed";
		return true;
	} else {
		LOG(ERROR) << "Sync: Get Node(" << path << ") Failed" << " Error Code : " << rc;
		if (data != nullptr) {
			free(data);
			data = nullptr;
		}
		return false;
	}
}

bool ZkClient::ReadRole(const std::string &path, ZkRole *role) {
	RoleWthCtx *rwc = reinterpret_cast<RoleWthCtx*>(calloc(1, sizeof(RoleWthCtx)));
	rwc->client = this;
	rwc->role = role;
	rwc->service_name = service_name_.c_str();
	rwc->namespace_name = namespace_name_.c_str();
	AddRoleWthCtx(rwc);
	char *data = reinterpret_cast<char*>(calloc(1, ZNODE_INFO_CEILING));
	int32_t len = ZNODE_INFO_CEILING;
	struct Stat *stat = reinterpret_cast<struct Stat*>(calloc(1, sizeof(struct Stat)));
	int32_t rc = zoo_wget(session_, path.c_str(), &ZkCallBack::role_change_watcher, rwc, data, &len, stat);
	if (ZOK == rc) {
		role->Update(stat, data, len);
		LOG(INFO) << "Sync: Get Role(" << path << ") Succeed";
		return true;
	} else {
		LOG(ERROR) << "Sync: Get Role(" << path << ") Failed" << " Error Code : " << rc;
		if (data != nullptr) {
			free(data);
			data = nullptr;
		}
		return false;
	}
}

bool ZkClient::MonitorNode(const std::string &path) {
	int32_t rc = zoo_awexists(session_, path.c_str(), &ZkCallBack::node_change_watcher, nwc_, &ZkCallBack::on_check_exist_done, 0);
	if (ZOK == rc) {
		LOG(INFO) << "Async: Try to Register Monitor On Node(" << path << ") Succeed";
		return true;
	} else {
		LOG(ERROR) << "Async: Try to Register Monitor On Node(" << path << ") Failed" << " Error Code : " << rc;
		LOG(ERROR) << "Please check the zookeeper server based on the error code, try to remonitor in " << INTERNAL << "s...";
		sleep(INTERNAL);
		return MonitorNode(path);
	}
}

void ZkClient::AddRoleWthCtx(RoleWthCtx* rwc) {
	g_rwcs_mutex_.lock();
	rwcs_.insert(rwc);
	g_rwcs_mutex_.unlock();
}

void ZkClient::DelRoleWthCtx(RoleWthCtx* rwc) {
	g_rwcs_mutex_.lock();
	rwcs_.erase(rwc);
	free(rwc);
	g_rwcs_mutex_.unlock();
}


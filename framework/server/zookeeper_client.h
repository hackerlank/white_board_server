/*
 * zookeeper_client.h
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_ZOOKEEPER_CLIENT_H_
#define FRAMEWORK_SERVER_ZOOKEEPER_CLIENT_H_

#include <unistd.h>
#include <string>
#include <mutex>
#include <set>
#include <zookeeper/zookeeper.h>

#include "zookeeper_node.h"
#include "zookeeper_role.h"
#include "logging.h"

#define ZNODE_INFO_CEILING 102400
#define TIMEOUT 4000
#define INTERNAL 10

class ZkCallBack {
public:
	static void session_event_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx);
	static void node_change_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx);
	static void role_change_watcher(zhandle_t* zh, int32_t type, int32_t state, const char* path, void* watcherCtx);
	static void on_node_created(int32_t rc, const char *path, const void *data);
	static void on_node_updated(int32_t rc, const struct Stat *stat, const void *data);
	static void on_check_exist_done(int32_t rc, const struct Stat *stat, const void *data);
};

class ZkClient;

typedef struct NodeWatcherCtx {
	ZkClient *client;
	ZkNode *node;
	const char* service_name;
	const char* namespace_name;
}NodeWthCtx;

typedef struct RoleWatcherCtx {
	ZkClient *client;
	ZkRole *role;
	const char* service_name;
	const char* namespace_name;
}RoleWthCtx;

class ZkClient {
public:
	ZkClient(const std::string &namespace_name, const std::string &service_name);
	virtual ~ZkClient();

	void SetIsChanged(bool isChanged);
	bool GetIsChanged();

public:
	bool InitSession(ZkNode* node);
	bool CloseSession();

	virtual bool CreateNode(std::string path) const; //async
	virtual bool UpdateNode(); //async

	virtual bool ReadNode(const std::string &path); //sync
	virtual bool ReadRole(const std::string &path, ZkRole *node); //sync

	bool MonitorNode(const std::string &path); //async

	void AddRoleWthCtx(RoleWthCtx* rwc);
	void DelRoleWthCtx(RoleWthCtx* rwc);

private:
	zhandle_t* session_;
 	bool isChanged_;
	std::string service_name_;
	std::string namespace_name_;

	NodeWthCtx *nwc_;
	std::set<RoleWthCtx*> rwcs_;
	std::mutex g_rwcs_mutex_;
};


#endif  // FRAMEWORK_SERVER_ZOOKEEPER_CLIENT_H_

/*
 * zookeeper_node.h
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_ZOOKEEPER_NODE_H_
#define FRAMEWORK_SERVER_ZOOKEEPER_NODE_H_

#include "zookeeper_role.h"

class ZkNode {
public:
	ZkNode(const std::string &name, const std::string &ips);
	virtual ~ZkNode();

public:
	virtual void Update(struct Stat *stat, char* data = 0, int32_t len = 0);

	std::string GetIps();
	std::string GetName();
	std::string GetInfo();
	int32_t GetVersion();
	std::vector<ZkRole*> GetRoles();

	const rapidjson::Document& MergeConfigure();
	const rapidjson::Document& GetConfigure();

protected:
	virtual void ResetRoles();
	virtual void UpdateRoles();

private:
	std::vector<ZkRole*> roles_; //order: version - older version - ...
	std::string name_;
	std::string ips_;
	struct Stat *stat_;
	rapidjson::Document info_;
	rapidjson::Document mergedConfig_;
	//TODO(Fubai) : resolve the invalid read caused by ~rapidjson::Document
	rapidjson::Document tmp_for_rapidjson_;
};


#endif  // FRAMEWORK_SERVER_ZOOKEEPER_NODE_H_

/*
 * zookeeper_role.h
 *
 *  Created on: Nov 21, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_SERVER_ZOOKEEPER_ROLE_H_
#define FRAMEWORK_SERVER_ZOOKEEPER_ROLE_H_

#include <cstring>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <zookeeper/zookeeper.h>

#include "logging.h"

class ZkRole {
public:
	static void merge(rapidjson::Document *config, std::vector<ZkRole*> &roles);

public:
	ZkRole(const std::string &name, const std::string &version);
	virtual ~ZkRole();

public:
	virtual void Update(struct Stat *stat, char* data = 0, int32_t len = 0);
	std::string GetName();
	std::string GetVersion();

private:
	std::string name_;
	std::string version_;
	struct Stat *stat_;
	rapidjson::Document info_;
};


#endif  // FRAMEWORK_SERVER_ZOOKEEPER_ROLE_H_

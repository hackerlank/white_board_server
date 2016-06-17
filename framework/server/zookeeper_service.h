/*
 * zookeeper_service.h
 *
 *  Created on: Nov 21, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_ZOOKEEPER_SERVICE_H_
#define FRAMEWORK_SERVER_ZOOKEEPER_SERVICE_H_

#include "service.h"
#include "zookeeper_node.h"
#include "zookeeper_client.h"

class ZkService : public Service {
public:
	ZkService(const std::string &namespace_name, const std::string &service_name, const std::string &node_name, const std::string &ips);
	virtual ~ZkService();

public:
    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();

    const rapidjson::Document& GetConfigure();
    bool IsChanged();
    std::string GetGameId();
protected:
    virtual bool RegisterlocalNode();
    virtual bool RegisterMonitor();

private:
    std::string namespace_name_;
    std::string name_;
    std::string node_name_;
    struct Stat *stat_;
    bool isRunning_;
    ZkNode local_node_;
    ZkClient client_;
};


#endif  // FRAMEWORK_SERVER_ZOOKEEPER_SERVICE_H_

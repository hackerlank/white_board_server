/*
 * game_module.h
 *
 *  Created on: Dec 26, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_GAME_SERVICE_H_
#define FRAMEWORK_SERVER_GAME_SERVICE_H_

#include <rapidjson/document.h>

#include <map>


#include "transcode_packet.h"
#include "transcode_protobuf.h"
#include "service.h"
#include "tcp_connection.h"

class GameService : public Service {
public:
	GameService();
	virtual ~GameService();

public:
	virtual bool Reload(rapidjson::Value& config) = 0;

public:
    virtual const std::set<std::string> GetDependentServices() const;
    virtual void Inject(std::string& name, Service* service);
    virtual bool IsDependentServicesResolved() const;

public:
    virtual bool Init() = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool IsRunning() = 0;

	virtual void Update();

	    virtual bool OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request);
	virtual bool OnRecvPacket(TcpConnection *conn, PacketTranscode &request);
	virtual bool OnConnClosed(TcpConnection *conn);

	//yujiding 2014/06/19 用于game_manager_service进行分发
	virtual bool OnRecvMsg(rapidjson::Value& msg);
};


#endif  // FRAMEWORK_SERVER_GAME_SERVICE_H_

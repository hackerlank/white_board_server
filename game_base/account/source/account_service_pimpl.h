/*
 * account_service_pimpl.h
 *
 *  Created on: Jan 8, 2014
 *      Author: vagrant
 */

#ifndef GAME_BASE_ACCOUNT_SOURCE_ACCOUNT_SERVICE_PIMPL_H_
#define GAME_BASE_ACCOUNT_SOURCE_ACCOUNT_SERVICE_PIMPL_H_

#include <rapidjson/document.h>

#include <map>
#include <string>
#include <set>
#include "transcode_protobuf.h"

#include "player_constructor.h"

class RabbitMQClient;
class PlayerManager;
class TcpConnection;
class Player;
class PlayerSession;
class PacketTranscode;
class AccountService;

class AccountServicePimpl {

public:
	explicit AccountServicePimpl(rapidjson::Value& conf);
	virtual ~AccountServicePimpl();

	virtual bool DropAllOnlinePlayer();

	virtual void KickAllPlayer();
protected:

    AccountService* account_service_;


protected:
	virtual bool Init();
    virtual bool Reload(rapidjson::Value& config);
	virtual void Destroy();

	virtual void Update();

		bool OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request);
	bool OnConnClosed(TcpConnection *conn);

	bool SendHeartBeat();

protected:

	virtual PlayerSession* CreatePlayerSession();
	virtual void DestroyPlayerSession(PlayerSession* playerSession);

private:
	std::map<std::string, TcpConnection*>::iterator AddTcpConnection(TcpConnection *tcpConnection);
	bool RemoveTcpConnection(const std::string &sessionId);
	TcpConnection* GetTcpConnectionBySessionId(const std::string &sessionId);

protected:
	rapidjson::Value conf_;
	PlayerManager *player_manager_;
	RabbitMQClient *account_rabbitmq_client_;
	RabbitMQClient *character_rabbitmq_client_;

	//来自GameServerManager的返回结果
	typedef int32_t(AccountServicePimpl::*RespHandler)(rapidjson::Value& json);
	std::map<uint32_t, RespHandler> resp_handler_;

	std::set<uint32_t> opcodes_;

	std::map<std::string, TcpConnection*> tcp_connection_list_;   	// sessionId ---  TcpConnection


	time_t last_heart_beat_time_;
	int32_t heart_beat_interval_;

	bool can_login_;

	void *handle_;

private:
	friend class AccountService;
};


#endif  // GAME_BASE_ACCOUNT_SOURCE_ACCOUNT_SERVICE_PIMPL_H_

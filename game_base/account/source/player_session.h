 
#ifndef GAME_BASE_ACCOUNT_SOURCE_PLAYER_SESSION_H_
#define GAME_BASE_ACCOUNT_SOURCE_PLAYER_SESSION_H_

#include <rapidjson/document.h>

#include <map>

#include "transcode_protobuf.h"
#include "tcp_connection.h"
#include "account_opcode_def.h"

class RabbitMQClient;
class Room;


class PlayerSession {
public:
	PlayerSession();
	virtual ~PlayerSession();

public:
	virtual int32_t HandleRequest(size_t msgtype, ProtobufTranscode &request);

	virtual	TcpConnection*	GetTcpConnection(); 
	virtual std::string GetRemoteAddress();
	virtual std::string GetUsrName();
	virtual void  SendErrorToClient(int32_t error_code,int32_t extra_info,std::string error_description);
	
	virtual void SetRoom(Room* room);
	
protected:
	virtual int32_t HandleAddShape(ProtobufTranscode &request);
	virtual int32_t HandleRoomOperate(ProtobufTranscode &request);
	virtual int32_t HandleRemoveShape(ProtobufTranscode &request);
public:
	bool SendTranscode(PacketTranscode &packet);
	bool SendTranscode(const ProtobufTranscode& packet);
	std::string GetSessionId();

private:
	void PlayerAskRoomList();
	void PlayerCreateRoom(const std::string& room_key);
	void PlayerEnterRoom(const std::string& room_key);
	void PlayerDeleteRoom(const std::string& room_key);
	void PlayerLeaveRoom();
	void PlayerCopyRoom(const std::string& room_key_src,const std::string& room_key_des);

	void PlayerSyncPlayerList(const std::string& room_key);
	
protected:
	TcpConnection *conn_;
	std::string sessionId_;
	Room* room_;
	std::string usr_name_;

private:
	friend class PlayerManager;
	friend class AccountServicePimpl;
	friend class AccountService;

};

#endif  // GAME_BASE_ACCOUNT_SOURCE_PLAYER_SESSION_H_

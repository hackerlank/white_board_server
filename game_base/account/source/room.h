#ifndef ROOM_H_
#define ROOM_H_

#include <set>
#include <map>
#include <time.h>

#include "player_session.h"

class ProtobufTranscode;

class Room{
public:
	Room(std::string room_key);
	~Room();
	
	int32_t PlayerAddShape(PlayerSession* session,const ProtobufTranscode& packet);
	int32_t PlayerRemoveShape(PlayerSession* session,const ProtobufTranscode& packet);	
	
	void BroadCast(const ProtobufTranscode& packet);
	
	void EnterRoom(PlayerSession* session);
	void LeaveRoom(PlayerSession* session);
	
	int32_t GetPlayerSize();
	std::string GetRoomKey();
	
	bool CanDelete();
	
	void SyncPlayerList(PlayerSession* session);
	void BroadCastPlayerList();
	
	void ForceDeleteRoom();
private:	
	void SyncShapeObjectInfo(PlayerSession* session);	
public:
	std::string room_key_;
	std::set<PlayerSession*> sessions_;
	std::map<uint32_t,std::string> shape_object_;//图元的序列化结果
	
	uint32_t shape_max_id_;
	time_t can_delete_time_;
};

#endif  // ROOM_H_

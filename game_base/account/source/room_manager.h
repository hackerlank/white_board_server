#ifndef ROOM_MANAGER_H_
#define ROOM_MANAGER_H_

#include <map>
#include <time.h>
#include "singleton.h"
#include "room.h"

class RoomManager : public Singleton <RoomManager>
{
protected:
	RoomManager();
	~RoomManager();
	
public:
	
	void PlayerAskRoomList(PlayerSession* session);
	void BroadCastRoomList();
	void PlayerCreateRoom(const std::string &room_key,PlayerSession* session);
	void PlayerEnterRoom(const std::string & room_key,PlayerSession* session);
	void PlayerDeleteRoom(const std::string & room_key,PlayerSession* session);
	
	void PlayerCopyRoom(const std::string &room_key_src,const std::string &room_key_des,PlayerSession* session);
	void PlayerSyncPlayerList(const std::string& room_key,PlayerSession* session);
	
	void Update();

private:
	Room* CreateRoom(const std::string& room_key);
	void BroadCast(Room* room,const ProtobufTranscode& packet);
	Room* CopyRoom(const std::string& room_src,const std::string &room_des);
private:
	std::map<std::string,Room*> rooms_;
private:
	friend class Singleton<RoomManager>;
};







#endif  // ROOM_MANAGER_H_

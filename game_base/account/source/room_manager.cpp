#include "room_manager.h"
#include "transcode_protobuf.h"
#include "account_opcode_def.h"
#include "ErrorResp.pb.h"
#include "RoomListResp.pb.h"

RoomManager::RoomManager(){

}

RoomManager::~RoomManager(){
	for(auto it =rooms_.begin();it!=rooms_.end();it++){
		delete (it->second);
		it->second = nullptr;
	}
	rooms_.clear();
}


Room* RoomManager::CreateRoom(const std::string& room_key){
	
	Room* room =new Room(room_key);
	rooms_.insert(std::make_pair(room_key,room));
	
	return room;
}


void RoomManager::BroadCast(Room* room,const ProtobufTranscode& packet){
	room->BroadCast(packet);
}


void RoomManager::PlayerDeleteRoom(const std::string & room_key,PlayerSession* session){
	auto it = rooms_.find(room_key);
	if(it==rooms_.end()){	
		session->SendErrorToClient(Shape_Delete_Room_Not_Exist,ProtoBuff_Room_Operate,ERROR_DELETE_ROOM_NOT_EXIST);
	}else{
		it->second->ForceDeleteRoom();
		session->SendErrorToClient(shape_Room_Operation_Success, ProtoBuff_Room_Operate, SUCCESS_ROOM_OPERATION);		
	}
}

void RoomManager::PlayerAskRoomList(PlayerSession* session){
	RoomListResp resp;
	for(auto it =rooms_.begin();it!= rooms_.end();it++){
		resp.add_room_list(it->first);
	}
	session->SendTranscode(ProtobufTranscode(ProtoBuff_Sync_Room_List,resp.SerializeAsString()));	
}

void RoomManager::BroadCastRoomList(){
	RoomListResp resp;
	for(auto it =rooms_.begin();it!= rooms_.end();it++){
		resp.add_room_list(it->first);
	}
	ProtobufTranscode room_list_packet(ProtoBuff_Sync_Room_List,resp.SerializeAsString());
	for(auto it =rooms_.begin();it!= rooms_.end();it++){
		it->second->BroadCast(room_list_packet);
	}
}

void RoomManager::PlayerCreateRoom(const std::string &room_key,PlayerSession* session){
	auto it = rooms_.find(room_key);
	if(it!=rooms_.end()){	
		session->SendErrorToClient(Shape_Create_Room_When_Key_Exist,ProtoBuff_Room_Operate,ERROR_CREATE_ROOM_EXIST);
	}else{
		CreateRoom(room_key);
		BroadCastRoomList();
		session->SendErrorToClient(shape_Room_Operation_Success, ProtoBuff_Room_Operate, SUCCESS_ROOM_OPERATION);
	}
}

void RoomManager::PlayerSyncPlayerList(const std::string& room_key,PlayerSession* session){
	auto it = rooms_.find(room_key);	
	if(it==rooms_.end()){
		session->SendErrorToClient(Shape_Sync_Player_List_No_Room,ProtoBuff_Sync_Player_List,ERROR_SYNC_PLAYER_LIST_NO_ROOOM);
	}else{
		it->second->SyncPlayerList(session);
	}	
}


void RoomManager::PlayerEnterRoom(const std::string &room_key,PlayerSession* session){
	auto it = rooms_.find(room_key);
	if(it==rooms_.end()){
		session->SendErrorToClient(Shape_Enter_Room_Not_Exist,ProtoBuff_Room_Operate,ERROR_ENTER_ROOM_NOT_EXIST);
	}else{
		it->second->EnterRoom(session);
		session->SendErrorToClient(shape_Room_Operation_Success, ProtoBuff_Room_Operate, SUCCESS_ROOM_OPERATION);
	}
}

void RoomManager::PlayerCopyRoom(const std::string &room_key_src,
	const std::string &room_key_des,PlayerSession* session){
	auto it_src = rooms_.find(room_key_src);
	auto it_des = rooms_.find(room_key_des);
	if(it_src == rooms_.end()){
		session->SendErrorToClient(Shape_Copy_Room_When_Src_Key_No_Exist,ProtoBuff_Room_Operate,ERROR_COPY_ROOM_SRC_NOT_EXIST);
		return;
	}
	if(it_des != rooms_.end()){
		session->SendErrorToClient(Shape_Copy_Room_When_Des_Key_Exist,ProtoBuff_Room_Operate,ERROR_COPY_ROOM_DES_EXIST);
		return;		
	}
	Room* room_des = CopyRoom(room_key_src,room_key_des);
	rooms_.insert(std::make_pair(room_key_des,room_des));
	BroadCastRoomList();
	session->SendErrorToClient(shape_Room_Operation_Success, ProtoBuff_Room_Operate, SUCCESS_ROOM_OPERATION);
}

Room* RoomManager::CopyRoom(const std::string& room_src_key,const std::string &room_des_key){
	Room* room_des =new Room(room_des_key);
	auto it_src = rooms_.find(room_src_key);
	Room* room_src = it_src->second;
	room_des->shape_object_ = room_src->shape_object_;
	room_des->shape_max_id_ = room_src->shape_max_id_;
	return room_des;
}

void RoomManager::Update(){
	for(auto it =rooms_.begin();it!= rooms_.end();){
		if(it->second->CanDelete()){
			delete it->second;
			it->second=nullptr;
			it = rooms_.erase(it);	
			BroadCastRoomList();
		}else{
			it++;
		}
	}
}

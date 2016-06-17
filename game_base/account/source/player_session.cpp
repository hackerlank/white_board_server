/*
 * player_session.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: shshang
 */
#include "Login.pb.h"
#include "WhiteBoardMessage.pb.h"
#include "RoomOperate.pb.h"
#include "ErrorResp.pb.h"

#include "room.h"
#include "room_manager.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "rabbitmq_client.h"
#include "player_session.h"
#include "account_opcode_def.h"

#include "game_server_service.h"
#include "service_manager.h"
#include "beluga_track.h"
#include "cb_time.h"



typedef int32_t (PlayerSession::*ReqHandler)(ProtobufTranscode&);
static std::map<uint32_t, ReqHandler > req_handler_;

typedef int32_t (PlayerSession::*RespHandler)(rapidjson::Value &);
static std::map<uint32_t, RespHandler > resp_handler_;

PlayerSession::PlayerSession()
	: conn_(nullptr)
	, room_(nullptr)
	, usr_name_("")
	{

	static bool handler_initialized_ = false;
	if (!handler_initialized_) {
		handler_initialized_ = true;
		
		req_handler_[ProtoBuff_Room_Operate]=&PlayerSession::HandleRoomOperate;
		req_handler_[ProtoBuff_Add_Sharp]=&PlayerSession::HandleAddShape;
		req_handler_[ProtoBuff_Remove_Sharp]=&PlayerSession::HandleRemoveShape;
	}
}

PlayerSession::~PlayerSession() {
	PlayerLeaveRoom();
}

std::string PlayerSession::GetUsrName(){
	return usr_name_;
}

std::string PlayerSession::GetSessionId(){
	std::string tmp;
	tmp = GetRemoteAddress() + "  " + usr_name_ + "   " + sessionId_;
	return tmp;
}

int32_t PlayerSession::HandleAddShape(ProtobufTranscode &request){
	
	if(room_){
		room_->PlayerAddShape(this,request);
	}else{
		SendErrorToClient(Shape_No_In_Room,ProtoBuff_Add_Sharp,ERROR_NOT_IN_ROOM);	
		LOG(ERROR) << "[PlayerSession::HandleAddShape] player not in room "<< GetSessionId();
	}
	return 1;
}

int32_t PlayerSession::HandleRemoveShape(ProtobufTranscode &request){
	if(room_){
		room_->PlayerRemoveShape(this,request);
	}else{
		SendErrorToClient(Shape_No_In_Room,ProtoBuff_Remove_Sharp,ERROR_NOT_IN_ROOM);
		LOG(ERROR) << "[PlayerSession::HandleRemove] player not in room "<< GetSessionId();
	}
	return 1;
}


TcpConnection*	PlayerSession::GetTcpConnection()
{
	return conn_; 
}

int32_t PlayerSession::HandleRequest(size_t msgtype, ProtobufTranscode &request) {
	size_t opcode = msgtype;
	auto it = req_handler_.find(opcode);
	LOG(INFO) << "[PlayerSession::HandleRequest] get opcode   " << opcode;
	if (it != req_handler_.end()) {
		return (this->*(it->second))(request);
	} 
	else 
	{
		LOG(ERROR) << " TcpConnection " << this->GetTcpConnection() << " HandleRequest, OpCode " << opcode ;
		return SessionHandleError;
	}
}

bool PlayerSession::SendTranscode(const ProtobufTranscode& packet)
{
if (conn_ != nullptr) {
		conn_->Write(packet);
		return true;
	} else {
		//查找该log进行错误定位
		return false;
	}
}

bool PlayerSession::SendTranscode(PacketTranscode &packet) {
	//assert(conn_ != nullptr);
    LOG(INFO) << "[PlayerSession::SendTranscode] send opcode   " << packet.GetOpcode();
	if (conn_ != nullptr) {
		conn_->Write(packet);
		return true;
	} else {
        //查找该log进行错误定位
        LOG(ERROR) << " AccoouSendTranscode ERROR "; 
		return false;
	}
}

void  PlayerSession::SendErrorToClient(int32_t error_code,int32_t extra_info,std::string error_description){
	ErrorResp resp;
	resp.set_errorcode(error_code);
	resp.set_extrainfo(extra_info);
	resp.set_errordescription(error_description);
	this->SendTranscode(ProtobufTranscode(ProtoBuff_Error_Info, resp.SerializeAsString()));
}


void PlayerSession::SetRoom(Room* room){
	room_ =room;
}

/*
oprate_type
1 请求房间列表
2 创建房间
3 进入房间
4 离开房间
5 销毁房间
6 拷贝房间
7 请求房间内玩家列表
*/
int32_t PlayerSession::HandleRoomOperate(ProtobufTranscode &request){
	
	RoomOperate room_operate;
	if(!room_operate.ParseFromString(request.GetBody())){
		SendErrorToClient(Shape_Packet_Wrong,ProtoBuff_Room_Operate,ERROR_PACKET_WRONG);
		LOG(ERROR) << "[PlayerSession::HandleRoomOperate] get body_wrong session:" << GetSessionId();		
		
	}
	
	int32_t operate_type = room_operate.oprate_type();
	std::string usr_name = room_operate.use_name();
	std::string room_src = room_operate.room_key_origin();
	std::string room_des = room_operate.room_key_destination();
	
	LOG(INFO) << "[PlayerSession::HandleRoomOperate] operate= "<< operate_type << " usr name =" <<usr_name
		<< "  room_src=" << room_src << "  room_des=" <<room_des;
	
	usr_name_ = usr_name;
	
	switch (operate_type)
	{
	case 1:
		PlayerAskRoomList();
		break;
	case 2:
		PlayerCreateRoom(room_src);
		break;
	case 3:
		PlayerEnterRoom(room_src);
		break;	
	case 4:
		PlayerLeaveRoom();
		break;		
	case 5:
		PlayerDeleteRoom(room_src);
		break;		
	case 6:
		PlayerCopyRoom(room_src,room_des);
		break;		
	case 7:
		PlayerSyncPlayerList(room_src);
		break;
	default:
		break;
	}
		
	return 1;
}

void PlayerSession::PlayerAskRoomList(){
	RoomManager::instance()->PlayerAskRoomList(this);
}

void PlayerSession::PlayerCreateRoom(const std::string& room_key){
	RoomManager::instance()->PlayerCreateRoom(room_key,this);
}

void PlayerSession::PlayerEnterRoom(const std::string& room_key){
	if(room_!=nullptr){
		room_->LeaveRoom(this);	
	}

	RoomManager::instance()->PlayerEnterRoom(room_key,this);
}

void PlayerSession::PlayerDeleteRoom(const std::string& room_key){
	RoomManager::instance()->PlayerDeleteRoom(room_key,this);
}

void PlayerSession::PlayerLeaveRoom(){
	if(room_!=nullptr){
		room_->LeaveRoom(this);
	}else{
		SendErrorToClient(Shape_Leave_When_not_In_Room,ProtoBuff_Room_Operate,ERROR_NOT_IN_ROOM);
		LOG(ERROR) << "[PlayerSession::PlayerLeaveRoom] leave but not in any_room :" << GetSessionId();		
	}
}


void PlayerSession::PlayerCopyRoom(const std::string& room_key_src,const std::string& room_key_des){
	RoomManager::instance()->PlayerCopyRoom(room_key_src,room_key_des,this);
}

void PlayerSession::PlayerSyncPlayerList(const std::string& room_key){
	RoomManager::instance()->PlayerSyncPlayerList(room_key,this);
}

std::string PlayerSession::GetRemoteAddress()
{
	std::string ip;
	TcpConnection* conn = GetTcpConnection();
	if(conn)
	{
		ip = conn->remote_address();
	}
	return ip;
}

#include "WhiteBoardMessage.pb.h"
#include "RemoveShape.pb.h"
#include "room.h"
#include "transcode_protobuf.h"
#include "account_opcode_def.h"
#include "PlayerListResp.pb.h"



Room::Room(std::string room_key)
:room_key_(room_key)
,shape_max_id_(1)
,can_delete_time_(0)
{

}

Room::~Room(){
	
}

void Room::BroadCast(const ProtobufTranscode& packet){
	for (auto it =sessions_.begin();it!=sessions_.end();it++){
		if((*it)!=nullptr){
			(*it)->SendTranscode(packet);
		}
	}
}


void Room::EnterRoom(PlayerSession* session){
	if(sessions_.find(session)==sessions_.end()){
		can_delete_time_ =0;
		sessions_.insert(session);
		session->SetRoom(this);	
		SyncShapeObjectInfo(session);
		BroadCastPlayerList();
	}

}


void Room::LeaveRoom(PlayerSession* session){
	for (auto it =sessions_.begin();it!=sessions_.end();){
		if((*it) == session){
			it = sessions_.erase(it);
			session->SetRoom(nullptr);			
			BroadCastPlayerList();
			break;
		}else{
			it++;
		}
	}
	if(GetPlayerSize()==0){// 房间为空 1小时后可以删除
		can_delete_time_ = time(nullptr) +3600;
	}
}

bool Room::CanDelete(){
	if(GetPlayerSize()==0 && can_delete_time_!=0 && time(nullptr) > can_delete_time_ ){
		return true;
	}
	return false;
}

int32_t Room::GetPlayerSize(){
	return sessions_.size();
}

std::string Room::GetRoomKey(){
	return room_key_;
}

void Room::SyncShapeObjectInfo(PlayerSession* session){
	if(shape_object_.empty()){
		return;
	}
	WhiteBoardMessageList resp;
	for(auto it =shape_object_.begin();it!= shape_object_.end();it++){
		WhiteBoardMessage* obj=resp.add_shapeobject();
		obj->ParseFromString(it->second);
	}
	session->SendTranscode(ProtobufTranscode(ProtoBuff_Add_Sharp,resp.SerializeAsString()));	
}

void Room::SyncPlayerList(PlayerSession* session){
	PlayerListResp list_player;
	for(auto it = sessions_.begin();it!=sessions_.end();++it){
		list_player.add_player_list((*it)->GetUsrName());
	}
	ProtobufTranscode list_packet(ProtoBuff_Sync_Player_List,list_player.SerializeAsString());
	session->SendTranscode(list_packet);

}

void Room::BroadCastPlayerList(){
	PlayerListResp list_player;
	for(auto it = sessions_.begin();it!=sessions_.end();++it){
		list_player.add_player_list((*it)->GetUsrName());
	}
	ProtobufTranscode list_packet(ProtoBuff_Sync_Player_List,list_player.SerializeAsString());
	BroadCast(list_packet);
}

void Room::ForceDeleteRoom(){
	for (auto it =sessions_.begin();it!=sessions_.end();it++){
		LeaveRoom(*it);
		(*it)->SendErrorToClient(Shape_Room_Delete_You_Out,ProtoBuff_Room_Operate,ERROR_ROOM_DELET);
	}
	can_delete_time_ = time(nullptr);
}

int32_t Room::PlayerAddShape(PlayerSession* session,const ProtobufTranscode& request){

	WhiteBoardMessageList req_list;

	uint32_t shape_id =0;
	if(req_list.ParseFromString(request.GetBody())){

		for(int i =0;i< req_list.shapeobject_size();i++){
			WhiteBoardMessage* req = req_list.mutable_shapeobject(i);
			shape_id =  req->shapeid();	
			std::string type = req->shapetype();
			uint32_t datasize, propsize;
			datasize = propsize = 0;
			if (req->has_shapedata()) {
				datasize += req->shapedata().size();
			}
			if (req->has_shapeproperty()) {
				propsize += req->shapeproperty().size();
			}
			auto it = shape_object_.find(shape_id);
			if(it != shape_object_.end()){
				it->second =  req->SerializeAsString();
				LOG(INFO) <<"[Room::AddShape] change shape , id = " << shape_id << ", type = " << type << ", datasize = " << datasize << ", propsize = " << propsize;
			}else{
				//添加一个对象
				shape_id = shape_max_id_;
				req->set_shapeid(shape_id);
				shape_object_.insert(std::make_pair(shape_id,req->SerializeAsString()));
				shape_max_id_++;
				LOG(INFO) << "[Room::AddShape] add shape,id = " << shape_id  << ", type = " << type << ", datasize = " << datasize << ", propsize = " << propsize ;
			}
		
		}

		BroadCast(ProtobufTranscode(ProtoBuff_Add_Sharp,req_list.SerializeAsString()));
	}else{
		session->SendErrorToClient(Shape_Packet_Wrong,ProtoBuff_Add_Sharp,ERROR_PACKET_WRONG);
		LOG(ERROR) << "[Room::AddShape] get body_wrong session:" << session->GetSessionId();
	}
	return shape_id;
}

int32_t Room::PlayerRemoveShape(PlayerSession* session,const ProtobufTranscode& request){
	uint32_t shape_id =0;
	
	RemoveShape req;
	if(req.ParseFromString(request.GetBody())){
		shape_id = req.shapeid();
		auto it =shape_object_.find(shape_id);
		if(it== shape_object_.end()){
			session->SendErrorToClient(Shape_Id_Not_Exist,shape_id,ERROR_SHAPE_ID_NOT_EXIST);
			LOG(ERROR) << "[Room::RemoveShape] the shappe  " << shape_id << " not exits"; 
		}else{
			shape_object_.erase(it);
			BroadCast(ProtobufTranscode(ProtoBuff_Remove_Sharp,req.SerializeAsString()));		
		}
	}else{
		session->SendErrorToClient(Shape_Packet_Wrong,ProtoBuff_Remove_Sharp,ERROR_PACKET_WRONG);
		LOG(ERROR)<<"[Room::RemoveShape] get body_wrong session:" << session->GetSessionId();
	}
	

	return shape_id;
}



/*
 * account_service_pimpl.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: vagrant
 */

#include <csv_parser/csv_parser.h>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <dlfcn.h>

#include <algorithm>


#include "account_opcode_def.h"
#include "account_service_pimpl.h"
#include "tcp_connection.h"
#include "rabbitmq_client.h"
#include "player_session.h"
#include "transcode_packet.h"
#include "player_constructor.h"
#include "player_manager.h"
#include "game_manager_service.h"
#include "service_manager.h"
#include "CbFormat.h"
#include "zookeeper_service.h"
#include "rapidjson_utility.h"

#define HEART_BEAT_INTERVAL 30 //s

AccountServicePimpl::AccountServicePimpl(rapidjson::Value& conf)
: player_manager_(nullptr)
, account_rabbitmq_client_(nullptr)
, character_rabbitmq_client_(nullptr)
, last_heart_beat_time_(0)
, heart_beat_interval_(HEART_BEAT_INTERVAL)
, can_login_(true)
, handle_(nullptr)
{
	conf_  = conf;
}

AccountServicePimpl::~AccountServicePimpl() {
	if (account_rabbitmq_client_ != nullptr) {
		delete account_rabbitmq_client_;
		account_rabbitmq_client_ = nullptr;
	}
	if (character_rabbitmq_client_ != nullptr) {
		delete character_rabbitmq_client_;
		character_rabbitmq_client_ = nullptr;
	}
		if (player_manager_ != nullptr) {
						delete player_manager_;
								player_manager_ = nullptr;
									}
}

bool AccountServicePimpl::Reload(rapidjson::Value& config){
    if (!config.IsObject()) {
        LOG(ERROR) << "[AccountServicePimpl::Reload]  account service pimp conf is not a object";
        return false;
    }
	
    return true;
}

bool AccountServicePimpl::Init() {
	LOG(INFO) << "[AccountServicePimpl::Init] account service pimp init";


	opcodes_.insert(ProtoBuff_Add_Sharp);
	opcodes_.insert(ProtoBuff_Room_Operate);
	opcodes_.insert(ProtoBuff_Remove_Sharp);

	rapidjson::Value& conf = conf_["Pimpl"];

	if (!conf.IsObject()) {
		LOG(ERROR) << "account service pimp conf is not a object";
		return false;
	}
	std::string app_id = "whit_board";
		player_manager_ = new PlayerManager([&]() { return CreatePlayerSession();},
													[&](PlayerSession* playerSession) { DestroyPlayerSession(playerSession); },
													app_id);
			player_manager_->Init();
    return Reload(conf);
}


void AccountServicePimpl::Destroy() {
	if (account_rabbitmq_client_ == nullptr || character_rabbitmq_client_ == nullptr) {
		return;
	}
	if (!account_rabbitmq_client_->Stop()) {
		return;
	}
	if (!character_rabbitmq_client_->Stop()) {
		return;
	}
	return;
}

void AccountServicePimpl::Update() {

}


bool AccountServicePimpl::OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request) {
	if (!can_login_)
	{
		return false;
	}

	std::string sessionId = conn->GetSessionId();

	PlayerSession* player_session = player_manager_->GetPlayerSessionBySessionId(sessionId);

	if (player_session == nullptr) {
		return false;
	}

	size_t msgtype = request.GetOpcode();

	LOG(DEBUG) << "[AccountServicePimpl::OnRecvPacket]account service pimp on recv packet  " << msgtype;
	player_session->HandleRequest(msgtype, request);

	//if (player_manager_->TriggerFunc(player_session, msgtype, TRIGGER_TYPE_BEFORE) == TRIGGER_RESULT_OPCODE_SUCCESS) {
	//	player_session->HandleRequest(msgtype, request);
	//	player_manager_->TriggerFunc(player_session, msgtype, TRIGGER_TYPE_AFTER);
	//}
	return true;
}

bool AccountServicePimpl::OnConnClosed(TcpConnection *conn) {
	std::string sessionId = conn->GetSessionId();
	//bool result = player_manager_->OnPlayerDropped(sessionId);
	RemoveTcpConnection(sessionId);
	return true;
}

std::map<std::string, TcpConnection*>::iterator AccountServicePimpl::AddTcpConnection(TcpConnection *tcpConnection) {
	// save tcp connection for response
	std::string sessionId = tcpConnection->GetSessionId();
	tcp_connection_list_[sessionId] = tcpConnection;
	auto it = tcp_connection_list_.find(sessionId);
	auto it_player_session = player_manager_->AddPlayerSession(sessionId);
	it_player_session->second->conn_ = it->second;
	return it;
}

bool AccountServicePimpl::RemoveTcpConnection(const std::string &sessionId) {
	auto it = tcp_connection_list_.find(sessionId);
	if (it != tcp_connection_list_.end()) {
		tcp_connection_list_.erase(it);
	} else {
		return false;
	}
	player_manager_->RemovePlayerSession(sessionId);
	return true;
}

TcpConnection* AccountServicePimpl::GetTcpConnectionBySessionId(const std::string &sessionId) {
	auto it = tcp_connection_list_.find(sessionId);
	if (it == tcp_connection_list_.end()) {
		return nullptr;
	}
	return it->second;
}




PlayerSession* AccountServicePimpl::CreatePlayerSession() {
	return new PlayerSession();
}
void AccountServicePimpl::DestroyPlayerSession(PlayerSession* playerSession) {
	delete playerSession;
}



bool AccountServicePimpl::DropAllOnlinePlayer()
{
	//LOG(DEBUG) << "[AccountServicePimpl::DropAllOnlinePlayer]";
	//for (auto it = player_manager_->online_users_.begin(); it != player_manager_->online_users_.end(); ++it)
	//{
	//	TcpConnection* connection = nullptr;
	//	if (it->second && (connection = it->second->GetConnection()))
	//	{
	//		connection->Close();
	//	}
	//}

	return true;
}



void AccountServicePimpl::KickAllPlayer()
{
	LOG(INFO)<<"AccountServicePimpl::KickAllPlayer";
}


/*
 * player_manager.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: vagrant
 */
#include "global_error_code.h"
#include "player_manager.h"
#include "player_session.h"
#include  "CbFormat.h"

#define GAME_SERVER_SESSION_ID "GAME_SERVER_SESSION_ID"

#define DEFAULT_ACCOUNT_ID 0
#define DEFAULT_USER_NAME "DEFAULT_NAME"
#define GAME_ONLINE_PLAYER_NUM_STATS_SECONDS 60

PlayerManager::PlayerManager(
player_session_creator_func player_session_creator,
player_session_deletor_func player_session_deletor,
std::string& gameId)

: player_session_creator_(player_session_creator)
, player_session_deletor_(player_session_deletor)
{
}

PlayerManager::~PlayerManager() {
	auto player_sessions_end = player_sessions_.end();
	for (auto it = player_sessions_.begin(); it != player_sessions_end; ++it) {
		if (it->second != nullptr) {
			player_session_deletor_(it->second);
			it->second = nullptr;
		}
	}
}

void PlayerManager::Init() {

}


void PlayerManager::Update() {

	//player.update
//	auto it_end = online_users_.end();
//	for (auto it = online_users_.begin(); it != it_end; ++it) {
//		it->second->Update();
//	}
}

std::map<std::string, PlayerSession*>::iterator PlayerManager::AddPlayerSession(const std::string &sessionId) {
	auto it = player_sessions_.find(sessionId);
	if (it == player_sessions_.end()) {
		LOG(INFO) << "[PlayerManager::AddPlayerSession] new PlayerSession(), SessionId " << sessionId;
		player_sessions_[sessionId] = player_session_creator_();
		it = player_sessions_.find(sessionId);

		it->second->sessionId_ = sessionId;
		//it->second->player_manager_ = this;
	}
	return it;
}

bool PlayerManager::RemovePlayerSession(const std::string &sessionId) 
{
	auto it = player_sessions_.find(sessionId);
	if (it == player_sessions_.end()) {
		return false;
	}

	assert(it->second != nullptr);
	
	if (it->second != nullptr)
	{
		player_session_deletor_(it->second);
	}
	else
	{
		LOG(INFO) << "[PlayerManager::RemovePlayerSession],SessionId " << sessionId;
	}
	
	player_sessions_.erase(it);
	return true;
}




PlayerSession* PlayerManager::GetPlayerSessionBySessionId(const std::string &sessionId) {
	auto it = player_sessions_.find(sessionId);
	if (it != player_sessions_.end()) {
		return it->second;
	}
	return nullptr;
}






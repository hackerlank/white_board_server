
/*
 * player_manager.h
 *
 *  Created on: Jan 8, 2014
 *      Author: vagrant
 */

#ifndef GAME_BASE_ACCOUNT_SOURCE_PLAYER_MANAGER_H_
#define GAME_BASE_ACCOUNT_SOURCE_PLAYER_MANAGER_H_

#include <map>
#include <string>

#include "player_constructor.h"


class PlayerManager {
public:
	PlayerManager(player_session_creator_func player_session_creator,
			player_session_deletor_func player_session_deletor ,
			std::string& gameId);
	~PlayerManager();

	PlayerSession* GetPlayerSessionBySessionId(const std::string &sessionId);


public:

	void Init();
	void Update();

	std::map<std::string, PlayerSession*>::iterator AddPlayerSession(const std::string &sessionId);
	bool RemovePlayerSession(const std::string &sessionId);


	void RemoveDroppedUsers();

	void TrackOnlinePlayerStats();

public:
	bool OnPlayerDropped(const std::string &sessionId);


public:

	std::map<std::string, PlayerSession*>  	player_sessions_;         	// sessionId  ---  player

	int64_t player_unactive_time_;
	int64_t character_info_max_dirty_time;

	std::string gameId_;

	int32_t maintenance_;

	time_t lastTrackOnlinePlayerStatsTime_;



private:

	player_session_creator_func				player_session_creator_;
	player_session_deletor_func				player_session_deletor_;

private:
	friend class PlayerSession;
};
#endif  // GAME_BASE_ACCOUNT_SOURCE_PLAYER_MANAGER_H_

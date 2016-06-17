/*
 * character_opcode_def.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jszhang
 */

#ifndef GAME_BASE_ACCOUNT_CHARACTER_OPCODE_DEF_H_
#define GAME_BASE_ACCOUNT_CHARACTER_OPCODE_DEF_H_

#include <rapidjson/document.h>


#include "player_def.h"
#define DNGAMEID "DN"

class Player;

enum CharacterServiceReqType {
	GET_CHARACTER_INFO_REQ = 1,
	UPDATE_CHARACTER_INFO_REQ,
};

enum CharacterServiceRespType {
	GET_CHARACTER_INFO_RESP = 1,
	UPDATE_CHARACTER_INFO_RESP,
};

/*
 *  game server ==> redis/database
 */

struct CharacterInfoReqToCS {
	uint32_t accountId;
	std::string gameId;
	uint32_t respMsgType;

	void ToJson(std::string& request);
};

struct CharacterInfoUpdateReqToCS {
	uint32_t accountId;
	std::string gameId;
	//PlayerCharacterInfo *characterInfo;
	Player* player;
	uint32_t respMsgType;

	void ToJson(std::string& request);
};

/*
 *
 */

/*
 * 	redis/database ==> game server
 */

struct CharacterInfoRespFromCS {
	uint32_t accountId;
	rapidjson::Value characterInfo;

	int32_t	 create_message_time_resp_;

	void FromJson(rapidjson::Value& response);
};

struct CharacterInfoUpdateRespFromCS {
	uint32_t accountId;

	int32_t	 create_message_time_resp_;

	void FromJson(rapidjson::Value& response);
};

/*
 *
 */


#endif  // GAME_BASE_ACCOUNT_CHARACTER_OPCODE_DEF_H_

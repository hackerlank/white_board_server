/*
 * character_opcode_def.cpp
 *
 *  Created on: Feb 21, 2014
 *      Author: vagrant
 */


#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "logging.h"

#include "character_opcode_def.h"
#include <time.h>

#include "account_service.h"
#include "player_manager.h"
#include "service_manager.h"
#include "item_interface.h"


void CharacterInfoReqToCS::ToJson(std::string& request) {
	rapidjson::Document req;
	req.SetObject();

	req.AddMember("accountId", accountId, req.GetAllocator());

	rapidjson::Value game_id_value(gameId.c_str(), req.GetAllocator());
	req.AddMember("gameId", game_id_value, req.GetAllocator());

	req.AddMember("respMsgType", respMsgType, req.GetAllocator());

	//创建消息的时间
	int32_t currentTime = time(nullptr);
	req.AddMember("messageCreateTime", currentTime, req.GetAllocator());

    AccountService* account_service = ServiceManager::instance()->LocateService<AccountService>("Account");
    assert(account_service != nullptr);
    account_service->GetPlayerManager()->AddCSServerSeqID();
    req.AddMember("csSeqID", account_service->GetPlayerManager()->GetCSServerSeqID(), req.GetAllocator());

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	req.Accept(writer);

	char msgType[10] = {'\0'};
	snprintf(msgType, sizeof(msgType), "%d", GET_CHARACTER_INFO_REQ);
	request = std::string(msgType).append(":").append(std::string(buffer.GetString()));
}

void CharacterInfoUpdateReqToCS::ToJson(std::string& request) {
	rapidjson::Document req;
	req.SetObject();

	req.AddMember("accountId", accountId, req.GetAllocator());

	rapidjson::Value game_id_value(gameId.c_str(), req.GetAllocator());
	req.AddMember("gameId", game_id_value, req.GetAllocator());


	rapidjson::Value characterInfoJson;
	std::string propJson;
	
	player->GetPlayerCharacterInfo()->ToJson(player, characterInfoJson, propJson, req.GetAllocator());
	
	req.AddMember("characterInfo", characterInfoJson, req.GetAllocator());


	req.AddMember("respMsgType", respMsgType, req.GetAllocator());

	//创建消息的时间
	int32_t currentTime = time(nullptr);
	req.AddMember("messageCreateTime", currentTime, req.GetAllocator());


    AccountService* account_service = ServiceManager::instance()->LocateService<AccountService>("Account");
    assert(account_service != nullptr);
    account_service->GetPlayerManager()->AddCSServerSeqID();
    req.AddMember("csSeqID", account_service->GetPlayerManager()->GetCSServerSeqID(), req.GetAllocator());


	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	req.Accept(writer);

	char msgType[10] = {'\0'};
	snprintf(msgType, sizeof(msgType), "%d", UPDATE_CHARACTER_INFO_REQ);
	request = std::string(msgType).append(":").append(std::string(buffer.GetString()));

}

void CharacterInfoRespFromCS::FromJson(rapidjson::Value& response) {
	if (!response.IsObject()) {
		LOG(ERROR) << "the response is not a object";
		return;
	}
	if (response.HasMember("accountId") && response["accountId"].IsInt()) {
		accountId = response["accountId"].GetInt();
	}
	if (response.HasMember("characterInfo") && response["characterInfo"].IsObject()) {
		characterInfo = response["characterInfo"];
	}

	if (response.HasMember("createMessageRespTime") && response["createMessageRespTime"].IsInt()) {
		create_message_time_resp_ = response["createMessageRespTime"].GetInt();
	}
}


void CharacterInfoUpdateRespFromCS::FromJson(rapidjson::Value& response) {
	if (!response.IsObject()) {
		LOG(ERROR) << "the response is not a object";
		return;
	}
	if (response.HasMember("accountId") && response["accountId"].IsInt()) {
		accountId = response["accountId"].GetInt();
	}

	if (response.HasMember("createMessageRespTime") && response["createMessageRespTime"].IsInt()) {
		create_message_time_resp_ = response["createMessageRespTime"].GetInt();
	}

//	LOG(INFO) << "Update accountId = " << accountId << " successfully";
}

#include "game_manager_service_pimpl.h"

#include "account_opcode_def.h"
#include "service_manager.h"
#include "account_service.h"
#include "player_manager.h"
#include "zookeeper_service.h"

#define HEART_BEAT_INTERVAL 30 //s

GameManagerServicePimpl::GameManagerServicePimpl(rapidjson::Value& config)
: game_manager_server_rabbitmq_(nullptr)
, last_heart_beat_time_(0)
, heart_beat_interval_(HEART_BEAT_INTERVAL)
{
	config_ = config;
}

GameManagerServicePimpl::~GameManagerServicePimpl()
{

}

bool GameManagerServicePimpl::Init()
{

	LOG(INFO) << " GameManagerServicePimpl::Init() ..... "; 


	Reload(config_);

	LOG(INFO) << " GameManagerServicePimpl::Init() End !!!! " << game_manager_server_rabbitmq_ ;

	return true; 

}

void GameManagerServicePimpl::Update()
{
}

void GameManagerServicePimpl::Destroy()
{

}

bool GameManagerServicePimpl::RegisterRespHandler(uint32_t resp_code, GameService* game_service)
{
	auto resp_handler_iter = resp_service_.find(resp_code);
	if (resp_handler_iter == resp_service_.end())
	{
		resp_service_[resp_code] = game_service;
		return true;
	}
	return false;
}

bool GameManagerServicePimpl::SendHeartBeat()
{
	return false;
}

bool GameManagerServicePimpl::Reload(rapidjson::Value& conf)
{

	return true;
}



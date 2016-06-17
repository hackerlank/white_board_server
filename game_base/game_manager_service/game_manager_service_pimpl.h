/*
* game_manager_service_pimpl.h
*
*  Created on: June 19, 2014
*      Author: yuji ding
*/
#ifndef GAME_BASE_GAME_MANAGER_SERVICE_PIMPL_H_
#define GAME_BASE_GAME_MANAGER_SERVICE_PIMPL_H_

#include "game_manager_service.h"

class GameManagerServicePimpl
{
public:
	explicit GameManagerServicePimpl(rapidjson::Value& config);
	virtual ~GameManagerServicePimpl();
public:
	//Init config(json格式)和 csv文件
	virtual bool Init();
	virtual bool Reload(rapidjson::Value& conf);

	virtual void Update();

	virtual void Destroy();

public:

	RabbitMQClient* game_manager_server_rabbitmq() const { return game_manager_server_rabbitmq_; }

	virtual bool RegisterRespHandler(uint32_t resp_code, GameService* game_service);

	virtual bool SendHeartBeat();

public:

	//来自GameServerManager的返回结果
	std::map<uint32_t, GameService*> resp_service_;


	rapidjson::Value config_;

private:
	RabbitMQClient*	game_manager_server_rabbitmq_;

	time_t last_heart_beat_time_;
	int32_t heart_beat_interval_;
};

#endif //GAME_BASE_GAME_MANAGER_SERVICE_PIMPL_H_
/*
* game_manager_service.h
*
*  Created on: June 16, 2014
*      Author: yujiding
*/
#ifndef GAME_BASE_GAME_MANAGER_SERVICE_H_
#define GAME_BASE_GAME_MANAGER_SERVICE_H_

#include "game_service.h"
#include "rabbitmq_client.h"

class GameManagerServicePimpl;

class GameManagerService : public GameService{
public:
	explicit GameManagerService(rapidjson::Value& config);
	virtual ~GameManagerService();

public:
	// GameService override
	virtual const std::set<std::string> GetDependentServices() const;
	virtual bool IsDependentServicesResolved() const;

	virtual void Inject(std::string& name, Service* service);

	virtual bool Init();
	virtual bool Start();
	virtual bool Stop();
	virtual bool IsRunning();

public:
	RabbitMQClient* GetGameManagerServerRabbitmq();

	virtual bool RegisterRespHandService(uint32_t resp_code, GameService* game_service);

	virtual void Update();

	typedef GameManagerServicePimpl* (*game_manager_service_pimpl_creator)(rapidjson::Value& config);
	typedef void(*game_manager_service_pimpl_deletor)(GameManagerServicePimpl* pimpl);

	virtual bool Reload(rapidjson::Value& config);
public:
	void SetCfgFilePath(std::string& cfg_name){ conf_path_name_ = cfg_name; }
	const std::string& GetCfgFilePath() const {return conf_path_name_;}

protected:
	rapidjson::Value config_;
	void* handle_;

	game_manager_service_pimpl_creator creator_;
	game_manager_service_pimpl_deletor deletor_;

	GameManagerServicePimpl* game_manager_service_pimpl_;

	bool is_running_;

	std::string conf_path_name_;
};



#endif  // GAME_BASE_GAME_MANAGER_SERVICE_GAME_MANAGER_SERVICE_H_

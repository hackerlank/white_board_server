/*
* game_manager_service.cpp
*
*  Created on: June 16, 2014
*      Author: yujiding
*/
#include "game_manager_service.h"

#include <dlfcn.h>

#include "game_manager_service_pimpl.h"

GameManagerService::GameManagerService(rapidjson::Value& config)
: game_manager_service_pimpl_(nullptr)
, is_running_(nullptr)
{
	config_ = config;
}

GameManagerService::~GameManagerService()
{
	if (game_manager_service_pimpl_ != nullptr)
	{
		game_manager_service_pimpl_->Destroy();
		deletor_(game_manager_service_pimpl_);
		game_manager_service_pimpl_ = nullptr;
	}
	if (handle_ != nullptr)
	{
		dlclose(handle_);
	}
}

const std::set<std::string> GameManagerService::GetDependentServices() const
{
    std::set<std::string> dependency;
    dependency.insert("ZkService");
    return  dependency;
}

bool GameManagerService::IsDependentServicesResolved() const
{
	return true;
}

void GameManagerService::Inject(std::string& name, Service* service)
{

}

bool GameManagerService::Init()
{
	if (!Reload(config_))
	{
		return false;
	}

	assert(game_manager_service_pimpl_ != nullptr);

	return true;
}

bool GameManagerService::Start()
{
	if (is_running_)
	{
	return true;
	}
	
	is_running_ = true;
	return true;
}

bool GameManagerService::Stop()
{
	if (!is_running_)
	{
		return false;
	}

	is_running_ = false;
	return true;
}

bool GameManagerService::IsRunning()
{
	return is_running_;
}

void GameManagerService::Update()
{
	assert(game_manager_service_pimpl_ != nullptr);
	game_manager_service_pimpl_->Update();
}

RabbitMQClient* GameManagerService::GetGameManagerServerRabbitmq()
{
	assert(game_manager_service_pimpl_ != nullptr);
	return game_manager_service_pimpl_->game_manager_server_rabbitmq();
}

bool GameManagerService::Reload(rapidjson::Value& config)
{
	LOG(INFO) << " GameManagerService::Reload ...."; 
	if (!config_.IsObject()) {
		LOG(ERROR) << "config is not an object";
		return false;
	}
	if (!config_.HasMember("PimplUrl") || !config_["PimplUrl"].IsString())
	{
		LOG(ERROR) << "PimplUrl missing or error";
		return false;
	}

	if (game_manager_service_pimpl_ == nullptr)
	{
	//create and Init the pimpl
		void* handle;
		game_manager_service_pimpl_creator creator = nullptr;
		game_manager_service_pimpl_deletor deletor = nullptr;
		GameManagerServicePimpl* game_manager_service_pimpl = nullptr;
	
		handle = dlopen(config["PimplUrl"].GetString(), RTLD_LAZY | RTLD_GLOBAL);
		if (handle == nullptr)
		{
			LOG(ERROR) << dlerror();
			return false;
		}
		else
		{
			char *error = nullptr;
			creator = reinterpret_cast<game_manager_service_pimpl_creator>(dlsym(handle, "CreateGameManagerServicePimpl"));
			if ((error = dlerror()) != nullptr)
			{
				LOG(ERROR) << error;
				dlclose(handle);
				handle = nullptr;
				return false;
			}
			deletor = reinterpret_cast<game_manager_service_pimpl_deletor>(dlsym(handle, "DeleteGameManagerServicePimpl"));
			if ((error = dlerror()) != nullptr)
			{
				LOG(ERROR) << error;
				dlclose(handle);
				handle = nullptr;
				return false;
			}
		}
	
		game_manager_service_pimpl = creator(config["Pimpl"]);
		if (game_manager_service_pimpl == nullptr)
		{
			dlclose(handle);
			handle = nullptr;
			return false;
		}
		if (!game_manager_service_pimpl->Init())
		{
			dlclose(handle);
			handle = nullptr;
			return false;
		}
		if (game_manager_service_pimpl_ != nullptr)
		{
			//TODO(hdchen),热更新.so文件策略
			//restart the pimpl
			assert(creator_ != nullptr && deletor_ != nullptr);
			Stop();
			game_manager_service_pimpl_->Destroy();
			deletor_(game_manager_service_pimpl_);
	
			if (handle_ != nullptr)
			{
				dlclose(handle_);
				handle_ = nullptr;
			}
			game_manager_service_pimpl_ = game_manager_service_pimpl;
			creator_ = creator;
			deletor_ = deletor;
			handle_ = handle;
	
			config_ = config;
			Start();
		}
		else
		{
			//start the pimpl
			game_manager_service_pimpl_ = game_manager_service_pimpl;
			creator_ = creator;
			deletor_ = deletor;
			handle_ = handle;
	
			Start();
		}
		//resolve dependency
	} 
	else
	{
		LOG(INFO) << "[GameManagerService] game_manager_service_pimpl_ reload begin";
		if (false == game_manager_service_pimpl_->Reload(config["Pimpl"]))
		{
            LOG(ERROR) << "[GameManagerService] game_manager_service_pimpl_ reload failed";
			return false;
		}
	}
	LOG(INFO) << " GameManagerService::End !!!!";
	return true;
}

bool GameManagerService::RegisterRespHandService(uint32_t resp_code, GameService* game_service)
{
	return game_manager_service_pimpl_->RegisterRespHandler(resp_code, game_service);
}

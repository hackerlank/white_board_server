/*
 * game_script.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: jszhang
 */

#include "game_script.h"
#include "game_server_service.h"

static void onFileChanged(const std::string &file, EFileChangeType type, void* para) {
	if (type == FILE_CHANGE_TYPE_MODIFY) {
		GameScript *game_script = reinterpret_cast<GameScript *>(para);
		game_script->Reload();
	}
}

GameScript::GameScript() :engine(nullptr) {
//	GameServerService::newInstance();
}

GameScript::~GameScript() {
//	GameServerService::deleteInstance();
    if (engine != nullptr){
        delete engine;
        engine = nullptr;
    }
}


void GameScript::SetLuaPathFile(std::string& file_name)
{
	lua_path_file = file_name;
}

bool GameScript::Init() {
	engine = new LuaHostEngine();


	//const char *file = "lua/serverConfig.lua";
	const char* file = lua_path_file.c_str();
	if (engine->load(file) != LUA_HOST_ENGINE_SUCCESS) {
		return false;
	}

	FileMonitor *file_monitor = static_cast<FileMonitor*>(GetService("FileMonitor"));
	file_monitor->AddFile(file, onFileChanged, this);

	return true;
}

bool GameScript::Start() {
	return true;
}
bool GameScript::Stop() {
	return true;
}
bool GameScript::IsRunning() {
	return true;
}

void GameScript::Reload() {
	if (engine->reload() == LUA_HOST_ENGINE_SUCCESS) {
		LOG(INFO) << "GameScript reloaded...";
	} else {
		LOG(ERROR) << "GameScript reload fail.";
	}
}

const std::set<std::string> GameScript::GetDependentServices() const {
	std::set<std::string> dependentServices = std::set<std::string>();
//	dependentServices.insert("GameServerService");
	dependentServices.insert("FileMonitor");
	return dependentServices;
}

bool GameScript::IsDependentServicesResolved() const {
	if (/*dependency_.find("GameServerService") != dependency_.end() && */dependency_.find("FileMonitor") != dependency_.end()) {
		return true;
	}
	return false;
}

Service* GameScript::GetService(const char *service_name) {
	return dependency_[service_name];
}

void GameScript::Inject(std::string& name, Service* service) {
	dependency_[name] = service;
}

/*
 * game_lua_service.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: vagrant
 */

#include "game_server_service.h"

#include "game_script.h"
#include "service_manager.h"

GameServerService::GameServerService()
		: engine_(nullptr)
		, game_script_(nullptr){
}

GameServerService::~GameServerService() {
}

const std::set<std::string> GameServerService::GetDependentServices() const {
	std::set<std::string> dependentServices = std::set<std::string>();
	dependentServices.insert("GameScript");
	return dependentServices;
}

bool GameServerService::IsDependentServicesResolved() const {
	if (game_script_ != nullptr) {
		return true;
	}
	return false;
}

void GameServerService::Inject(std::string& name, Service* service) {
	game_script_ = reinterpret_cast <GameScript*>(service);
}

bool GameServerService::Init() {
	engine_ = game_script_->engine;
	return true;
}

bool GameServerService::Start() {
	return true;
}

bool GameServerService::Stop() {
	return true;
}

bool GameServerService::IsRunning() {
	return true;
}

void GameServerService::HandleUnknowPacket(PacketTranscode *resp, uint32_t opcode) {
	assert(resp != nullptr);
}

void GameServerService::GetServerConfig(PacketTranscode & req, PacketTranscode & resp) {
	size_t szieOfPacketLen = sizeof(uint16_t);
	size_t szieOfOpCodeLen = sizeof(uint16_t);
	size_t HeadLen = szieOfPacketLen + szieOfOpCodeLen;
	lua_tinker::lua_lstring stingPacket(req.GetBuffer()->data + HeadLen, req.GetBuffer()->size - HeadLen);

	engine_->callLuaCDEL("getServerConfig", &stingPacket);
	engine_->getRetValue<PacketTranscode>(&resp);
}

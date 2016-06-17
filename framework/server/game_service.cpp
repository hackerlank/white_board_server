/*
 * game_module.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: shshang
 */

#include "game_service.h"

GameService::GameService() {

}

GameService::~GameService() {

}

const std::set<std::string> GameService::GetDependentServices() const {
	std::set<std::string> dependentServices;
	return dependentServices;
}

bool GameService::IsDependentServicesResolved() const {
	return true;
}

void GameService::Inject(std::string& name, Service* service) {

}

void GameService::Update() {

}

bool GameService::OnRecvPacket(TcpConnection *conn, PacketTranscode &request) {
	return true;
}

bool GameService::OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request) {
			return true;
}


bool GameService::OnConnClosed(TcpConnection *conn) {
	return true;
}

bool GameService::Reload(rapidjson::Value& config) {
	return true;
}

bool GameService::OnRecvMsg(rapidjson::Value& msg)
{
	return true;
}


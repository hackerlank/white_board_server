/*
 * game_lua_service.h
 *
 *  Created on: Jan 15, 2014
 *      Author: vagrant
 */

#ifndef FRAMEWORK_LUA_SERVICE_GAME_SERVER_SERVICE_H_
#define FRAMEWORK_LUA_SERVICE_GAME_SERVER_SERVICE_H_

#include "service.h"
#include "script_base.hpp"


class GameScript;
class GameServerService : public Service{
public:
	GameServerService();
	virtual ~GameServerService();

public:
	// Service override
    virtual const std::set<std::string> GetDependentServices() const;
    virtual bool IsDependentServicesResolved() const;

    virtual void Inject(std::string& name, Service* service);

    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();

public:
    void HandleUnknowPacket(PacketTranscode *resp, uint32_t opcode);
    void GetServerConfig(PacketTranscode & req, PacketTranscode & resp);

protected:
	LuaHostEngine * engine_;
	GameScript* game_script_;
};


#endif  // FRAMEWORK_LUA_SERVICE_GAME_SERVER_SERVICE_H_

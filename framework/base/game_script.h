/*
 * game_script.h
 *
 *  Created on: Dec 18, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_BASE_GAME_SCRIPT_H_
#define FRAMEWORK_BASE_GAME_SCRIPT_H_

#include "file_monitor.h"
#include "service.h"
#include "script_base.hpp"

typedef uint32_t AccountIdType;

class GameScript : public Service {
public:
	GameScript();
	~GameScript();

public:
	void Reload();

	virtual bool Init();
    virtual bool Start();
    virtual bool Stop();
    virtual bool IsRunning();

    virtual const std::set<std::string> GetDependentServices() const;
    Service *GetService(const char* service_name);
    virtual bool IsDependentServicesResolved() const;
    virtual void Inject(std::string& name, Service* service);

	void    SetLuaPathFile(std::string& file_name);

protected:
	LuaHostEngine * engine;
protected:
	std::string     lua_path_file;
private:
	std::map<std::string, Service*> dependency_;
private:
	friend class GameScriptTest;
	friend class LuaService;
	friend class Dispatcher;
	friend class GameServerService;
};





#endif  // FRAMEWORK_BASE_GAME_SCRIPT_H_

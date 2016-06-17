/*
 * account_service.h
 *
 *  Created on: Dec 26, 2013
 *      Author: jszhang
 */

#ifndef GAME_BASE_ACCOUNT_ACCOUNT_SERVICE_H_
#define GAME_BASE_ACCOUNT_ACCOUNT_SERVICE_H_

#include <rapidjson/document.h>

#include <map>

#include "logging.h"
#include "game_service.h"

class Dispatcher;
class AccountServicePimpl;

class AccountService : public GameService {
public:
	explicit AccountService(rapidjson::Value& config);
	virtual ~AccountService();

public:


	virtual bool Reload(rapidjson::Value& config);


	virtual bool OnShutDown();

	void KickAllPlayer();
public:
	// GameService override
	virtual bool OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request);
	virtual bool OnConnClosed(TcpConnection *conn);
	virtual bool OnRecvMsg(rapidjson::Value& msg);

public:
	// Service override
	virtual bool Init();
	virtual bool Start();
	virtual bool Stop();
	virtual bool IsRunning();

	virtual void Update();

	virtual const std::set<std::string> GetDependentServices() const;
	virtual bool IsDependentServicesResolved() const;
	virtual void Inject(std::string& name, Service* service);

private:
	bool AddPimpl(std::string version, void* creator, void* deletor, void* handle, rapidjson::Value& conf);
	bool RemovePimpl(std::string version);
public:
	typedef AccountServicePimpl* (*account_service_pimp_creator_func)(rapidjson::Value& conf);
	typedef void (*account_service_pimp_deletor_func)(AccountServicePimpl* pimpl);

	

private:
	std::map<std::string, AccountServicePimpl*> 				account_service_pimps_;
	std::map<std::string, account_service_pimp_creator_func> 	account_service_pimp_creators_;
	std::map<std::string, account_service_pimp_deletor_func> 	account_service_pimp_deletors_;
	std::map<std::string, void*> 								so_handles_;

	bool														isRunning_;
	Dispatcher*													dispatcher_;

	rapidjson::Value											config_;
};


#endif  // GAME_BASE_ACCOUNT_ACCOUNT_SERVICE_H_

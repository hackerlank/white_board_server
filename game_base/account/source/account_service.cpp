/*
 * account_service.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: jszhang
 */

#include <stdio.h>
#include <dlfcn.h>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>


#include "account_service.h"
#include "account_opcode_def.h"
#include "account_service_pimpl.h"
#include "packet_dispatcher.h"
#include "rabbitmq_client.h"
#include "player_session.h"

#include "service_manager.h"
#include "game_manager_service.h"

/*
 *	format : json
 *	{
 *		"rabbitmq" :
 *		{
 *			"ip": "172.16.7.216",
 *			"port": 5672,
 *			"channel": 1,
 *			"writeExchange": "InMsgExchange",
 *			"writeBindingkey": "In",
 *			"writeQueuename": "InMsgQueue",
 *			"readExchange": "OutMsgExchange",
 *			"readBindingkey": "Out",
 *			"readQueuename": "OutMsgQueue",
 *			"username": "guest"
 *			"password": "guest"
 *		}
 *	}
 */

AccountService::AccountService(rapidjson::Value& config)
: isRunning_(false)
, dispatcher_(nullptr)
{
	config_ = config;
}

AccountService::~AccountService() {
	auto it_end = account_service_pimps_.end();
	for (auto it = account_service_pimps_.begin(); it != it_end; ++it) {
		if (it->second != nullptr) {
			it->second->Destroy();
			auto it_deletor = account_service_pimp_deletors_.find(it->first);
			assert(it_deletor != account_service_pimp_deletors_.end());
			it_deletor->second(it->second);
			it->second = nullptr;
		}
	}
}
bool AccountService::Init() {
	if (!Reload(config_)) {
		return false;
	}

	dispatcher_->SubscribeConnClosedMessage(this);


	return true;
}

bool AccountService::Start() {
	if (isRunning_) {
		return false;
	}
	isRunning_ = true;
	return true;
}

bool AccountService::Stop() {
	if (!isRunning_) {
		return false;
	}
	isRunning_ = false;
	return true;
}

bool AccountService::IsRunning() {
	return isRunning_;
}

void AccountService::Update() {
	if (account_service_pimps_.empty()) {
		return;
	}
	auto it = account_service_pimps_.begin(); 
	auto it_end = account_service_pimps_.end();

	if (it != it_end && it->second != nullptr)
	{
		it->second->Update(); 
	}
	/*
	auto it_end = account_service_pimps_.end();
	for (auto it = account_service_pimps_.begin(); it != it_end; ++it) {
		if (it->second != nullptr) {
			if (it->second->player_manager_->CanBeUnload() && it != --it_end) {
				RemovePimpl(it->first);
				continue;
			}
			it->second->Update();
		}
	}
	*/
}

bool AccountService::OnRecvPacket(TcpConnection *conn, ProtobufTranscode &request) {
	LOG(DEBUG) << "account service on recv packet";

	if (account_service_pimps_.empty()) {
		return false;
	}

	//Ascending traverse by version
	auto it_newest_version_pimp = --account_service_pimps_.end();
	auto it = account_service_pimps_.begin();
	for ( ;it != it_newest_version_pimp; ++it) {
		LOG(DEBUG) << "version is " << it->first << ":" << it->second;
		if (it->second->OnRecvPacket(conn, request)) {
			// Break when find the version which can deal the packet
			LOG(DEBUG) << "on recv , version is " << it->first;
			return true;
		}
	}

	if (it == it_newest_version_pimp) {
		it->second->AddTcpConnection(conn);
		it->second->OnRecvPacket(conn, request);
		return true;
	}
	return false;
}

bool AccountService::OnConnClosed(TcpConnection *conn) {
	auto it_end = account_service_pimps_.end();
	auto it = account_service_pimps_.begin();
	for (; it != it_end; ++it) {
		if (it->second->OnConnClosed(conn)) {
			break;
		}
	}
	if (it == it_end) {
		return false;
	}
	return true;
}

bool AccountService::Reload(rapidjson::Value& config) {
	//get so pathname and version
	if (!config.IsObject()) {
		LOG(ERROR) << "the config is not a object";
		return false;
	}

	if (!config.HasMember("Pimpl") || !config["Pimpl"].IsObject()) {
		LOG(ERROR) << "the Pimpl is missing or error";
		return false;
	}

	if (!config.HasMember("PimplUrl") || !config["PimplUrl"].IsString()) {
		LOG(ERROR) << "the PimplUrl is missing or error";
		return false;
	}

	if (!config.HasMember("PimplVersion") || !config["PimplVersion"].IsString()) {
		LOG(ERROR) << "the PimplVersion is missing or error";
		return false;
	}

	std::string soPathname = config["PimplUrl"].GetString();
	std::string version = config["PimplVersion"].GetString();


	//load so file
	void* handle = dlopen(soPathname.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if (handle == nullptr) {
		LOG(ERROR) << dlerror();
		return false;
	}

	char *error = nullptr;
	void* pimp_creator;
	void* pimp_deletor;


	pimp_creator = dlsym(handle, "CreateAccountServicePimpl");
	if ((error = dlerror()) != nullptr) {
		LOG(ERROR) << error;
		dlclose(handle);
		return false;
	}


	pimp_deletor = dlsym(handle, "DeleteAccountServicePimpl");
	if ((error = dlerror()) != nullptr) {
		LOG(ERROR) << error;
		dlclose(handle);
		return false;
	}

	AddPimpl(version, pimp_creator, pimp_deletor, handle, config);

	LOG(INFO) << "reload successfully";
	return true;
}


bool AccountService::AddPimpl(std::string version, void* creator, void* deletor, void* handle, rapidjson::Value& conf) {
	assert(creator != nullptr && deletor != nullptr && handle != nullptr);

	auto it = account_service_pimps_.find(version);
	if (it != account_service_pimps_.end()) {
		return false;
	}

	account_service_pimp_creator_func pimp_creator = reinterpret_cast<account_service_pimp_creator_func>(creator);
	AccountServicePimpl* account_service_pimp = reinterpret_cast<AccountServicePimpl*>(pimp_creator(conf));
	account_service_pimp->Init();
	dispatcher_->RegisterOpcode(account_service_pimp->opcodes_, this);

	account_service_pimps_[version] = account_service_pimp;
	account_service_pimp_creators_[version] = pimp_creator;
	account_service_pimp_deletors_[version] = reinterpret_cast<account_service_pimp_deletor_func>(deletor);
	so_handles_[version] = handle;

	return true;
}

bool AccountService::RemovePimpl(std::string version) {
	auto it_pimp = account_service_pimps_.find(version);
	if (it_pimp != account_service_pimps_.end()) {
		auto it_creator = account_service_pimp_creators_.find(version);
		auto it_deletor = account_service_pimp_deletors_.find(version);
		auto it_handle = so_handles_.find(version);
		assert(it_creator != account_service_pimp_creators_.end() && it_deletor != account_service_pimp_deletors_.end() && it_handle != so_handles_.end());
		if (it_pimp->second != nullptr) {
			it_pimp->second->Destroy();
			it_deletor->second(it_pimp->second);
			it_pimp->second = nullptr;
		}
		account_service_pimps_.erase(it_pimp);
		account_service_pimp_creators_.erase(it_creator);
		account_service_pimp_deletors_.erase(it_deletor);

		//close so handle
		if (it_handle->second != nullptr) {
			dlclose(it_handle->second);
		}
		so_handles_.erase(it_handle);
	}
	LOG(DEBUG) << "AccountPimp version " << version << " is not add to this module";
	return false;
}

const std::set<std::string> AccountService::GetDependentServices() const {
	std::set<std::string> dependency;
	dependency.insert("Dispatcher");
	return dependency;
}

void AccountService::Inject(std::string &name, Service* service) {
	if (name == "Dispatcher") {
		dispatcher_ = static_cast<Dispatcher*>(service);
	}
}


bool AccountService::IsDependentServicesResolved() const {
	return dispatcher_ != nullptr;
}



bool AccountService::OnRecvMsg(rapidjson::Value& msg)
{
	return true;
}

bool AccountService::OnShutDown()
{
    AccountServicePimpl* account_service_pimp = account_service_pimps_.begin()->second;
    assert(account_service_pimp != nullptr);

    account_service_pimp->can_login_ = false;

    return	account_service_pimp->DropAllOnlinePlayer();
}




void AccountService::KickAllPlayer()
{
	AccountServicePimpl* account_service_pimp = account_service_pimps_.begin()->second;
	assert(account_service_pimp != nullptr);
	return account_service_pimp->KickAllPlayer();
}

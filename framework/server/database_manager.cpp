#include "database_manager.h"



/*	format: json
 *  "{"accountDBHost" : "172.16.7.201" ,
 *  "accountDBPort" : 3306 ,
 *  "accountDBUser" : "baina" ,
 *  "accountDBPassword" : "123456" ,
 *  "accountDB" : "accountdb"
 *  }"
 */
DatabaseManager::DatabaseManager(rapidjson::Value& conf):  pAccountDB_(nullptr), pWorldDB_(nullptr), pLogDB_(nullptr)
{
	conf_ = conf;
}

bool DatabaseManager::SetupDBWorld()
{
	return true;
}

bool DatabaseManager::SetupDBCharacter()
{
	return true;
}
bool DatabaseManager::SetupDBLog()
{
	return true;
}

bool DatabaseManager::InitWorldDB()
{
	if(!conf_.HasMember("worldDBHost") || !conf_["worldDBHost"].IsString()) {
		LOG(INFO) << "worldDB missing or error";
		return false;
	}
	if(!conf_.HasMember("worldDBPort") || !conf_["worldDBPort"].IsInt()) {
		LOG(INFO) << "worldDBPort missing or error";
		return false;
	}
	if(!conf_.HasMember("worldDBUser") || !conf_["worldDBUser"].IsString()) {
		LOG(INFO) << "worldDBUser missing or error";
		return false;
	}
	if(!conf_.HasMember("worldDBPassword") || !conf_["worldDBPassword"].IsString()) {
		LOG(INFO) << "worldDBPassword missing or error";
		return false;
	}
	if(!conf_.HasMember("worldDB") || !conf_["worldDB"].IsString()) {
		LOG(INFO) << "worldDB missing or error";
		return false;
	}
	const char* pszHost = conf_["worldDBHost"].GetString();
	uint32_t nPort = conf_["worldDBPort"].GetInt();
	const char* pszUser = conf_["worldDBUser"].GetString();
	const char* pszPassword = conf_["worldDBPassword"].GetString();
	const char* pszWorldDB = conf_["worldDB"].GetString();
	pWorldDB_ = new MySqlDatabase();
	if(pWorldDB_ == nullptr) {
		LOG(ERROR) << "Memory is not enough,init failed";
		return false;
	}
	bool isOk = pWorldDB_->Connect(pszHost,nPort,pszUser,pszPassword,pszWorldDB);
	if ( !isOk )
	{
		return false;
	}
	SetupDBWorld();
	return true;
}

bool DatabaseManager::InitAccountDB()
{
	if(!conf_.HasMember("accountDBHost") || !conf_["accountDBHost"].IsString()) {
		LOG(INFO) << "accountDB missing or error";
		return false;
	}
	if(!conf_.HasMember("accountDBPort") || !conf_["accountDBPort"].IsInt()) {
		LOG(INFO) << "accountDBPort missing or error";
		return false;
	}
	if(!conf_.HasMember("accountDBUser") || !conf_["accountDBUser"].IsString()) {
		LOG(INFO) << "accountDBUser missing or error";
		return false;
	}
	if(!conf_.HasMember("accountDBPassword") || !conf_["accountDBPassword"].IsString()) {
		LOG(INFO) << "accountDBPassword missing or error";
		return false;
	}
	if(!conf_.HasMember("accountDB") || !conf_["accountDB"].IsString()) {
		LOG(INFO) << "accountDB missing or error";
		return false;
	}
	const char* pszHost = conf_["accountDBHost"].GetString();
	uint32_t nPort = conf_["accountDBPort"].GetInt();
	const char* pszUser = conf_["accountDBUser"].GetString();
	const char* pszPassword = conf_["accountDBPassword"].GetString();
	const char* pszCharacterDB = conf_["accountDB"].GetString();
	pAccountDB_ = new MySqlDatabase();
	if(pAccountDB_ == nullptr) {
		LOG(ERROR) << "Memory is not enough,init failed";
		return false;
	}
	bool isOk = pAccountDB_->Connect(pszHost,nPort,pszUser,pszPassword,pszCharacterDB);
	if ( !isOk )
	{
		LOG(ERROR) << "DataBase connect failed\n";
		return false;
	}
	LOG(INFO) << "DataBase connect success\n";

	SetupDBCharacter();
	return true;
}

bool DatabaseManager::InitLogDB()
{
	if(!conf_.HasMember("logDBHost") || !conf_["logDBHost"].IsString()) {
		LOG(INFO) << "logDB missing or error";
		return false;
	}
	if(!conf_.HasMember("logDBPort") || !conf_["logDBPort"].IsInt()) {
		LOG(INFO) << "LogDBPort missing or error";
		return false;
	}
	if(!conf_.HasMember("logDBUser") || !conf_["logDBUser"].IsString()) {
		LOG(INFO) << "logDBUser missing or error";
		return false;
	}
	if(!conf_.HasMember("logDBPassword") || conf_["logDBPassword"].IsString()) {
		LOG(INFO) << "logDBPassword missing or error";
		return false;
	}
	if(!conf_.HasMember("logDB") || !conf_["logDB"].IsString()) {
		LOG(INFO) << "logDB missing or error";
		return false;
	}
	const char* pszHost = conf_["logDBHost"].GetString();
	uint32_t nPort = conf_["logDBPort"].GetInt();
	const char* pszUser = conf_["logDBUser"].GetString();
	const char* pszPassword = conf_["logDBPassword"].GetString();
	const char* pszLogDB = conf_["logDB"].GetString();
	pLogDB_ = new MySqlDatabase();
	if(pLogDB_ == nullptr) {
		LOG(ERROR) << "Memory is not enough,init failed";
		return false;
	}
	bool isOk = pLogDB_->Connect(pszHost,nPort,pszUser,pszPassword,pszLogDB);
	if ( !isOk )
	{
		return false;
	}
	SetupDBLog();
	return true;
}


DatabaseManager::~DatabaseManager()
{
	if(pWorldDB_ != nullptr) {
		delete	pWorldDB_;
	}
	if(pAccountDB_ != nullptr) {
		delete	pAccountDB_;
	}
	if(pLogDB_ != nullptr) {
		delete	pLogDB_;
	}
}

MySqlDatabase* DatabaseManager::GetCharacterDB()
{
	return pAccountDB_;
}

MySqlDatabase* DatabaseManager::GetWorldDB()
{
	return pWorldDB_;
}

MySqlDatabase* DatabaseManager::GetLogDB()
{
	return pLogDB_;
}

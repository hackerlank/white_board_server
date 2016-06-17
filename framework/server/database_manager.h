#ifndef FRAMEWORK_SERVER_DATABASE_MANAGER_H_
#define FRAMEWORK_SERVER_DATABASE_MANAGER_H_

#include <rapidjson/document.h>
#include "mysql_database.h"

class DatabaseManager //: public Service
{
public:
	explicit DatabaseManager(rapidjson::Value& conf);
	~DatabaseManager();
	MySqlDatabase* GetCharacterDB();
	MySqlDatabase* GetWorldDB();
	MySqlDatabase* GetLogDB();

	bool InitAccountDB();
	bool InitWorldDB();
	bool InitLogDB();

	bool SetupDBCharacter();
	bool SetupDBWorld();
	bool SetupDBLog();

protected:
	MySqlDatabase*								pAccountDB_;
	MySqlDatabase*								pWorldDB_;
	MySqlDatabase*								pLogDB_;

private:
	rapidjson::Value conf_;
	std::mutex mutex_;
};

#endif  // FRAMEWORK_SERVER_DATABASE_MANAGER_H_

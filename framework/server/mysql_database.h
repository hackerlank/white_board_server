#ifndef FRAMEWORK_SERVER_MYSQL_DATABASE_H_
#define FRAMEWORK_SERVER_MYSQL_DATABASE_H_

#include <mutex>
#include <atomic>
#include "scope_guard.h"
#include "mysql_connection.h"


class MySqlDatabase
{
private:

public:
	MySqlDatabase();
	virtual ~MySqlDatabase();

	const char* GetHostName();
	const char* GetDatabaseName();

	bool Connect(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName, uint32_t connCount = 5,uint32_t connCountMax = 50);

	MySqlConnection*  GetConnection();
	bool			  PutConnection( MySqlConnection*& pConn);

private:
	bool _NewConnection();

private:
	std::mutex  mutex_;
	int16_t		port_;
	std::string host_;
	std::string database_;
	std::string user_;
	std::string password_;

	std::atomic_long pool_index_;

	int32_t				default_connection_count_;
	int32_t				max_connection_count_;

	int32_t				connectionCount_;
	MySqlConnection**	connections_;
	bool*				is_free_;


};

#endif  // FRAMEWORK_SERVER_MYSQL_DATABASE_H_

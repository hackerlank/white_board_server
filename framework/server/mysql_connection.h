#ifndef FRAMEWORK_SERVER_MYSQL_CONNECTION_H_
#define FRAMEWORK_SERVER_MYSQL_CONNECTION_H_
#include <stdarg.h>
#include "logging.h"
#include "mysql_rst.h"
class MySqlConnection {
public:
	MySqlConnection(void);
	virtual ~MySqlConnection(void);

	virtual bool Connect(const char* Hostname, uint32_t port, const char* Username, const char* Password, const char* DatabaseName);
	virtual bool ReConnect();

	virtual bool SelectDatabase(const char* pszDatabaseName);
	virtual void Close() ;
	virtual bool CheckValid();

	virtual std::string EscapeString(std::string Escape);

	virtual MysqlRstPtr QueryFormat(const char* QueryString, ...);
	virtual int ExecuteFormat(const char* QueryString, ...);

	virtual MysqlRstPtr Query(const char* QueryString);
	virtual MysqlRstPtr CallProc(const char* QueryString);
	virtual int Execute(const char* QueryString);
	virtual uint32_t GetInsertID();

	virtual void TransactionBegin();
	virtual void Commit() ;
	virtual void Rollback();

	virtual bool Lock( const char* pTableName );
	virtual bool Unlock( );

	const char* GetLastError();
	uint32_t	GetLastErrNo();
	const std::string GetHostName();
	const std::string GetDatabaseName();

	void SetPoolIndex(int32_t index);
	int GetPoolIndex();

protected:
	bool	_Query(const char* strSql);
	MysqlRstPtr _FetchRecordset();

protected:
	MYSQL* connection_;
	int32_t pool_index_;

	const char* hostname_;
	uint32_t port_;
	const char* username_;
	const char* password_;
	const char* databaseNam_;
};

#endif  // FRAMEWORK_SERVER_MYSQL_CONNECTION_H_

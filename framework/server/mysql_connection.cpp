#include "mysql_connection.h"


MySqlConnection::MySqlConnection(void) : pool_index_(-1), hostname_(nullptr), port_(0), username_(nullptr), password_(nullptr), databaseNam_(nullptr)
{
	connection_ = nullptr;
}

MySqlConnection::~MySqlConnection(void)
{
	Close();
}

void MySqlConnection::SetPoolIndex(int index) {
	pool_index_ = index;
}

int MySqlConnection::GetPoolIndex() {
	return pool_index_;
}

bool MySqlConnection::Connect(const char* Hostname, uint32_t port,
							 const char* Username, const char* Password, const char* DatabaseName)
{
	hostname_ = Hostname;
	port_ = port;
	username_ = Username;
	password_ = Password;
	databaseNam_ = DatabaseName;

	my_bool my_true = true;

	LOG(INFO) << "Connecting to `"<< Hostname <<"`, database `"<< DatabaseName <<"`...";

	MYSQL* mysqlConn;
	mysqlConn = mysql_init( nullptr );
	if( mysqlConn == nullptr )
		return false;

	if(mysql_options(mysqlConn, MYSQL_SET_CHARSET_NAME, "utf8"))
		LOG(INFO) << "MySQLDatabase: Could not set utf8 character set.";

	if (mysql_options(mysqlConn, MYSQL_OPT_RECONNECT, &my_true))
		LOG(INFO) << "MySQLDatabase: MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted.";

	connection_ = mysql_real_connect( mysqlConn, Hostname, Username, Password, DatabaseName, port, nullptr, CLIENT_MULTI_STATEMENTS );
	if( connection_ == nullptr )
	{
		LOG(INFO) << "MySQLDatabase: Connection failed due to: `%s`" << mysql_error( mysqlConn );
		mysql_close(mysqlConn);
		return false;
	}
	return true;
}

bool MySqlConnection::ReConnect()
{
	return Connect(hostname_,port_,username_,password_,databaseNam_);
}

void MySqlConnection::Close()
{
	if (connection_) {
		mysql_close(connection_);
	}
	connection_ = nullptr;
}

bool MySqlConnection::CheckValid()
{
	return connection_ && (mysql_ping(connection_) == 0);
}

bool MySqlConnection::SelectDatabase(const char* pszDatabaseName)
{
	int nResult = connection_ ? mysql_select_db(connection_, pszDatabaseName) : -1;
	return (nResult == 0);
}

std::string MySqlConnection::EscapeString(std::string Escape)
{
	char a2[524288] = { 0 };
	std::string ret;
	if(connection_ && mysql_real_escape_string(connection_, a2, Escape.c_str(), (uint64_t)Escape.length()) == 0)
		ret = Escape.c_str();
	else
		ret = a2;
	return std::string(ret);
}

bool MySqlConnection::_Query(const char* strSql)
{
	if ( !connection_ )
		return false;
	LOG(INFO) << "-> MySQL: " << strSql;
	int result = mysql_query( connection_,strSql);
	if(result > 0)
	{
		if ( !CheckValid() )
		{
			Close();
			bool isOk = ReConnect();
			if ( !isOk )
			{
				LOG(INFO) << "Mysql connection reconnect faild!";
				return false;
			}
		}
		result = mysql_query( connection_,strSql);
		if(result > 0)
		{
			const char* pError = GetLastError() ;
			LOG(INFO) <<  (pError?pError:"Unknown Error!");
			LOG(INFO) << "<- MySQL Error: " << result << ":" << pError;
		}
	}
	else
	{
		LOG(INFO) << "<- MySQL: "<< strSql <<" returns "<< result<< ".";
	}
	return (result == 0 ? true : false);
}

MysqlRstPtr MySqlConnection::QueryFormat(const char* szQueryString, ...)
{
	char sql[524288];
	va_list vlist;
	va_start(vlist, szQueryString);
	vsnprintf(sql, sizeof(sql), szQueryString, vlist);
	va_end(vlist);

	return Query(sql);
}

int MySqlConnection::ExecuteFormat(const char* szQueryString, ...)
{
	char sql[524288];
	va_list vlist;
	va_start(vlist, szQueryString);
	vsnprintf(sql, sizeof(sql), szQueryString, vlist);
	va_end(vlist);

	return Execute(sql);
}

int MySqlConnection::Execute(const char* szQueryString)
{
	bool isSuc = _Query(szQueryString);
	if ( !isSuc )
	{
		return -1;
	}
	return  static_cast<int>(mysql_affected_rows(connection_));
}

MysqlRstPtr MySqlConnection::Query(const char* szQueryString)
{
	bool isSuc = _Query( szQueryString);
	if(!isSuc)
	{
		return MysqlRstPtr(nullptr);
	}

	MYSQL_RES * pRes = mysql_store_result( connection_ );
	uint32_t uRows = (uint32_t)mysql_affected_rows( connection_ );
	uint32_t uFields = (uint32_t)mysql_field_count( connection_ );

	MysqlRstPtr pRst(new MysqlRst( pRes, uFields, uRows ));
	pRst->NextRow();
	return pRst;
}

MysqlRstPtr MySqlConnection::CallProc(const char* szQueryString)
{
	if(!_Query( szQueryString))
	{
		return MysqlRstPtr(nullptr);
	}
	MYSQL_RES *pRes = mysql_store_result(connection_);
	uint32_t uRows = (uint32_t)mysql_affected_rows(connection_);
	uint32_t uFields = (uint32_t)mysql_field_count(connection_);
	MysqlRstPtr pRst(new MysqlRst( pRes, uFields, uRows ));
	pRst->NextRow();
	while (mysql_next_result(connection_) == 0)
	{
		MYSQL_RES * tmp = mysql_store_result(connection_);
		if (tmp)
		{
			mysql_free_result(tmp);
		}
	}
	return pRst;
}

uint32_t MySqlConnection::GetInsertID()
{
	return (uint32_t)mysql_insert_id(connection_);
}

 void MySqlConnection::TransactionBegin()
{
	_Query("START TRANSACTION");
}
 void MySqlConnection::Commit()
{
	_Query("COMMIT");
}
 void MySqlConnection::Rollback()
 {
	 _Query("ROLLBACK");
 }

 bool MySqlConnection::Lock( const char* pTableName )
 {
	 char buff[1024];
	 snprintf(buff, sizeof(buff), "LOCK TABLES %s WRITE", pTableName);
	 _Query( buff );
	 return true;
 }

 bool MySqlConnection::Unlock( )
 {
	 _Query( "UNLOCK TABLES " );
	 return true;
 }


 uint32_t	MySqlConnection::GetLastErrNo()
 {
	 if (!connection_)
		 return 0;
	 return mysql_errno(connection_);
 }

const char* MySqlConnection::GetLastError()
{
	if ( !connection_ )
		return "Invalid Connection";
	if(mysql_errno(connection_))
	{
		return mysql_error(connection_);
	}else{
		return nullptr;
	}
}


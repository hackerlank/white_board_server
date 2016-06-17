#include "mysql_database.h"

MySqlDatabase::MySqlDatabase() :
pool_index_(-1)
,default_connection_count_(0)
,max_connection_count_(0)
,connectionCount_(0)
,connections_(nullptr)
,is_free_(nullptr)
{
}

MySqlDatabase::~MySqlDatabase()
{
	if (connections_ != nullptr) {
		for (int i=0; i<max_connection_count_; i++) {
			MySqlConnection *conn = connections_[i];
			if (conn != nullptr) {
				delete conn;
			}
		}
		delete[] connections_;
		connections_ = nullptr;
	}

	if (is_free_ != nullptr) {
		delete[] is_free_;
		is_free_ = nullptr;
	}
}


bool MySqlDatabase::_NewConnection()
{
	if (connectionCount_ >= max_connection_count_) {
		LOG(INFO) << "CMySqlDatabase: connection exceed max connection count" << max_connection_count_;
		return false;
	}

	MySqlConnection * pConnection = new MySqlConnection;
	if ( !pConnection->Connect(host_.c_str(),port_,user_.c_str(),password_.c_str(),database_.c_str()) )
	{
		delete pConnection;
		return false;
	}
	connections_[connectionCount_] = pConnection;
	is_free_[connectionCount_] = true;
	pConnection->SetPoolIndex(connectionCount_);
	connectionCount_++;

	return true;
}

bool MySqlDatabase::Connect(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName, uint32_t connCount,uint32_t connCountMax)
{
	host_ = Hostname;
	port_ = port;
	database_ = DatabaseName;
	user_ = Username;
	password_ = Password;
	default_connection_count_ = connCount;
	max_connection_count_ = connCountMax;

	connections_ = new MySqlConnection*[max_connection_count_]();
	is_free_ = new bool[max_connection_count_]();

	{
		mutex_.lock();
		ON_SCOPE_EXIT([&]{mutex_.unlock();});
		for (int i = 0; i < default_connection_count_; i ++)
		{
			if ( !_NewConnection() )
			{
				return false;
			}
		}
		return true;
	}
}


MySqlConnection* MySqlDatabase::GetConnection()
{
	MySqlConnection *pConn = nullptr;

	int maxRetryCount = connectionCount_;
	int iConn = -1;
	while(maxRetryCount--) {
		long _index = ++pool_index_;

		int count = connectionCount_;
		int iConn = _index % count;

		if (is_free_[iConn]) {
			is_free_[iConn] = false;
			pConn = connections_[iConn];
			break;
		}
	}

	if (pConn == nullptr)
	{
		bool isOk = false;
		{
			mutex_.lock();
			ON_SCOPE_EXIT([&]{mutex_.unlock();});
			isOk = _NewConnection();
			iConn = connectionCount_ - 1;
		}

		if ( isOk )
		{
			is_free_[iConn] = false;
			pConn = connections_[iConn];
		} else {
			return GetConnection();
		}
	}
	return pConn;
}

bool MySqlDatabase::PutConnection( MySqlConnection*& pConn)
{
	int iConn = pConn->GetPoolIndex();
	is_free_[iConn] = true;
	pConn = nullptr;
	return true;
}

const char* MySqlDatabase::GetHostName()
{
	return host_.c_str();
}
const char* MySqlDatabase::GetDatabaseName()
{
	return database_.c_str();
}

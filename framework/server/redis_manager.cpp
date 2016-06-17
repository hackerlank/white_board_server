#include "redis_manager.h"

RedisReply::RedisReply() : reply_(nullptr) {
}

RedisReply::~RedisReply() {
	SetReply(nullptr);
}

void RedisReply::SetReply(redisReply* r) {
	if (reply_ != nullptr) {
		freeReplyObject(reply_);
	}
	reply_ = r;
}

Redis::Redis()
: c_(nullptr)
, pool_index_(-1)
, host_(nullptr)
, port_(0)
, pending_reply_count_(0)
{
}

Redis::~Redis() {
	Close();

	if (host_) {
		free(host_);
		host_ = nullptr;
	}
}

void Redis::SetPoolIndex(int index) {
	pool_index_ = index;
}

int Redis::GetPoolIndex() {
	return pool_index_;
}

bool Redis::Connect(const char* host, unsigned int port) {
	pending_reply_count_ = 0;

	if (host != host_) {
		if (host_ != nullptr) {
			free(host_);
		}
		host_ =  reinterpret_cast<char*>(calloc(strlen(host)+1, sizeof(host_)));
		memcpy(host_, host, strlen(host));
	}

	port_ = port;
	LOG(INFO) << "Connecting to Redis `" << host << "`...";

	c_ = redisConnect(host, port);
	if (c_->err) {
		LOG(INFO) << "[redis] Connection error: " << c_->errstr;
		return false;
	}
	return true;
}

bool Redis::ReConnect() {
	Close();
	return Connect(host_, port_);
}

void Redis::Close() {
	if (c_ != nullptr) {
		redisFree(c_);
		c_ = nullptr;
	}
}

bool Redis::Ensure(bool *reconnected) {
	if (reconnected != nullptr) {
		*reconnected = false;
	}

	if (c_ == nullptr || c_->err != 0) {
		if (reconnected != nullptr) {
			*reconnected = true;
		}
		return ReConnect();
	}
	return true;
}

bool Redis::Ping()
{
	return ExecuteCommand(nullptr, "PING");
}

bool Redis::ExecuteCommand(RedisReply &reply, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	bool ret = ExecuteCommandV(&reply, format, ap);
	va_end(ap);
	return ret;
}

bool Redis::ExecuteCommand(RedisReply *reply, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	bool ret = ExecuteCommandV(reply, format, ap);
	va_end(ap);
	return ret;
}

bool Redis::ExecuteCommandV(RedisReply *reply, const char *format, va_list ap) {
	if (!Ensure()) {
		return false;
	}

	redisReply* r = reinterpret_cast<redisReply*>(redisvCommand(c_, format, ap));
	if (r == nullptr) {
		if (ReConnect()) {
			r = reinterpret_cast<redisReply*>(redisvCommand(c_, format, ap));
		}
	}
	if (nullptr == r) {
		return false;
	}

	if (reply != nullptr) {
		reply->SetReply(r);
	} else {
		freeReplyObject(r);
	}
	return true;
}


bool Redis::ExecuteCommandArgv(RedisReply *reply, int argc, const char **argv, const size_t *argvlen) {
	if (!Ensure()) {
		return false;
	}

	redisReply* r = reinterpret_cast<redisReply*>(redisCommandArgv(c_, argc, argv, argvlen));
	if (r == nullptr) {
		if (ReConnect()) {
			r = reinterpret_cast<redisReply*>(redisCommandArgv(c_, argc, argv, argvlen));
		}
	}

	if (r == nullptr) {
		return false;
	}

	if (reply == nullptr) {
		freeReplyObject(r);
	} else {
		reply->SetReply(r);
	}
	return true;
}

bool Redis::ExecuteCommandInPipeline(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	bool ret = ExecuteCommandInPipelineV(format, ap);
	va_end(ap);
	return ret;
}

bool Redis::ExecuteCommandInPipelineV(const char *format, va_list ap) {
	if (!Ensure()) {
		return false;
	}

	int r = redisvAppendCommand(c_, format, ap);
	if (r == REDIS_ERR) {
		if (ReConnect()) {
			r = redisAppendCommand(c_, format, ap);
		}
	}

	pending_reply_count_++;

	if (r == REDIS_ERR) {
		GetReplyInPipeline(nullptr);
		return false;
	}

	return true;
}

bool Redis::ExecuteCommandInPipelineArgv(int argc, const char **argv, const size_t *argvlen) {
	if (!Ensure()) {
		return false;
	}

	int r = redisAppendCommandArgv(c_, argc, argv, argvlen);
	if (r == REDIS_ERR) {
		if (ReConnect()) {
			r = redisAppendCommandArgv(c_, argc, argv, argvlen);
		}
	}

	pending_reply_count_++;

	if (r == REDIS_ERR) {
		GetReplyInPipeline(nullptr);
		return false;
	}

	return true;
}

bool Redis::GetReplyInPipeline(RedisReply &reply) {
	return GetReplyInPipeline(&reply);
}

bool Redis::GetReplyInPipeline(RedisReply *reply) {
	bool reconnected = false;
	if (!Ensure(&reconnected) || reconnected == true || pending_reply_count_ <= 0) {
		// prevent getreply block current thread when connection reconnected.
		return false;
	}

	pending_reply_count_--;

	redisReply *r = nullptr;
	if (REDIS_OK != redisGetReply(c_, reinterpret_cast<void**>(&r))) {
		return false;
	}

	if (nullptr != reply) {
		reply->SetReply(r);
	} else {
		freeReplyObject(r);
	}
	return true;
}

bool Redis::GetReply(RedisReply *reply) {
	bool reconnected = false;
	if (!Ensure(&reconnected) || reconnected == true) {
		// prevent getreply block current thread when connection reconnected.
		return false;
	}

	redisReply *r = nullptr;
	if (redisGetReply(c_, reinterpret_cast<void**>(&r)) != REDIS_OK) {
		return false;
	}
	if (nullptr == reply) {
		freeReplyObject(r);
	} else {
		reply->SetReply(r);
	}
	return true;
}
/*
 * 	format: json
 * 	{
 * 		"host" : "172.16.7.201" ,
 * 		"port" : 6380 ,
 * 		"maxConnections" : 50 ,
 * 		"initConnections" : 10
 * 	}
 */
RedisManager::RedisManager(rapidjson::Value &conf)
: host_(nullptr)
, port_(0)
, maxConnections_(0)
, countConnections_(0)
, pool_index_(0)
, connections_(nullptr)
, is_free_(nullptr)
{
	conf_ = conf;
}

RedisManager::~RedisManager() {
	if (connections_ != nullptr) {
		for (int i=0; i<maxConnections_; i++) {
			Redis* redis = connections_[i];
			if (redis != nullptr) {
				delete redis;
			}
		}
		delete[] connections_;
		connections_ = nullptr;
	}

	if (is_free_ != nullptr) {
		delete[] is_free_;
		is_free_ = nullptr;
	}

	if (host_ != nullptr) {
		free(host_);
		host_ = nullptr;
	}
}

Redis* RedisManager::_NewConnection()
{
	if (countConnections_ >= maxConnections_) {
		LOG(INFO) << "RedisManager: connection exceed max connection count" << maxConnections_;
		return nullptr;
	}

	mutex_of_connection_.lock();
	ON_SCOPE_EXIT([&]{
		mutex_of_connection_.unlock();
	});

	Redis *redis = new Redis();
	if (!redis->Connect(host_, port_))
	{
		delete redis;
		return nullptr;
	}

	connections_[countConnections_] = redis;
	is_free_[countConnections_] = true;
	redis->SetPoolIndex(countConnections_);
	countConnections_++;

	return redis;
}

bool RedisManager::Init() {
	if(!conf_.HasMember("host") || !conf_["host"].IsString()) {
		LOG(INFO) << "host missing or error";
		return false;
	}
	if(!conf_.HasMember("port") || !conf_["port"].IsInt()) {
		LOG(INFO) << "port missing or error";
		return false;
	}
	if(!conf_.HasMember("maxConnections") || !conf_["maxConnections"].IsInt()) {
		LOG(INFO) << "maxConnections missing or error";
		return false;
	}
	if(!conf_.HasMember("initConnections") || !conf_["initConnections"].IsInt()) {
		LOG(INFO) << "initConnections missing or error";
		return false;
	}
	const char* host = conf_["host"].GetString() ;
	uint32_t port = conf_["port"].GetInt();
	int32_t maxConnections = conf_["maxConnections"].GetInt();
	int32_t initConnections = conf_["initConnections"].GetInt();
	Init(host, port, maxConnections, initConnections);
	return true;
}

bool RedisManager::Init(const char* host, uint32_t port, int maxConnections, int initConnections)
{
	if (host_ != nullptr) {
		return false;
	}
	host_ = reinterpret_cast<char*>(calloc(strlen(host)+1, sizeof(host_)));
	memcpy(host_, host, strlen(host));
	port_ = port;

	maxConnections_ = maxConnections;
	connections_ = new Redis*[maxConnections_]();
	is_free_ = new bool[maxConnections_]();

	while (initConnections-- > 0) {
		if (_NewConnection() == nullptr) {
			return false;
		}
	}

	return true;
}

bool RedisManager::Start() {
	return false;
}

bool RedisManager::Stop() {
	return false;
}

bool RedisManager::IsRunning() {
	return false;
}

Redis* RedisManager::Get()
{
	Redis *redis = nullptr;

	int maxRetryCount = countConnections_;

	while(maxRetryCount--) {
		long _index = ++pool_index_;

		int count = countConnections_;
		int iConn = _index % count;

		if (is_free_[iConn]) {
			is_free_[iConn] = false;
			redis = connections_[iConn];
			break;
		}
	}

	if (redis == nullptr)
	{
		redis = _NewConnection();
		if (redis) {
			int iConn = redis->GetPoolIndex();
			is_free_[iConn] = false;
		} else {
			return Get();
		}
	}

	return redis;
}

void RedisManager::Put(Redis* redis)
{
	int iConn = redis->GetPoolIndex();
	is_free_[iConn] = true;
}

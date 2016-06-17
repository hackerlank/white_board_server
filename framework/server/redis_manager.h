#ifndef FRAMEWORK_SERVER_REDIS_MANAGER_H_
#define FRAMEWORK_SERVER_REDIS_MANAGER_H_

#include <hiredis/hiredis.h>
#include <singleton.h>

#include <cstring>
#include <atomic>
#include <mutex>
#include <rapidjson/document.h>
#include "logging.h"
#include "scope_guard.h"
#include "service.h"



class RedisReply {
public:
	RedisReply();
	~RedisReply();

	inline redisReply* GetReply() const {
		return reply_;
	}

	inline bool Status() const {
		return reply_ != nullptr && reply_->type == REDIS_REPLY_STATUS;
	}

	inline bool Error() const {
		return reply_ == nullptr || reply_->type == REDIS_REPLY_ERROR;
	}

	inline bool Integer() const {
		return reply_ != nullptr && reply_->type == REDIS_REPLY_INTEGER;
	}

	inline bool Nil() const {
		return reply_ != nullptr && reply_->type == REDIS_REPLY_NIL;
	}

	inline bool String() const {
		return reply_ != nullptr && reply_->type == REDIS_REPLY_STRING && reply_->len > 0;
	}

	inline bool Array() const {
		return reply_ != nullptr && reply_->type == REDIS_REPLY_ARRAY && reply_->elements > 0;
	}

protected:
	void SetReply(redisReply* r);

protected:
	redisReply* reply_;

private:
	friend class Redis;
};

class Redis {
protected:
	redisContext*	c_;
	int				pool_index_;

	char*			host_;
	uint32_t		port_;

	int				pending_reply_count_;

public:
	Redis();
	virtual ~Redis();

	virtual bool Connect(const char* Hostname, unsigned int port);
	virtual bool ReConnect();
	virtual void Close() ;

	void SetPoolIndex(int index);
	int GetPoolIndex();

	inline bool Ensure(bool *reconnected = nullptr);
	bool Ping();

	bool ExecuteCommand(RedisReply &reply, const char *format, ...);
	bool ExecuteCommand(RedisReply *reply, const char *format, ...);
	bool ExecuteCommandV(RedisReply *reply, const char *format, va_list ap);
	bool ExecuteCommandArgv(RedisReply *reply, int argc, const char **argv, const size_t *argvlen);

	bool ExecuteCommandInPipeline(const char *format, ...);
	bool ExecuteCommandInPipelineV(const char *format, va_list ap);
	bool ExecuteCommandInPipelineArgv(int argc, const char **argv, const size_t *argvlen);

	bool GetReplyInPipeline(RedisReply &reply);
	bool GetReplyInPipeline(RedisReply *reply);

	bool GetReply(RedisReply *reply);

	inline bool Error() {
		return c_ != nullptr && c_->err != 0;
	}

	inline char* GetError() {
		if (!Error()) return nullptr;
		return c_->errstr;
	}
};

class RedisManager : public Service {
public:
	explicit RedisManager(rapidjson::Value &conf);
	~RedisManager();

	bool Init();

	bool Start();
	bool Stop();
	bool IsRunning();

	Redis* Get();
	void   Put(Redis* redis);

protected:
	bool Init(const char* host, uint32_t port, int maxConnections, int initConnections);

private:
	Redis* _NewConnection();


private:
	rapidjson::Value conf_;

	std::mutex	mutex_of_connection_;
	char*		host_;
	uint32_t	port_;
	int32_t		maxConnections_;
	int32_t		countConnections_;

	std::atomic_long pool_index_;

	Redis**		connections_;
	bool*		is_free_;

};

#endif  // FRAMEWORK_SERVER_REDIS_MANAGER_H_

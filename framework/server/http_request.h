/*
 * http_request.h
 *
 *  Created on: Dec 16, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_HTTP_REQUEST_H_
#define FRAMEWORK_SERVER_HTTP_REQUEST_H_

#include <string>
#include <map>
#include <vector>
#include "mongoose.h"

class HttpRequest {
public:
	HttpRequest(struct mg_connection *request);
	virtual ~HttpRequest();

public:
	void ParseData();
	const char * GetURL() const;
	const char * GetMethod() const;

	const std::string &GetData(const std::string &key) const;
	const std::string &GetData(const char *key) const;
	int32_t GetIntData(const std::string &key, const int32_t def = 0) const;
	int32_t GetIntData(const char *key, const int32_t def = 0) const;
	int64_t GetInt64Data(const std::string &key, const int64_t def = 0) const;
	int64_t GetInt64Data(const char *key, const int64_t def = 0) const;
	const std::map<std::string, std::string> & GetAllData() const;

public:
	static const std::string GET;
	static const std::string POST;
	static const std::string PUT;
	static const std::string HEAD;
	static const std::string DEL;

private:
    struct mg_connection *request_;
	std::map<std::string, std::string> data_;
};


#endif  // FRAMEWORK_SERVER_HTTP_REQUEST_H_

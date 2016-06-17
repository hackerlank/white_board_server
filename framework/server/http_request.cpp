/*
 * http_request.cpp
 *
 *  Created on: Dec 16, 2013
 *      Author: shshang
 */

#include "http_request.h"
#include "http_parse_query.h"

const std::string HttpRequest::GET = "GET";
const std::string HttpRequest::POST = "POST";
const std::string HttpRequest::PUT = "PUT";
const std::string HttpRequest::HEAD = "HEAD";
const std::string HttpRequest::DEL = "DELETE";

HttpRequest::HttpRequest(mg_connection *request) : request_(request) {
	ParseData();
}

HttpRequest::~HttpRequest() {

}

const char* HttpRequest::GetURL() const {
	return request_->uri;
}

const char* HttpRequest::GetMethod() const {
	return request_->request_method;
}

static const std::string sEmpty;
const std::string& HttpRequest::GetData(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = data_.find(key);
	if (it != data_.end()) {
		return it->second;
	}
	return sEmpty;
}

const std::string& HttpRequest::GetData(const char *key) const {
	return GetData(std::string(key));
}

int32_t HttpRequest::GetIntData(const char *key, const int32_t def /* = 0 */) const {
	return GetIntData(std::string(key), def);
}

int32_t HttpRequest::GetIntData(const std::string &key, const int32_t def /* = 0 */) const {
	std::map<std::string, std::string>::const_iterator it = data_.find(key);
	if (it == data_.end()) {
		return def;
	} else {
		int32_t ret = def;
		sscanf(it->second.c_str(), "%10d", &ret);
		return ret;
	}
}

int64_t HttpRequest::GetInt64Data(const char *key, const int64_t def /* = 0 */) const {
	return GetIntData(std::string(key), def);
}

int64_t HttpRequest::GetInt64Data(const std::string &key, const int64_t def /* = 0 */) const {
	std::map<std::string, std::string>::const_iterator it = data_.find(key);
	if (it == data_.end()) {
		return def;
	} else {
		int64_t ret = def;
#ifdef __APPLE__
		sscanf(it->second.c_str(), "%20lld", &ret);
#else
        sscanf(it->second.c_str(), "%20ld", &ret);
#endif
		return ret;
	}
}

const std::map<std::string, std::string>& HttpRequest::GetAllData() const {
	return data_;
}

void HttpRequest::ParseData() {
	if (request_->query_string != NULL) {
		queryStringParse(data_, request_->query_string);
	}

    if (request_->content != nullptr && request_->content_len > 0) {
        std::string temp_content(request_->content);
        temp_content = temp_content.substr(0, request_->content_len);
        queryStringParse(data_, temp_content.c_str());
    }
}

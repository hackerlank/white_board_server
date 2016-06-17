/*
 * http_response.h
 *
 *  Created on: Dec 16, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_HTTP_RESPONSE_H_
#define FRAMEWORK_SERVER_HTTP_RESPONSE_H_

#include <string>
#include "mongoose.h"

class HttpResponse {
public:
	explicit HttpResponse(struct mg_connection *conn);
	~HttpResponse();

public:
	void SetStatusCode(uint32_t code);
	void SetContentType(const char *contentType);
	void SetContentType(const std::string &contentType);
	void AppendContent(const char *content);
	void AppendContent(const std::string &content);
	void Flush();

private:
	std::string contentType_;
	std::string content_;
	mg_connection *conn_;
	uint32_t code_;
};

#endif  // FRAMEWORK_SERVER_HTTP_RESPONSE_H_

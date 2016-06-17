/*
 * http_response.cpp
 *
 *  Created on: Dec 16, 2013
 *      Author: shshang
 */

#include "http_response.h"
#include <map>
#include <functional>


typedef std::map<uint32_t, std::function<void (std::string&)> > ifdo_t;

static ifdo_t codeMsgs;

/* delete by malong in 2015-3-25 because this function witch was defined but no used cause complier warning 
static std::string GetCodeString(uint32_t code) {
	std::string msg("OK");
	if (codeMsgs.find(code) != codeMsgs.end()) {
		codeMsgs[code](msg);
	}
	return msg;
}
*/

static void InitCodeMsgs() {
	codeMsgs[201] = ([](std::string& msg) { msg = "Created"; });
	codeMsgs[202] = ([](std::string& msg) { msg = "Accept"; });
	codeMsgs[203] = ([](std::string& msg) { msg = "Non-Authoritative Information"; });
	codeMsgs[204] = ([](std::string& msg) { msg = "No Content"; });
	codeMsgs[205] = ([](std::string& msg) { msg = "Reset Content"; });
	codeMsgs[206] = ([](std::string& msg) { msg = "Partial Content"; });

	codeMsgs[300] = ([](std::string& msg) { msg = "Multiple Choices"; });
	codeMsgs[301] = ([](std::string& msg) { msg = "Moved Permanently"; });
	codeMsgs[302] = ([](std::string& msg) { msg = "Found"; });
	codeMsgs[303] = ([](std::string& msg) { msg = "See Other"; });
	codeMsgs[304] = ([](std::string& msg) { msg = "Not Modified"; });
	codeMsgs[305] = ([](std::string& msg) { msg = "Use Proxy"; });
	codeMsgs[306] = ([](std::string& msg) { msg = "Unused"; });
	codeMsgs[307] = ([](std::string& msg) { msg = "Temporary Redirect"; });

	codeMsgs[400] = ([](std::string& msg) { msg = "Bad Request"; });
	codeMsgs[401] = ([](std::string& msg) { msg = "Unauthorized"; });
	codeMsgs[402] = ([](std::string& msg) { msg = "Payment Required"; });
	codeMsgs[403] = ([](std::string& msg) { msg = "Forbidden"; });
	codeMsgs[404] = ([](std::string& msg) { msg = "Not Found"; });
	codeMsgs[405] = ([](std::string& msg) { msg = "Method Not Allowed"; });
	codeMsgs[406] = ([](std::string& msg) { msg = "Not Acceptable"; });
	codeMsgs[407] = ([](std::string& msg) { msg = "Proxy Authentication Required"; });
	codeMsgs[408] = ([](std::string& msg) { msg = "Request Timeout"; });
	codeMsgs[409] = ([](std::string& msg) { msg = "Conflict"; });
	codeMsgs[410] = ([](std::string& msg) { msg = "Go"; });
	codeMsgs[411] = ([](std::string& msg) { msg = "Length Required"; });
	codeMsgs[412] = ([](std::string& msg) { msg = "Precondition Failed"; });
	codeMsgs[413] = ([](std::string& msg) { msg = "Request Entity Too Large"; });
	codeMsgs[414] = ([](std::string& msg) { msg = "Request-URI Too Long"; });
	codeMsgs[415] = ([](std::string& msg) { msg = "Unsupported Media Type"; });
	codeMsgs[416] = ([](std::string& msg) { msg = "Requested Range Not Satisfiable"; });
	codeMsgs[417] = ([](std::string& msg) { msg = "Expectation Failed"; });

	codeMsgs[500] = ([](std::string& msg) { msg = "Internal Server Error"; });
	codeMsgs[501] = ([](std::string& msg) { msg = "Not Implemented"; });
	codeMsgs[502] = ([](std::string& msg) { msg = "Bad Gateway"; });
	codeMsgs[503] = ([](std::string& msg) { msg = "Service Unavailable"; });
	codeMsgs[504] = ([](std::string& msg) { msg = "Gateway Timeout"; });
	codeMsgs[505] = ([](std::string& msg) { msg = "HTTP Version Not Supported"; });
}

static void DestroyCodeMsgs() {
	codeMsgs.clear();
}

HttpResponse::HttpResponse(struct mg_connection *conn) : contentType_("text/plain"), conn_(conn), code_(200) {
	InitCodeMsgs();
}

HttpResponse::~HttpResponse() {
	DestroyCodeMsgs();
}

void HttpResponse::SetStatusCode(uint32_t code) {
	code_ = code;
}

void HttpResponse::SetContentType(const char *contentType) {
	contentType_ = contentType;
}

void HttpResponse::SetContentType(const std::string &contentType) {
	contentType_ = contentType;
}

void HttpResponse::AppendContent(const char *content) {
	content_.append(content);
}

void HttpResponse::AppendContent(const std::string &content) {
	content_.append(content);
}

void HttpResponse::Flush() {
    char s[128] = {0};
    snprintf(s, sizeof(s), "%zu", content_.size());

    mg_send_status(conn_, code_);
    mg_send_header(conn_, "Content-Type", contentType_.c_str());
    mg_send_header(conn_, "Content-Length", s);
    mg_send_data(conn_, content_.c_str(), content_.size());
}



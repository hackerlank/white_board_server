/*
 * http_parse_query.h
 *
 *  Created on: Dec 16, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_SERVER_HTTP_PARSE_QUERY_H_
#define FRAMEWORK_SERVER_HTTP_PARSE_QUERY_H_

#include <string.h>
#include <stdlib.h>

#include "logging.h"

void queryStringParse(char *s, char **key, char **value) {
	//LOG(INFO) << "query string parse all" << s;
	char *name;
	char *val;
	int32_t c;
	char buf[3] = {'\0'};
	int32_t pos = 0;

	if(!s) {
		return;
	}

	name = s;
	while(*s && (*s != '=')) {
		s++;
	}
	if(!*s) {
		return;
	}
	*(s++) = '\0';
	*key = reinterpret_cast<char*>(calloc(strlen(name)+1, sizeof(*key)));
	memcpy(*key, name, strlen(name));

	//LOG(INFO) << "query string parse value" << strlen(s) << s;
	*value = reinterpret_cast<char*>(calloc(strlen(s)+1, sizeof(*value)));
	for(val = s; *val; ++val) {
		switch(*val) {
			case '%':
				buf[0] = *(++val);
				buf[1] = *(++val);
				buf[2] = '\0';
				sscanf(buf, "%2x", &c);
				break;
			case '+':
				c = ' ';
				break;
			default:
				c = *val;
				break;
		}
		//LOG(INFO) << "switch pos" << pos << strlen(s);
		switch(c) {
			case '\\': pos += snprintf(*value + pos, strlen(s) + 1 - pos, "\\\\"); break;
			case '\'': pos += snprintf(*value + pos, strlen(s) + 1 - pos, "\\'"); break;
			case '\n': pos += snprintf(*value + pos, strlen(s) + 1 - pos, "\\n"); break;
			default:   pos += snprintf(*value + pos, strlen(s) + 1 - pos, "%c", static_cast<char>(c)); break;
		}
		//LOG(INFO) << "insert to value" << *value;
	}
}

void queryStringParse(std::map<std::string, std::string> &data, const char *query) {
	char *s = reinterpret_cast<char*>(calloc(strlen(query)+1, sizeof(*s)));
	memcpy(s, query, strlen(query));
	//LOG(INFO) << "queryStringParse" << s;

	char *inner_ptr = nullptr;
	char *phase = strtok_r(s, "&", &inner_ptr);
	//LOG(INFO) << "queryStringParse phase" << phase;
	while (phase) {
		char *key = nullptr;
		char *value = nullptr;
		queryStringParse(phase, &key, &value);
		if (key != nullptr && value != nullptr) {
			data[std::string(key)] = std::string(value);
		}
		if (key != nullptr) {
			free(key);
		}
		if (value != nullptr) {
			free(value);
		}
		phase = strtok_r(0, "&", &inner_ptr);

		//LOG(INFO) << "queryStringParse phase" << phase;
	}
	free(s);
}



#endif  // FRAMEWORK_SERVER_HTTP_PARSE_QUERY_H_

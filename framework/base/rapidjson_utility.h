#ifndef FRAMEWORK_BASE_RAPIDJSON_UTILITY_H_
#define FRAMEWORK_BASE_RAPIDJSON_UTILITY_H_

#include <rapidjson/document.h>

class RapidjsonUtility
{
public:
    static bool GetDoubleValue(rapidjson::Value& json_value, const char* name, double& value);
    static bool GetStringValue(rapidjson::Value& json_value, const char* name, std::string& value);
    static bool GetIntValue(rapidjson::Value& json_value, const char* name, int32_t& value);
    static bool GetInt64Value(rapidjson::Value& json_value, const char* name, int64_t& value);

    static bool GetUint8Value(rapidjson::Value& json_value, const char* name, uint8_t& value);
    static bool GetUintValue(rapidjson::Value& json_value, const char* name, uint32_t& value);

    static bool GetUint64Value(rapidjson::Value& json_value, const char* name, uint64_t& value);
    static bool ReadConfFile(const char* filename, rapidjson::Document& conf);
};

#endif  // FRAMEWORK_BASE_RAPIDJSON_UTILITY_H_
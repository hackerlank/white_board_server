#include "rapidjson_utility.h"
#include "logger.h"
#include "logging.h"

bool RapidjsonUtility::ReadConfFile(const char* filename, rapidjson::Document& conf)
{
    FILE* configFile = fopen(filename, "r");
    if (configFile == nullptr) {
        LOG(ERROR) << "fopen error "<<filename;
        return false;
    }
    char config[102400] = { 0 };
    uint32_t realLength = fread(config, sizeof(config[0]), sizeof(config), configFile);
    if (realLength >= sizeof(config))
    {
        LOG(ERROR) << "fread error , buffer size  " << sizeof(config) << " is not enough";
        fclose(configFile);
        return false;
    }

    int32_t result = 0;
    if ((result = ferror(configFile)) != 0)
    {
        LOG(ERROR) << "fopen error code is " << result;
        fclose(configFile);
        return false;
    }
    fclose(configFile);
    std::string json_config = "";
    json_config.append(config);
    conf.SetObject();
    conf.Parse<0>(json_config.c_str());
    return true;
}

bool RapidjsonUtility::GetDoubleValue(rapidjson::Value& json_value, const char* name, double& value)
{
	if (!json_value.IsObject())	{
		return false;
	}
	rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
	if (Iter != json_value.MemberEnd() && (Iter->value.IsInt() || Iter->value.IsDouble())){
		value = Iter->value.GetDouble();
		return true;
	}
	return false;
}

bool RapidjsonUtility::GetStringValue(rapidjson::Value& json_value, const char* name, std::string& value)
{
	 if (!json_value.IsObject()) {
		 return false;
	 }
	 rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
	 if (Iter != json_value.MemberEnd() && Iter->value.IsString()) {
		 value = Iter->value.GetString();
		 return true;
	 }
	 return false;
}

bool RapidjsonUtility::GetIntValue(rapidjson::Value& json_value, const char* name, int32_t& value)
{
	 if (!json_value.IsObject()) {
		 return false;
	 }
	 rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
	 if (Iter != json_value.MemberEnd() && (Iter->value.IsInt() || Iter->value.IsUint())){
		 value = Iter->value.GetInt();
		 return true;
	 }
	 return false;
}

bool RapidjsonUtility::GetInt64Value(rapidjson::Value& json_value, const char* name, int64_t& value){
    if (!json_value.IsObject()) {
        return false;
    }
    rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
    if (Iter != json_value.MemberEnd() && (Iter->value.IsInt64() || Iter->value.IsUint64())){
        value = Iter->value.GetInt64();
        return true;
    }
    return false;
}

bool RapidjsonUtility::GetUintValue(rapidjson::Value& json_value, const char* name, uint32_t& value)
{
	 if (!json_value.IsObject()){
		 return false;
	 }
	 rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
	 if (Iter != json_value.MemberEnd() && Iter->value.IsInt()) {
		 value = Iter->value.GetUint();
		 return true;
	 }
	 return false;
}

bool RapidjsonUtility::GetUint64Value(rapidjson::Value& json_value, const char* name, uint64_t& value){
    if (!json_value.IsObject()){
        return false;
    }
    rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
    if (Iter != json_value.MemberEnd() && Iter->value.IsUint64()) {
        value = Iter->value.GetUint64();
        return true;
    }
    return false;
}


bool RapidjsonUtility::GetUint8Value(rapidjson::Value& json_value, const char* name, uint8_t& value){
    if (!json_value.IsObject()){
        return false;
    }
    rapidjson::Value::MemberIterator Iter = json_value.FindMember(name);
    if (Iter != json_value.MemberEnd() && Iter->value.IsUint64()) {
        value = Iter->value.GetUint64();
        return true;
    }
    return false;
}


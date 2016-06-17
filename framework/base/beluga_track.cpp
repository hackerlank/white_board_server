#include "beluga_track.h"
#include "logger.h"

inline const char* getValue(const std::map<std::string, std::string>& data, const char* key) {
	std::map<std::string, std::string>::const_iterator it = data.find(std::string(key));
	if (it == data.end()) {
		return "-";
	}
	return it->second.c_str();
}

#define getSafeChar(a) a == NULL || strlen(a) == 0 ? "-" : a;

bool BelugaTrack(const std::map<std::string, std::string>& data, const char* category, const char* action, const char* label, const int32_t value) {
    /*const char* pid = getValue(data, "pid");
	const char* isu = getValue(data, "isu");
	const char* did = getValue(data, "did");
	const char* model = getValue(data, "model");
	const char* app_id = getValue(data, "app_id");
	const char* pn = getValue(data, "pn");
	const char* appvn = getValue(data, "appvn");
	const char* appvc = getValue(data, "appvc");
	const char* sdkv = getValue(data, "sdkv");
	const char* chn = getValue(data, "chn");
	const char* cc = getValue(data, "cc");
	const char* os = getValue(data, "os");
	const char* osv = getValue(data, "osv");
	const char* lang = getValue(data, "lang");
	const char* cpu = getValue(data, "cpu");
	const char* res = getValue(data, "res");
	const char* nt = getValue(data, "nt");
	const char* no = getValue(data, "no");
	const char* lon = getValue(data, "lon");
	const char* lat = getValue(data, "lat");

	const char* ctg = getSafeChar(category);
	const char* act = getSafeChar(action);
	const char* lab = getSafeChar(label);

	char val[32] = { 0 };
	snprintf(val,sizeof(val), "%d", value);

	char timestamp[28] = { 0 };
	snprintf(timestamp, sizeof(timestamp), "%lld", (long long int)(time(NULL) * 1000));

	const char* remote_ip = getValue(data, "remote_ip");
	const char* user_agent = "-";
	const char* path = "-";
	const char* acc_id = getValue(data, "acc_id");
	const char* acc_type = getValue(data, "acc_type");

	char log[4096] = { 0 };
	snprintf(log,sizeof(log),"[TRACK]\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s"
		"\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s"
		, pid, isu, did, model, app_id, pn, appvn, appvc, sdkv, chn, cc, os, osv, lang
		, cpu, res, nt, no, lon, lat, ctg, act, lab, val, timestamp, timestamp, timestamp
		, remote_ip, user_agent, path, acc_id, acc_type);
    LOG(INFO) << log;*/
	return true;
}

#ifndef FRAMEWORK_SERVER_FILE_MONITOR_H_
#define FRAMEWORK_SERVER_FILE_MONITOR_H_

#include <string>
#include <map>
#include <thread>
#include "service.h"
#include "logging.h"
#define CHECK_TIME_DURATION 1
enum EFileMonitorCallbackInvokeType
{
	FILE_MONITOR_CALLBACK_INVOKE_AT_MONITOR_THREAD = 1,
	FILE_MONITOR_CALLBACK_INVOKE_IN_UPDATE = 2,
};

enum EFileChangeType
{
	FILE_CHANGE_TYPE_MODIFY = 1,
	FILE_CHANGE_TYPE_DELETE = 2,
};

typedef void (* FILE_MONITOR_CALLBACK)(const std::string &, EFileChangeType, void *);

class FileMonitor : public Service
{
public:
	FileMonitor();
	virtual ~FileMonitor();
	bool Init();
	bool Init(EFileMonitorCallbackInvokeType type);
	bool Start();
	bool Stop();
	bool IsRunning();

	bool AddFile(const std::string & strFileName, FILE_MONITOR_CALLBACK callback, void*);
	void DeleteFile(const std::string & strFileName);

	void CheckModification();
	void ProcessCallback();
	void Update();

	std::string GetValueWithKey(const std::string& content, std::string keyStr, std::string valueStr);
	int GetValueWithKey(const std::string& content, std::string keyStr, int valueNum);
protected:
	time_t _GetFileModTime(const std::string& strFileName) const;

private:
	std::map<std::string, time_t> mapFileList_;
	std::map<std::string, FILE_MONITOR_CALLBACK> mapFileCallback_;
	std::map<std::string, EFileChangeType> changedFileList_;
	std::map<std::string, void*> para_;
	std::thread *thread_;
	bool isRunning_;
	EFileMonitorCallbackInvokeType type_;
	time_t lastCheckTime_;
};

#endif  // FRAMEWORK_SERVER_FILE_MONITOR_H_

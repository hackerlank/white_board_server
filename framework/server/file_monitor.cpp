#include "file_monitor.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

FileMonitor::FileMonitor() : thread_(nullptr), isRunning_(false), type_(FILE_MONITOR_CALLBACK_INVOKE_IN_UPDATE),  lastCheckTime_(0)
{
}

FileMonitor::~FileMonitor()
{
	if (thread_ != nullptr) {
		delete thread_;
		thread_ = nullptr;
	}
}

bool FileMonitor::Init() {
	return Init(FILE_MONITOR_CALLBACK_INVOKE_IN_UPDATE);
}

bool FileMonitor::Init(EFileMonitorCallbackInvokeType type)
{
	if (thread_ != nullptr) {
		return false;
	}

	type_ = type;

	return true;
}

std::string FileMonitor::GetValueWithKey(const std::string& content, std::string keyStr, std::string valueStr)
{
	std::string::size_type pos = 0;
	if( (pos = content.find(keyStr, pos)) != std::string::npos)
	{
		int beginIndex = content.find("=",pos) +1;
		int endIndex = content.find("\n",pos);
		valueStr = content.substr(beginIndex,endIndex-beginIndex);
	}
	return valueStr;
}

int FileMonitor::GetValueWithKey(const std::string& content, std::string keyStr, int valueNum)
{
	char buffer[48] = { 0 };
	snprintf(buffer, sizeof(buffer) ,"%d", valueNum);
	return atoi(this->GetValueWithKey(content,keyStr, buffer).c_str());
}

bool FileMonitor::AddFile(const std::string &strFileName, FILE_MONITOR_CALLBACK callback, void *para)
{
	if (strFileName.empty())
	{
		return false;
	}

	if (callback == nullptr) {
		return false;
	}

	time_t modTime = _GetFileModTime(strFileName);
	if (modTime == 0) {
		return false;
	}

	mapFileCallback_[strFileName] = callback;
	mapFileList_[strFileName] = modTime;
	para_[strFileName] = para;
	return true;
}

void FileMonitor::DeleteFile(const std::string &strFileName)
{
	mapFileList_.erase(strFileName);
	mapFileCallback_.erase(strFileName);
	para_.erase(strFileName);
}

void FileMonitor::CheckModification()
{
	for(std::map<std::string, time_t>::const_iterator itr = mapFileList_.begin(); itr != mapFileList_.end(); )
	{
		time_t modTime = _GetFileModTime(itr->first);
		EFileChangeType _changeType;

		if (modTime == 0) {
			_changeType = FILE_CHANGE_TYPE_DELETE;
			changedFileList_[itr->first] = _changeType;
			mapFileList_.erase(itr++);
		} else if (itr->second != modTime) {
			_changeType = FILE_CHANGE_TYPE_MODIFY;
			changedFileList_[itr->first] = _changeType;
			mapFileList_[itr->first] = modTime;
			++itr;
		} else {
			++itr;
		}
	}
}

void FileMonitor::ProcessCallback() {
	if (changedFileList_.empty()) {
		return;
	}

	for (auto it = changedFileList_.begin(); it != changedFileList_.end(); ++it) {
		mapFileCallback_[it->first](it->first, it->second, para_[it->first]);
	}

	changedFileList_.clear();
}

void FileMonitor::Update() {
	ProcessCallback();
}

bool FileMonitor::Start() {
	if (thread_ != nullptr || isRunning_ != false) {
		return false;
	}
	isRunning_ = true;
	thread_ = new std::thread([this]{
		while(this->IsRunning()) {
			time_t now = time(nullptr);
			if (now - lastCheckTime_ < CHECK_TIME_DURATION) {
				sleep(CHECK_TIME_DURATION + lastCheckTime_ - now);
				continue;
			}
			lastCheckTime_ = now;
			this->CheckModification();
			if (this->type_ == FILE_MONITOR_CALLBACK_INVOKE_AT_MONITOR_THREAD) {
				this->ProcessCallback();
			}
		}
	});
	if (thread_ == nullptr) {
		LOG(INFO) << "Memory is not enough" ;
		return false;
	}
	return true;
}

bool FileMonitor::Stop()
{
	if (thread_ == nullptr || isRunning_ == false) {
		return false;
	}
	isRunning_ = false;
	thread_->join();
	return true;
}

bool FileMonitor::IsRunning()
{
	return isRunning_;
}

time_t FileMonitor::_GetFileModTime(const std::string& strFileName) const {
	FILE* f = nullptr;
	f = fopen(strFileName.c_str(), "r");
	if (f == nullptr) {
		return 0;
	}

	struct stat buf;
	if (fstat(fileno(f), &buf) != 0) {
		fclose(f);
		f = nullptr;
		return 0;
	}

	time_t v = buf.st_mtime;

	fclose(f);
	f = nullptr;
	return v;
}

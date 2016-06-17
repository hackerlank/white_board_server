/*
 *  worker.h
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_BASE_WORKER_H_
#define FRAMEWORK_BASE_WORKER_H_
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "runnable.h"
#include "logging.h"
class Worker {
public:
	explicit Worker(Runnable *iRunnable);
	~Worker();
	bool Run();
	bool SetRunnable(Runnable *runnable);

private:
	void RunTask(Runnable *runnable);
	bool IsRunning();
	bool Stop();
protected:
	Runnable *runnable_;
	std::thread *thread_;
	std::condition_variable cv_;
private:
	std::mutex mutex_;
	bool isRunning_;
private:
	friend class ThreadPool;
};

#endif  // FRAMEWORK_BASE_WORKER_H_

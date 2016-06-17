/*
 * thread_pool.h
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_BASE_THREAD_POOL_H_
#define FRAMEWORK_BASE_THREAD_POOL_H_

#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include "logging.h"
#include "service.h"
#include "runnable.h"
#include "worker.h"

#define DEFAULTMAXPOOLSIZE 10

class ThreadPool: public Service
{
public:
	ThreadPool();
	explicit ThreadPool(uint32_t max_pool_size);
	virtual  ~ThreadPool();
	bool Init();
	bool Start();
	bool Stop();
	void AddWork(Runnable *runnable);
	bool IsRunning();
	void Update();



protected:
	std::mutex mutex_;
	bool isRunning_;
	uint32_t max_pool_size_;
	std::queue<Runnable*> runnables_;
	std::vector<Worker*> workers_;

};

#endif  // FRAMEWORK_BASE_THREAD_POOL_H_

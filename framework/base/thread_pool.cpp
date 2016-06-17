/*
 * thread_pool.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */


#include "thread_pool.h"



ThreadPool::ThreadPool() : isRunning_(false), max_pool_size_(DEFAULTMAXPOOLSIZE){

}


ThreadPool::ThreadPool(uint32_t max_pool_size) : isRunning_(false), max_pool_size_(max_pool_size){

}

ThreadPool::~ThreadPool()
{
	mutex_.lock();
	auto iter_end = workers_.end();
	for (auto iter = workers_.begin(); iter != iter_end; ++iter ) {
		if ((*iter) != nullptr) {
			if((*iter)->IsRunning()) {
				(*iter)->Stop();
			}
			delete (*iter);
			(*iter) = nullptr;
		}
	}
	mutex_.unlock();
}

bool ThreadPool::Init() {
	for (uint32_t i = 0; i < max_pool_size_; i++) {
		Worker *newWorker = new Worker(nullptr);
		if(newWorker == nullptr) {
			LOG(INFO) << "Memory is not enough";
			return false;
		}
		workers_.push_back(newWorker);
	}
	return true;
}

bool ThreadPool::Start()
{
	mutex_.lock();
	auto iter_end = workers_.end();
	for (auto iter = workers_.begin(); iter != iter_end; ++iter )
	{
		if((*iter) != nullptr )
		{
			(*iter)->thread_ = new std::thread(
					[=]{(*iter)->Run();
			});
		}
	}
	isRunning_ = true;
	mutex_.unlock();

	return true;
}

bool ThreadPool::Stop()
{
	mutex_.lock();
	auto iter_end = workers_.end();
	for (auto iter = workers_.begin(); iter != iter_end; ++iter )
	{
		if ( (*iter) != nullptr ) {
			(*iter)->Stop();
		}
	}
	isRunning_ = false;
	mutex_.unlock();

	return true;
}


void ThreadPool::AddWork(Runnable *runnable)
{
	mutex_.lock();
	runnables_.push(runnable);
	mutex_.unlock();
}


bool ThreadPool::IsRunning()
{
	return isRunning_;
}

void ThreadPool::Update() {
	mutex_.lock();
	
	auto iter_end = workers_.end();
	for (auto iter = workers_.begin(); iter != iter_end; ++iter )
	{
		if ((*iter)->runnable_ == nullptr) 
		{
			if (runnables_.empty()) {
				break;
			}
			Runnable *runnable = runnables_.front();
			runnables_.pop();
			(*iter)->SetRunnable(runnable);
			(*iter)->cv_.notify_one();
			if((*iter)->thread_ == nullptr) {
				(*iter)->thread_ = new std::thread([=](){
					(*iter)->Run();
				});
			}
			//break;
		}
	}
	mutex_.unlock();
}

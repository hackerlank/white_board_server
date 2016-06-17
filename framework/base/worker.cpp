/*
 *  worker.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */

#include "worker.h"

Worker::Worker(Runnable *iRunnable) : runnable_(iRunnable), thread_(nullptr), isRunning_(false) {}

Worker::~Worker() {
	if (thread_ != nullptr) {
		delete(thread_);
		thread_ = nullptr;
	}
}

bool Worker::IsRunning() {
	return isRunning_;
}

bool Worker::SetRunnable(Runnable *runnable) {
	mutex_.lock();
	runnable_ = runnable;
	mutex_.unlock();
	return true;
}

bool Worker::Run() {
	if (isRunning_ == true) {
		return false;
	}

	isRunning_ = true;

	while (IsRunning()) {
		Runnable *task;
		if((task = runnable_) == nullptr) {
			std::unique_lock<std::mutex> lck(mutex_);
			cv_.wait(lck);
			continue;
		}

		RunTask(task);
		delete task;

		LOG(DEBUG) << "[Worker::Run over, delete runnable]";

		SetRunnable(nullptr);		
	}
	return true;
}

void Worker::RunTask(Runnable *runnable) {
	runnable->Run();
}

bool Worker::Stop() {
	if(isRunning_ == false || thread_ == nullptr) {
		return false;
	}
	isRunning_ = false;
	cv_.notify_one();
	thread_->join();
	delete(thread_);
	thread_ = nullptr;
	return true;
}

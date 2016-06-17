/*
 *  async_work_manager.h
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_SERVER_ASYNC_WORK_MANAGER_H_
#define FRAMEWORK_SERVER_ASYNC_WORK_MANAGER_H_

#include "thread_pool.h"
#include "singleton.h"

class AsyncWorkManager : public ThreadPool, public Singleton<AsyncWorkManager>{
public:
	explicit AsyncWorkManager(uint32_t max_pool_size = 10);
};


#endif  // FRAMEWORK_SERVER_ASYNC_WORK_MANAGER_H_

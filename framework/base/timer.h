/*
 * timer.h
 *
 *  Created on: Dec 11, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_BASE_TIMER_H_
#define FRAMEWORK_BASE_TIMER_H_

// 时钟相关
#include <time.h>
#include <inttypes.h>

class Timer {
public:
	Timer() {}
	virtual ~Timer() {}

	// 得到流逝的毫秒
	virtual uint64_t GetEesTimes() { return 0; }

	uint64_t GetEesTimesEx() { return 0; }

	// 判断是否流逝了一定的时间
	bool IsEesTimes(uint64_t dwCount) { return true; }

	bool IsEesTimes(uint64_t dwCount, uint64_t loop ) { return true; }

	void ZeroPreTime() {}

	// 设置_preCount值 [12/17/2008 lzy]
	void SetPreTime(uint64_t dwCount) {}

	void SetPreTime(){}
};


#endif  // FRAMEWORK_BASE_TIMER_H_

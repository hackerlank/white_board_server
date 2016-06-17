/*
 *  runnable.h
 *
 *  Created on: Dec 17, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_BASE_RUNNABLE_H_
#define FRAMEWORK_BASE_RUNNABLE_H_

class Runnable {
public:
	Runnable();
	virtual ~Runnable();
	virtual void Run() = 0;
};

#endif  // FRAMEWORK_BASE_RUNNABLE_H_

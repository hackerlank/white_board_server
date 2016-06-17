//
//  singleton.h
//  gateway
//
//  Created by Fubai Zhong on 10/4/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_SINGLETON_H_
#define FRAMEWORK_BASE_SINGLETON_H_

#include <assert.h>

template <class T>
class Singleton {
protected:
    Singleton() {};
    virtual ~Singleton() {};
    
public:
    static T * instance() {
        static T t;
        return &t;
    }
};

#endif  // FRAMEWORK_BASE_SINGLETON_H_

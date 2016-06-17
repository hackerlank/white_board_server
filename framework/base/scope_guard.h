//
//  scope_guard.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_SCOPE_GUARD_H_
#define FRAMEWORK_BASE_SCOPE_GUARD_H_

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

#include <functional>

class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> on_exit_scope)
    : on_exit_scope_(on_exit_scope), dismissed_(false) {
    }
    
    ~ScopeGuard() {
        if (!dismissed_) {
            on_exit_scope_();
        }
    }
    
    void Dismiss() {
        dismissed_ = true;
    }
    
private:
    std::function<void()> on_exit_scope_;
    bool dismissed_;
    
    
private: // noncopyable
    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard & operator=(const ScopeGuard &) = delete;
};

#endif  // FRAMEWORK_BASE_SCOPE_GUARD_H_

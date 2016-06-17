//
//  event_loop.h
//  gateway
//
//  Created by Fubai Zhong on 10/8/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_EVENT_LOOP_H_
#define FRAMEWORK_BASE_EVENT_LOOP_H_

template <class EV>
class EventLoop {
public:
    EventLoop() {};
    virtual ~EventLoop() {};

    virtual void PostEvent(const EV& event) = 0;
    virtual bool PeakEvent(EV** event) = 0;
    virtual void DestroyEvent(EV* event) = 0;
};

#endif  // FRAMEWORK_BASE_EVENT_LOOP_H_

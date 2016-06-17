//
//  double_buffer_queue.h
//  gateway
//
//  Created by Fubai Zhong on 10/13/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_BASE_DOUBLE_BUFFER_QUEUE_H_
#define FRAMEWORK_BASE_DOUBLE_BUFFER_QUEUE_H_

#include <vector>
#include <queue>
#include <atomic>
#include <functional>
#include <mutex>

#include "scope_guard.h"

#include "logging.h"

template<typename T>
class DoubleBufferQueue {
public:
	DoubleBufferQueue() :  DoubleBufferQueue([](T& t){ if (t) delete t; })
	{
	}

	DoubleBufferQueue(std::function<void(T&)> delete_func) : delete_func_(delete_func), queue_index_(0){
        for (int i=0; i<2; i++) {
            queues_.push_back(std::queue<T>());
            mutexes_.push_back(new std::mutex());
        }
#ifndef __linux__
        std::atomic_init(&queue_index_, 0);
#endif
    };

    ~DoubleBufferQueue() {
        for (auto it = queues_.begin(); it != queues_.end(); ++it) {
            while (!it->empty()) {
                T& t = it->front();
                delete_func_(t);
                it->pop();
            }
        }
        queues_.clear();

        for (auto it = mutexes_.begin(); it != mutexes_.end(); ++it) {
            delete *it;
        }
        mutexes_.clear();
    };

    DoubleBufferQueue<T>* swap() {

		//LOG(DEBUG) << this << "  DoubleBufferQueue Swap  Begin  ";

        const auto current_queue_index = queue_index_.load();
        const auto next_queue_index = NextQueueIndex(current_queue_index);
        queue_index_.store(next_queue_index);

		//LOG(DEBUG) << this << "  DoubleBufferQueue Swap  End  ";


        return this;
    };

    void push(T& t) {

		//LOG(DEBUG) << this << " DoubleBufferQueue Push Begin  ";

        const auto current_queue_index = queue_index_.load();
        const auto next_queue_index = NextQueueIndex(current_queue_index);
        auto& mutex_push = mutexes_[next_queue_index];
        auto& queue_push = queues_[next_queue_index];

        mutex_push->lock();
        ON_SCOPE_EXIT([&] {mutex_push->unlock();});

        queue_push.push(t);

		//LOG(DEBUG) << this << " DoubleBufferQueue Push End  ";
    };

    bool pop(T* t) {

		//LOG(DEBUG) << this << " DoubleBufferQueue Pop Begin  "; 

        const auto current_queue_index = queue_index_.load();
        auto& mutex_pop = mutexes_[current_queue_index];
        auto& queue_pop = queues_[current_queue_index];

        mutex_pop->lock();
        ON_SCOPE_EXIT([&] {mutex_pop->unlock();});

        if (queue_pop.empty()) {
            *t = nullptr;
			//LOG(DEBUG) << this << " DoubleBufferQueue  Pop  End 1 ";
            return false;
        }

        *t = queue_pop.front();
        queue_pop.pop();
		//LOG(DEBUG) << this << "DoubleBufferQueue  Pop  End 2 ";
        return true;
    };

    bool empty() const {
        const auto current_queue_index = queue_index_.load();
        auto& mutex_pop = mutexes_[current_queue_index];
        auto& queue_pop = queues_[current_queue_index];

        mutex_pop->lock();
        ON_SCOPE_EXIT([&] {mutex_pop->unlock();});
        return queue_pop.empty();
    };

    size_t size() const {
        const auto current_queue_index = queue_index_.load();
        auto& mutex_pop = mutexes_[current_queue_index];
        auto& queue_pop = queues_[current_queue_index];

        mutex_pop->lock();
        ON_SCOPE_EXIT([&] {mutex_pop->unlock();});
        return queue_pop.size();
    };

protected:
    int NextQueueIndex(int v) {
        return (v + 1) % 2;
    }

private:
	std::function<void(T& t)>   delete_func_;
	std::vector<std::queue<T>>  queues_;
    std::atomic<int>            queue_index_;
    std::vector<std::mutex*>    mutexes_;
};

#endif  // FRAMEWORK_BASE_DOUBLE_BUFFER_QUEUE_H_


//
//  ThreadPool.hpp
//  Simple Thread Pool
//
//  Created by Julien Karst on 01/02/2017.
//  Copyright © 2017 Julien Karst. All rights reserved.
//

#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

class ThreadPool {
    private:
        int                                 _nbThreads;
        std::vector<std::thread>            _pool;
        std::condition_variable             _cond;
        std::mutex                          _mutex;
        std::queue<std::function<void()>>   _queue;
        bool                                _run;

        void addThreads() {
            for(int ii = 0; ii < _nbThreads; ii++) {
                _pool.push_back(std::thread(&ThreadPool::run, this));
            }
        };
    public:
        ThreadPool(int nbThreads) :  _nbThreads(nbThreads), _run(true) { addThreads(); };

        ThreadPool() : _nbThreads(std::thread::hardware_concurrency()), _run(false) { addThreads(); };

        void run() {
            std::function<void()> task;
            while (true) {
                {
                    std::unique_lock<std::mutex> lock(_mutex);

                    _cond.wait(lock, [this] {
                        return !this->_queue.empty();
                    });

                    task = _queue.front();
                    _queue.pop();
                }
                task();
                if (_run == false) { return; }
            }
        };

        void add(std::function<void()> task) {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _queue.push(task);
            }
            _cond.notify_one();
        };

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _run = false;
            }
            _cond.notify_all();
            for(std::thread &t : _pool) t.join();
        };
};

#endif /* ThreadPool_hpp */

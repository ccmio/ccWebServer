#pragma once

#include <mutex>
#include <thread>
#include <assert.h>
#include <queue>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = 8);
    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;
    ~ThreadPool();

    template<class F>
    void AddTask(F&& task);

private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool is_closed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};
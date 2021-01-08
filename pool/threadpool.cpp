#include "threadpool.h"

ThreadPool::ThreadPool(size_t thread_count = 8): pool_(std::make_shared<Pool>()) {
    assert(thread_count > 0);
    for (size_t i = 0; i < thread_count; ++i) {
        std::thread([pool = pool_] {
            std::unique_lock<std::mutex> locker(pool->mtx);
            while(true) {
                if (!pool->tasks.empty()) {
                    auto task = std::move(pool->tasks.front());
                    pool->tasks.pop();
                    locker.unlock();
                    task();
                    locker.lock();
                }
                else if (pool->is_closed) break;
                else pool->cond.wait(locker);
            }

        }).detach();
    }
}

ThreadPool::~ThreadPool() {
    if (static_cast<bool>(pool_)) {
        std::lock_guard<std::mutex> locker(pool_->mtx);
        pool_->is_closed = true;
    }
    pool_->cond.notify_all();
}

template<class F>
void ThreadPool::AddTask(F &&task) {
    {
        std::lock_guard<std::mutex> locker(pool_->mtx);
        pool_->tasks.emplace(std::forward<F>(task));
    }
}
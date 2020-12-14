#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>

#include <assert.h>
template<class T>
class BlockDeque {
public:
    explicit BlockDeque(size_t max_capacity = 1000);
    ~BlockDeque();
    
    void Clear();
    bool Empty();
    bool Full();
    void Close();
    size_t Size();
    size_t Capacity();

    T Front();
    T Back();
    
    void PushBack(const T& item);
    void PushFront(const T& item);
    bool Pop(T& item);
    bool Pop(T& item, int timeout);
    void Flush();

private:
    std::deque<T> deq_;
    size_t capacity_;
    std::mutex mtx_;
    bool is_close_;
    std::condition_variable cond_consumer_;
    std::condition_variable cond_producer_;
};
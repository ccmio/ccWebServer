#include "blockdeque.h"

template<class T>
BlockDeque<T>::BlockDeque(size_t max_capacity = 1000): capacity_(max_capacity) {
    assert(max_capacity > 0);
    is_close_ = false;
}
template<class T>
BlockDeque<T>::~BlockDeque() {
    Close();
}
// 清除队列内容
template<class T>
void BlockDeque<T>::Clear() {
    // 自解锁
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
}
// 判空
template<class T>
bool BlockDeque<T>::Empty() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.empty();
}
// 判满
template<class T>
bool BlockDeque<T>::Full() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size() >= capacity_;
}
// 关闭
template<class T>
void BlockDeque<T>::Close() {
/*     为了保证std::lock_guard生命周期只在这{}里面有效。 
    也就是说，当生命周期离开临界区时，它的生命周期就结束了。  */
    // {
    //     std::lock_guard<std::mutex> locker(mtx_);
    //     deq_.clear();
    //     is_close_ = true;
    // }
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
    is_close_ = true;
    // 关闭的同时唤醒其他进程
    cond_producer_.notify_all();
    cond_consumer_.notify_all();
}
// 求目前大小
template<class T>
size_t BlockDeque<T>::Size() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}
// 返回容量上限
template<class T>
size_t BlockDeque<T>::Capacity() {
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}
// 返回队列前端
template<class T>
T BlockDeque<T>::Front() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.front();
}
// 返回队列末尾
template<class T>
T BlockDeque<T>::Back() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.back();
}
// 添加元素到队尾
template<class T>
void BlockDeque<T>::PushBack(const T& item) {
    std::lock_guard<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {
        // 生产满额 等待消费者唤醒
        cond_producer_.wait(locker);
    }
    deq_.push_back(item);
    // 生产了内容 唤醒消费者
    cond_consumer_.notify_one();
}
// 添加元素到队头
template<class T>
void BlockDeque<T>::PushFront(const T& item) {
    std::lock_guard<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {
        // 生产满额 等待消费者唤醒
        cond_producer_.wait(locker);
    }
    deq_.push_front(item);
    // 生产了内容 唤醒消费者
    cond_consumer_.notify_one();
}
// 删除元素到队尾
template<class T>
bool BlockDeque<T>::Pop(T& item) {
    std::lock_guard<std::mutex> locker(mtx_);
    while(deq_.empty()) {
        // 消费一空 等待生产者唤醒
        cond_consumer_.wait(locker);
        // 是否要判断队列已关闭？
        if (is_close_) return false;
    }
    item = deq_.front();
    deq_.pop_front();
    // 消费了内容 唤醒生产者
    cond_producer_.notify_one();
    return true;
}
template<class T>
bool BlockDeque<T>::Pop(T& item, int timeout) {
    std::lock_guard<std::mutex> locker(mtx_);
    while(deq_.empty()) {
        // 消费一空 等待生产者唤醒
        if (cond_consumer_.wait_for(locker, std::chrono::seconds(timeout))
            == std::cv_status::timeout) {
                return false;
            }
        // 是否要判断队列已关闭？
        if (is_close_) return false;
    }
    item = deq_.front();
    deq_.pop_front();
    // 消费了内容 唤醒生产者
    cond_producer_.notify_one();
    return true;
}
// 通知消费者消费(试图清空内容)
template<class T>
void BlockDeque<T>::Flush() {
    cond_consumer_.notify_one();
}
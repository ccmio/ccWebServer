#pragma once

#include <queue>
#include <unordered_map>
#include <chrono>
#include <functional> // 函数模版类
#include <time.h>
#include <assert.h> 

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    // 结构体内嵌比较函数
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};

class Timer {
public:
    // 分配容器预留空间
    Timer();
    ~Timer();

    void Adjust(int id, int newExpires);
    void Add(int id, int timeout, const TimeoutCallBack& cb);
    void DoWork(int id);
    void Clear();
    void Tick();
    void Pop();
    int GetNextTick();

private:
    void Del_(size_t i);
    void SiftUp_(size_t i);
    bool SiftDown_(size_t index, size_t n);
    void SwapNode_(size_t i, size_t j);

    // 时间节点管理
    std::vector<TimerNode> heap_;
    std::unordered_map<int, size_t> ref_;
};

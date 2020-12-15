#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <string>
#include <sys/stat.h> // mkdir
#include <sys/time.h>
#include <stdarg.h>
#include "../buffer/buffer.h"
#include "blockdeque.h"
class Log {
public:
    void init(int level,
                const char* path="./log",
                const char* suffix=".log",
                int max_queue_len = 1024);
    static Log* Instance();
    static void FlushLogThread();

    void Write(int level, const char* format, ...); // 可变行参列表
    void Flush();
    
    int GetLevel();
    void SetLevel(int level);
    bool IsOpen();

private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();

    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 5000;

    const char* path_;
    const char* suffix_;
    
    int max_lines_;
    int line_count_;
    int today_;

    bool is_open_;

    Buffer buff_;
    int level_;
    bool is_async_;

    FILE* fp_;
    std::unique_ptr<BlockDeque<std::string>> block_deque_;
    std::unique_ptr<std::thread> write_thread_;
    std::mutex mtx_;
};

// 不同日志等级的宏实现
#define LogBase(level, format, ...) do { \
    Log* log = Log::Instance(); \
    if (log->IsOpen() && log->GetLevel() <= level) { \
        log->Write(level, format, ##__VA_ARGS__); \
        log->Flush(); \
    } \
} while(0);
#define LogDebug(format, ...) do {LogBase(0, format, ##__VA_ARGS__)} while(0);
#define LogInfo(format, ...) do {LogBase(1, format, ##__VA_ARGS__)} while(0);
#define LogWarn(format, ...) do {LogBase(2, format, ##__VA_ARGS__)} while(0);
#define LogError(format, ...) do {LogBase(3, format, ##__VA_ARGS__)} while(0);
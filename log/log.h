#pragma once

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
    bool IsOpen() { return is_open_; }

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

    Buffer
}
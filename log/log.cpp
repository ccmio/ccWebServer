#include "log.h"
#include <mutex>
Log::Log() {
    // 应改写的更优雅，可以接受外部参数，用初始化列表实现；
    line_count_ = 0;
    is_async_ = false;
    write_thread_ = nullptr;
    block_deque_ = nullptr;
    today_ = 0;
    fp_ = nullptr;
}
Log::~Log() {
    if (write_thread_ && write_thread_->joinable()) {
        while (!block_deque_->Empty()) {
            block_deque_->Flush();
        }
        block_deque_->Close();
        write_thread_->join();
    }
    if (fp_) {
        std::lock_guard<std::mutex> locker(mtx_);
        Flush();
        fclose(fp_);
    }
}
// 初始化设置
void Log::init(int level, const char* path="./log", const char* suffix=".log", int max_queue_len = 1024) {
    is_open_ = true;
    level_ = level;
    path_ = path;
    suffix_ =  suffix;
    if (max_queue_len > 0) {
        is_async_ = true;
        if (!block_deque_) {
            std::unique_ptr<BlockDeque<std::string>> new_deque(new BlockDeque<std::string>);
            block_deque_ = move(new_deque);
            std::unique_ptr<std::thread> new_thread(new std::thread(FlushLogThread));
            write_thread_ = move(new_thread);
        }
    } else {
        is_async_ = false;
    }

    line_count_ = 0;
    time_t timer = time(nullptr);
    struct tm* sys_time = localtime(&timer);
    struct tm t = *sys_time;
    char file_name[LOG_NAME_LEN] = {0};
    snprintf(file_name, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
    today_ = t.tm_mday;
    {
        std::lock_guard<std::mutex> locker(mtx_);
        buff_.RetrieveAll();
        if(fp_) {
            Flush();
            fclose(fp_);
        }
        fp_ = fopen(file_name, "a");
        if (fp_ == nullptr) {
            mkdir(path_, 0777); // chmod 777 ...
            fp_ = fopen(file_name, "a");
        }
        assert(fp_ != nullptr);
    }
}
// 单例模式
Log* Log::Instance() {
    static Log inst;
    return &inst;
}
// 初始化时清除数据
void Log::FlushLogThread() {
    Log::Instance()->AsyncWrite_();
}
// 写日志
void Log::Write(int level, const char* format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t t_sec = now.tv_sec;
    struct tm* sys_time = localtime(&t_sec);
    struct tm t = *sys_time;
    // 需要新增日志文件
    if (today_ != t.tm_mday || (line_count_ && (line_count_ % max_lines_ == 0))) {
        std::unique_lock<std::mutex> locker(mtx_);
        locker.unlock();

        char new_file[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if (today_ != t.tm_mday) {
            snprintf(new_file, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
            today_ = t.tm_mday;
            line_count_ = 0;
        } else {
            snprintf(new_file, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail, (line_count_ / MAX_LINES),  suffix_);
        }

        locker.lock();
        Flush();
        fclose(fp_);
        fp_ = fopen(new_file, "a");
        assert(fp_ != nullptr);
    }
    // 不需新增日志文件
    {
        std::unique_lock<std::mutex> locker(mtx_);
        line_count_++;
        int n = snprintf(buff_.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        buff_.HasWritten(n);
        AppendLogLevelTitle_(level);

        va_list vars;
        va_start(vars, format);
        int m = vsnprintf(buff_.BeginWrite(), buff_.WritableBytes(), format, vars);
        va_end(vars);

        buff_.HasWritten(m);
        buff_.Append("\n\0", 2);
        // 
        if (is_async_ && block_deque_ && !block_deque_->Full()) {
            block_deque_->PushBack(buff_.RetrieveAllToStr());
        } else {
            fputs(buff_.Peek(), fp_);
        }
        buff_.RetrieveAll();
    }
}
// 冲洗文件
void Log::Flush() {
    if (is_async_) {
        block_deque_->Flush();
    }
    fflush(fp_);
}
// 获取日志级别
int Log::GetLevel() {
    std::lock_guard<std::mutex> locker(mtx_);
    return level_;
}
// 设定日志级别
void Log::SetLevel(int level) {
    std::lock_guard<std::mutex> locker(mtx_);
    level_ = level;
}
// 判断是否可写
bool Log::IsOpen() {
    return is_open_;
}
// 添加日志每一行的标记
void Log::AppendLogLevelTitle_(int level) {
    switch(level) {
    case 0:
        buff_.Append("[debug]: ", 9);
        break;
    case 1:
        buff_.Append("[info] : ", 9);
        break;
    case 2:
        buff_.Append("[warn] : ", 9);
        break;
    case 3:
        buff_.Append("[error]: ", 9);
        break;
    default:
        buff_.Append("[info] : ", 9);
        break;
    }
}
// 异步写
void Log::AsyncWrite_() {
    //何来异步
    std::string str = "";
    while (block_deque_->Pop(str)) {
        std::lock_guard<std::mutex> locker(mtx_);
        fputs(str.c_str(), fp_);
    }
}
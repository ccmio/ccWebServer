#pragma once

#include <iostream>
#include <cstring>
#include <atomic>
#include <vector>

#include <sys/uio.h> // readv
#include <unistd.h>  // write

#include <assert.h>
/* 
|| 预留空间 || 已用空间 || 剩余空间 ||
0--------读指针------写指针------end
 */
class Buffer {
public:
    explicit Buffer(int init_buffer_size = 1024);
    ~Buffer() = default;

    // 模仿muduo
    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;
    
    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);
    void RetrieveAll();
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string &str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer &buff);

    ssize_t ReadFd(int fd, int *Errno);
    ssize_t WriteFd(int fd, int *Errno);

private:
    char* BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace_ (size_t len);

    std::vector<char> buffer_;
    std::atomic<size_t> read_pos_;
    std::atomic<size_t> write_pos_;
};
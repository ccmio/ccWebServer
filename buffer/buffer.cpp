#include "buffer.h"
/* 
|| 预留空间 || 已用空间 || 剩余空间 ||
0--------读指针------写指针------end
 */
Buffer::Buffer(int init_buff_size):
// 初始化了buffer的大小 默认1024
buffer_(init_buff_size), read_pos_(0), write_pos_(0) {}
// 可写容量
size_t Buffer::WritableBytes() const {
    return buffer_.size() - write_pos_;
}
// 可读容量
size_t Buffer::ReadableBytes() const {
    return buffer_.size() - read_pos_;
}
// 预留容量
size_t Buffer::PrependableBytes() const {
    return read_pos_;
}
// 获得读指针
const char* Buffer::Peek() const {
    return BeginPtr_() + read_pos_;
}
// 确保可读len长, 不够则拓展空间
void Buffer::EnsureWriteable(size_t len) {
    if (WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}
// 已经写了的长度len，更正写指针
void Buffer::HasWritten(size_t len) {
    write_pos_ += len;
}
// 废弃len长度数据
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    read_pos_ += len;
}
// 废弃数据直到end指针处
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end);
    Retrieve(end - Peek());
}
// 重置buffer
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    read_pos_ = 0;
    write_pos_ = 0;
}
// 取出所有数据返回，并重置buffer
std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}
// 获取写指针的const版本
const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + write_pos_;
}
// 获取写指针
char* Buffer::BeginWrite() {
    return BeginPtr_() + write_pos_;
}
// append写入buff，接受不同输入的不同版本
void Buffer::Append(const std::string &str) {
    Append(str.data(), str.length());
}
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}
void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}
void Buffer::Append(const Buffer &buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}
// 读描述符入buff
ssize_t Buffer::ReadFd(int fd, int *Errno) {
    // 备用buff
    char candicate_buff[65535];
    struct iovec iov[2];
    const size_t writable_len = WritableBytes();
    // 分散读 保证全部读完
    iov[0].iov_base = BeginPtr_() + write_pos_;
    iov[0].iov_len = writable_len;
    iov[1].iov_base = candicate_buff;
    iov[1].iov_len = sizeof(candicate_buff);

    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *Errno = errno;
    } else if (static_cast<size_t>(len) <= writable_len) {
        write_pos_ += len;
    } else { // 读了一些在buff里
        write_pos_ = buffer_.size();
        Append(candicate_buff, len - writable_len);
    }
    return len;
}
// 
ssize_t Buffer::WriteFd(int fd, int *Errno) {
    size_t readable_len = ReadableBytes();
    ssize_t len = write(fd, Peek(), readable_len);
    if (len < 0) {
        *Errno = errno;
        return len;
    }
    read_pos_ += len;
    return len;
}
// 获取初始位置指针
char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}
// 获取初始位置指针的const重载
const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}
void Buffer::MakeSpace_(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(write_pos_ + len + 1);
    } else { // 使用预留空间
        size_t readable_len = ReadableBytes();
        std::copy(BeginPtr_() + read_pos_, BeginPtr_() + write_pos_, BeginPtr_());
        read_pos_ = 0;
        write_pos_ = read_pos_ + readable_len;
        assert(readable_len == ReadableBytes()); // 确定需要这条吗
    }
}
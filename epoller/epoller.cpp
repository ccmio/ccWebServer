#include "epoller.h"
Epoller::Epoller(int max_event): epoll_fd_(epoll_create(512)), events_(max_event) {
    assert(epoll_fd_ >= 0 && events_.size());
}

Epoller::~Epoller() {
    close(epoll_fd_);
}

/* 
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  uint32_t events;	// Epoll events
  epoll_data_t data;	// User data variable
} __EPOLL_PACKED;
*/
// 增加描述符
bool Epoller::AddFd(int fd, uint32_t events) {
    if (fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}
// 修改描述符
bool Epoller::ModFd(int fd, uint32_t events) {
  if (fd < 0) return false;
  epoll_event ev = {0};
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}
// 删除描述符
bool Epoller::DelFd(int fd) {
  if (fd < 0) return false;
  epoll_event ev = {0};
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
}
// 
int Epoller::Wait(int timeoutMs = -1) {
  return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()), timeoutMs);
}

int Epoller::GetEventFd(size_t i) const {
  assert(i < events_.size() && i >=  0);
  return events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
  assert(i < events_.size() && i >= 0);
  return events_[i].events;
}
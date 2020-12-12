#include "webserver.h"
using namespace std;
   
WebServer::WebServer(int port, int trigMode, int timeoutMS, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, int logLevel, int logQueSize):
            port_(port), open_linger_(OptLinger), timeout_ms_(timeoutMS), is_close_(false),
            // timer_(new HeapTimer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller()) {
{
return;
}

WebServer::~WebServer() {
return;
}

void WebServer::Start() {
    int timeMS = -1;
    if (!is_close_) {
        LogInfo("[ccmio] ==========Server Start Success==========");
    }
    while (!is_close_) {
        if (timeout_ms_ > 0) {
            timeoutMS = timer_->GetNextTick();
        }
        int eventCnt = epoller_->Wait(timeMS);
        for (int i = 0; i < eventCnt; ++i) {
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if (fd == listen_fd_) {
                DealListen_();
            } else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            } else if (events & EPOLLIN) {
                assert(users_.count(fd) > 0);
                DealRead_(&users_[fd]);
            } else if (events & EPOLLOUT) {
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]);
            } else {
                LogERROR("[ccmio] Unexpected event!");
            }
        }
    }
}

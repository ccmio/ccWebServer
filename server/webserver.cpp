#include "webserver.h"
using namespace std;
   
WebServer::WebServer(int port, int trigMode, int timeout_ms_, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, int logLevel, int logQueSize):
            port_(port), open_linger_(OptLinger), timeout_ms_(timeout_ms_), is_close_(false),
            timer_(new Timer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller()) {
{return;
}

WebServer::~WebServer() {
    close(listen_fd_);
    is_close_ = true;
    free(src_dir_);
    SqlConnPool::Instance()->ClosePool();
}

void WebServer::Start() {
    int time_ms_ = -1;
    if (!is_close_) {
        LogInfo("[ccmio] ==========Server Start Success==========");
    }
    while (!is_close_) {
        if (time_ms_ > 0) {
            time_ms_ = timer_->GetNextTick();
        }
        int eventCnt = epoller_->Wait(time_ms_);
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

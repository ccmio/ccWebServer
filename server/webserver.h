#pragma once

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../epoller/epoller.h"
#include "../log/log.h"
#include "../timer/timer.h"
// #include "../sqlpool/sqlpool.h"
// #include "../threadpool/threadpool.h"
// #include "../sqlpool/sqlconnRAII.h"
// #include "../httpconn/httpconn.h"
class WebServer {
public:
    WebServer(
        int port, int trig_mode, int timeout_ms, bool opt_linger, 
        int sql_port, const char* sql_user, const char* sql_pwd, 
        const char* db_name, int conn_pool_num, int thread_num,
        bool open_log, int log_level, int log_queue_size);

    ~WebServer();
    void Start();

// private:
//     bool InitSocket_(); 
//     void InitEventMode_(int trig_mode);
//     void AddClient_(int fd, sockaddr_in addr);
  
//     void DealListen_();
//     void DealWrite_(HttpConn* client);
//     void DealRead_(HttpConn* client);

//     void SendError_(int fd, const char*info);
//     void ExtentTime_(HttpConn* client);
//     void CloseConn_(HttpConn* client);

//     void OnRead_(HttpConn* client);
//     void OnWrite_(HttpConn* client);
//     void OnProcess_(HttpConn* client);

//     static const int MAX_FD = 65536;

//     static int SetFdNonblock_(int fd);

    int port_;
    bool open_linger_;
    int timeout_ms_;  /* 毫秒MS */
    bool is_close_;
    int listen_fd_;
    char* src_dir_;
    
    uint32_t listen_event_;
    uint32_t conn_event_;
   
//     std::unique_ptr<HeapTimer> timer_;
//     std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
//     std::unordered_map<int, HttpConn> users_;
};

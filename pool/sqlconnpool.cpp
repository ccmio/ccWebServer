#include "sqlconnpool.h"

SqlConnPool::SqlConnPool() {
    use_count_ = 0;
    free_count_ = 0;
}

SqlConnPool::~SqlConnPool() {
    ClosePool();
}

static SqlConnPool *Instance() {
    static SqlConnPool conn_poll_;
    return &conn_poll_;
}

MYSQL *GetConn();
void FreeConn(MYSQL * conn);
int GetFreeConnCount();

void Init(const char* host, int port,
            const char* user,const char* pwd, 
            const char* dbName, int connSize);
void ClosePool();
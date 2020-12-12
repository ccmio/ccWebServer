#include <iostream>
<<<<<<< HEAD
#include "server/webserver.h"

int main(int, char**) {
    //daemon(1, 0); 

    WebServer server(
        1316, 3, 60000, false,             /* 端口 ET模式 timeoutMs 优雅退出  */
        3306, "root", "root", "webserver", /* Mysql配置 */
        12, 6, true, 1, 1024);             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    server.Start();
=======

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
>>>>>>> 2d11fe049e129b778f30fb6c2c6cc13e172fd2e2
}

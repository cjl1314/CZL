#ifndef CZL_UDP_H
#define CZL_UDP_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_UDP
    #ifdef CZL_SYSTEM_WINDOWS //声明windows平台
        #pragma comment(lib, "ws2_32.lib") //QtCreator下在.pro文件中加入 LIBS += -lwsock32
    #elif defined CZL_SYSTEM_LINUX
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
        #include <netdb.h>
        #include <sys/epoll.h>
        #define SOCKET_ERROR -1
        typedef int SOCKET;
    #else
        #undef CZL_LIB_UDP //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_UDP

#ifdef CZL_LIB_UDP
    extern const czl_sys_fun czl_lib_udp[]; //库函数表声明
    #define CZL_LIB_UDP_CNT 5               //库函数个数
    #define CZL_LIB_UDP_NAME "udp"          //库名
#endif //CZL_LIB_UDP

#endif // CZL_UDP_H

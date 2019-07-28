#ifndef CZL_WS_H
#define CZL_WS_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_WS
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
        #undef CZL_LIB_WS //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_WS

#ifdef CZL_LIB_WS
    extern const czl_sys_fun czl_lib_ws[]; //库函数表声明
    #define CZL_LIB_WS_CNT 6               //库函数个数
#endif //CZL_LIB_WS

#endif // CZL_WS_H

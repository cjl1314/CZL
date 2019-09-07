#ifndef CZL_WS_H
#define CZL_WS_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifndef CZL_LIB_TCP
    #undef CZL_LIB_WS
#endif

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
    #include "czl_tcp.h" //TCP库
    extern const czl_sys_fun czl_lib_ws[]; //库函数表声明
    #define CZL_LIB_WS_CNT 7               //库函数个数
    #define CZL_LIB_WS_NAME "ws"           //库名
    void** czl_ws_pop(czl_gp*, czl_var*, unsigned char);
#endif //CZL_LIB_WS

#endif // CZL_WS_H

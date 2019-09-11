#ifndef CZL_HTTP_H
#define CZL_HTTP_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifndef CZL_LIB_TCP
    #undef CZL_LIB_HTTP
#endif

#ifdef CZL_LIB_HTTP
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
        #undef CZL_LIB_HTTP //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_HTTP

#ifdef CZL_LIB_HTTP
    #include "czl_tcp.h" //TCP库
    extern const czl_sys_fun czl_lib_http[]; //库函数表声明
    #define CZL_LIB_HTTP_CNT 5               //库函数个数
    #define CZL_LIB_HTTP_NAME "http"         //库名
    void** czl_http_pop(czl_gp*, czl_var*, unsigned char);
#endif //CZL_LIB_HTTP

#endif // CZL_HTTP_H

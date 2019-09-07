#ifndef CZL_TCP_H
#define CZL_TCP_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_TCP
    #ifdef CZL_SYSTEM_WINDOWS //声明windows平台
        #pragma comment(lib, "ws2_32.lib") //QtCreator下在.pro文件中加入 LIBS += -lwsock32
    #elif defined CZL_SYSTEM_LINUX
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
        #include <netdb.h>
        #include <sys/epoll.h>
        #define CZL_TCP_REUSEPORT
        #define SOCKET_ERROR -1
        typedef int SOCKET;
    #else
        #undef CZL_LIB_TCP //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_TCP

#ifdef CZL_LIB_TCP
    extern const czl_sys_fun czl_lib_tcp[]; //库函数表声明
    #define CZL_LIB_TCP_CNT 11              //库函数个数
    #define CZL_LIB_TCP_NAME "tcp"          //库名
    #define CZL_TCP_BUF_SIZE 50*1024
    //
    #ifdef CZL_SYSTEM_WINDOWS
        extern CRITICAL_SECTION czl_tcp_cs;
    #else //CZL_SYSTEM_LINUX
        extern pthread_mutex_t czl_tcp_mutex;
    #endif
    //
    enum czl_tcp_type_enum
    {
        CZL_TCP_SRV,
        CZL_TCP_CLIENT,
        CZL_TCP_SRV_MASTER,
        CZL_TCP_SRV_WORKER
    };
    //
    typedef struct czl_tcp_handle
    {
        czl_gp *gp;
        SOCKET sock;
        unsigned char type;
        long maxFd;
        unsigned long limit;
        void **obj;
        czl_fun *login;
        czl_fun *leave;
        czl_fun *block;
        czl_fun *tcp;
    #ifdef CZL_LIB_HTTP
        czl_fun *http;
    #endif
    #ifdef CZL_LIB_WS
        czl_fun *ws;
    #endif
    #ifdef CZL_SYSTEM_WINDOWS
        czl_tabkv *last;
    #else //CZL_SYSTEM_LINUX
        int kdpfd;
        unsigned long count;
        struct epoll_event *events;
    #endif
    } czl_tcp_handle;
    char czl_tcp_connect(czl_gp*, czl_fun*);
    unsigned long czl_tcp_data(czl_gp*, SOCKET, long, char*);
#endif //CZL_LIB_TCP

#endif // CZL_TCP_H

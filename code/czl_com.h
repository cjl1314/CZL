#ifndef CZL_COM_H
#define CZL_COM_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_COM
    #ifdef CZL_SYSTEM_WINDOWS //声明windows平台

    #elif defined CZL_SYSTEM_LINUX
        #include <termios.h>
        #include <fcntl.h>
        #include <sys/epoll.h>
        extern const czl_sys_fun czl_lib_com[]; //库函数表声明
    #else
        #undef CZL_LIB_COM //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_COM

#ifdef CZL_LIB_COM
    extern const czl_sys_fun czl_lib_com[]; //库函数表声明
    #define CZL_LIB_COM_CNT 4               //库函数个数
    #define CZL_LIB_COM_NAME "com"          //库名
#endif //CZL_LIB_COM

#endif // CZL_COM_H

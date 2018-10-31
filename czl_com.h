#ifndef CZL_COM_H
#define CZL_COM_H

//#define CZL_LIB_COM //第三方库宏控

#ifdef CZL_LIB_COM
    #include "czl_lib.h" //CZL扩展库必须包含该头文件
    #ifdef CZL_SYSTEM_WINDOWS //声明windows平台
        #include <winspool.h> //具体平台的头文件依赖
        extern const czl_sys_fun czl_lib_com[]; //库函数表声明
    #else
        #undef CZL_LIB_COM //不支持其它平台关闭宏控
    #endif
#endif //CZL_LIB_COM

#endif // CZL_COM_H

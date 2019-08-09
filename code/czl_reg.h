#ifndef CZL_REG_H
#define CZL_REG_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_REG
    #include "pcre.h"
    extern const czl_sys_fun czl_lib_reg[]; //库函数表声明
    #define CZL_LIB_REG_CNT 5               //库函数个数
#endif //CZL_LIB_REG

#endif // CZL_REG_H

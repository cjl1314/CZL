#ifndef CZL_SQL_H
#define CZL_SQL_H

#include "czl_lib.h" //CZL扩展库必须包含该头文件

#ifdef CZL_LIB_SQL
    #include "sqlite3.h"
    #include "mysql/mysql.h"
    extern const czl_sys_fun czl_lib_sql[]; //库函数表声明
    #define CZL_LIB_SQL_CNT 7               //库函数个数
    #define CZL_LIB_SQL_NAME "sql"          //库名
#endif //CZL_LIB_SQL

#endif // CZL_SQL_H

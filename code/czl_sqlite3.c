#include "czl_sqlite3.h"

#ifdef CZL_LIB_SQL

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_sql_open(czl_gp *gp, czl_fun *fun);    //打开数据库
char czl_sql_close(czl_gp *gp, czl_fun *fun);   //关闭数据库
char czl_sql_exec(czl_gp *gp, czl_fun *fun);    //执行sql批量获取结果
char czl_sql_prepare(czl_gp *gp, czl_fun *fun); //编译sql
char czl_sql_query(czl_gp *gp, czl_fun *fun);   //执行编译sql查询
char czl_sql_bind(czl_gp *gp, czl_fun *fun);    //执行编译sql插入
char czl_sql_free(czl_gp *gp, czl_fun *fun);    //释放编译资源

//库函数表定义
const czl_sys_fun czl_lib_sql[] =
{
    //函数名,      函数指针,           参数个数,  参数声明
    {"open",      czl_sql_open,      1,        "str_v1"},
    {"close",     czl_sql_close,     1,        "int_v1"},
    {"exec",      czl_sql_exec,      2,        "int_v1,str_v2"},
    {"prepare",   czl_sql_prepare,   2,        "int_v1,str_v2"},
    {"query",     czl_sql_query,     1,        "int_v1"},
    {"bind",      czl_sql_bind,      2,        "int_v1,arr_v2"},
    {"free",      czl_sql_free,      1,        "int_v1"},
};

char czl_sql_open(czl_gp *gp, czl_fun *fun)
{
    char *path = CZL_STR(fun->vars->val.str.Obj)->str;
    sqlite3 *db;

    if (sqlite3_open(path, &db) != SQLITE_OK)
        fun->ret.val.inum = 0;
    else if (!(fun->ret.val.inum=czl_extsrc_create(gp, db, sqlite3_close, czl_lib_sql)))
        return 0;

    return 1;
}

char czl_sql_close(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_extsrc_free(gp, fun->vars->val.inum);
    return 1;
}

char czl_sql_exec(czl_gp *gp, czl_fun *fun)
{
    sqlite3 *db = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);
    char *sql = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (!db || sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK)
        fun->ret.val.inum = 0;
    else
        fun->ret.val.inum = 1;

    return 1;
}

char czl_sql_prepare(czl_gp *gp, czl_fun *fun)
{
    sqlite3_stmt *stmt;
    sqlite3 *db = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);
    char *sql = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        fun->ret.val.inum = 0;
    else if (!(fun->ret.val.inum=czl_extsrc_create(gp, stmt, sqlite3_finalize, czl_lib_sql)))
        return 0;

    return 1;
}

char czl_sql_query(czl_gp *gp, czl_fun *fun)
{
    sqlite3_stmt *stmt = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);

    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW)
        fun->ret.val.inum = 0;
    else
    {
        unsigned long i, cnt = sqlite3_column_count(stmt);
        void **obj = czl_array_create(gp, cnt, cnt);
        czl_var *vars;
        if (!obj)
            return 0;
        vars = CZL_ARR(obj)->vars;
        for (i = 0; i < cnt; ++i)
        {
            const unsigned char *str;
            unsigned long len;
            switch (sqlite3_column_type(stmt, i))
            {
            case SQLITE_INTEGER: case SQLITE_NULL:
                vars[i].type = CZL_INT;
            #ifdef CZL_SYSTEM_64bit
                vars[i].val.inum = sqlite3_column_int64(stmt, i);
            #else
                vars[i].val.inum = sqlite3_column_int(stmt, i);
            #endif
                break;
            case SQLITE_FLOAT:
                vars[i].type = CZL_FLOAT;
                vars[i].val.fnum = sqlite3_column_double(stmt, i);
                break;
            default: //SQLITE_TEXT/SQLITE_BLOB
                str = sqlite3_column_blob(stmt, i);
                len = sqlite3_column_bytes(stmt, i);
                if (!czl_str_create(gp, &vars[i].val.str, len+1, len, str))
                    return 0;
                vars[i].type = CZL_STRING;
                break;
            }
        }
        fun->ret.type = CZL_ARRAY;
        fun->ret.val.Obj = obj;
    }

    return 1;
}

char czl_sql_bind(czl_gp *gp, czl_fun *fun)
{
    sqlite3_stmt *stmt = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);

    if (!stmt)
        fun->ret.val.inum = 0;
    else
    {
        czl_array *arr = CZL_ARR(fun->vars[1].val.Obj);
        unsigned long i, cnt = arr->cnt;
        czl_var *vars = arr->vars;
        sqlite3_reset(stmt);
        for (i = 0; i < cnt; ++i)
        {
            czl_string *s;
            switch (vars[i].type)
            {
            case CZL_INT:
            #ifdef CZL_SYSTEM_64bit
                sqlite3_bind_int64(stmt, i+1, vars[i].val.inum);
            #else
                sqlite3_bind_int(stmt, i+1, vars[i].val.inum);
            #endif
                break;
            case CZL_FLOAT:
                sqlite3_bind_double(stmt, i+1, vars[i].val.fnum);
                break;
            case CZL_STRING:
                s = CZL_STR(vars[i].val.str.Obj);
                sqlite3_bind_blob(stmt, i+1, s->str, s->len, SQLITE_STATIC);
                break;
            default:
                sqlite3_bind_int(stmt, i+1, 0);
                break;
            }
        }
        sqlite3_step(stmt);
        fun->ret.val.inum = 1;
    }

    return 1;
}

char czl_sql_free(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_extsrc_free(gp, fun->vars->val.inum);
    return 1;
}
#endif //CZL_LIB_SQL

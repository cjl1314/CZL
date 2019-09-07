#include "czl_sql.h"

#ifdef CZL_LIB_SQL

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_sql_open(czl_gp *gp, czl_fun *fun);    //打开数据库
char czl_sql_error(czl_gp *gp, czl_fun *fun);   //获取错误信息
char czl_sql_exec(czl_gp *gp, czl_fun *fun);    //执行sql批量获取结果
char czl_sql_prepare(czl_gp *gp, czl_fun *fun); //编译sql
char czl_sql_query(czl_gp *gp, czl_fun *fun);   //执行编译sql查询
char czl_sql_reset(czl_gp *gp, czl_fun *fun);   //重置query光标
char czl_sql_bind(czl_gp *gp, czl_fun *fun);    //执行编译sql参数绑定

//库函数表定义
const czl_sys_fun czl_lib_sql[] =
{
    //函数名,      函数指针,           参数个数,  参数声明
    {"open",      czl_sql_open,      7,        "str_v1,str_v2=\"sqlite3\",str_v3=\"\",int_v4=0,str_v5=\"\",str_v6=\"\",int_v7=0"},
    {"error",     czl_sql_error,     1,        "v1=0"},
    {"exec",      czl_sql_exec,      2,        "src_v1"},
    {"prepare",   czl_sql_prepare,   2,        "src_v1,str_v2"},
    {"query",     czl_sql_query,     1,        "src_v1"},
    {"reset",     czl_sql_reset,     1,        "src_v1"},
    {"bind",      czl_sql_bind,      2,        "src_v1,arr_v2"},
};

#define CZL_MYSQL_MAX_FIELDS 1024
#define CZL_MYSQL_BUF_SIZE 50*1024

enum czl_sql_engine_enum
{
    CZL_SQL_ENGINE_SQLITE3,
    CZL_SQL_ENGINE_MYSQL
};

enum czl_sql_src_type_enum
{
    CZL_SQL_SRC_DB,
    CZL_SQL_SRC_STMT
};

enum czl_mysql_state_enum
{
    CZL_MYSQL_STATE_EXEC,
    CZL_MYSQL_STATE_RESULT,
    CZL_MYSQL_STATE_FETCH,
    CZL_MYSQL_STATE_END
};

void czl_sql_error_log(char *log_buf, const char *error)
{
    unsigned long len;

    if (!error) return;

    len = strlen(error);
    if (len >= CZL_LOG_BUF_SIZE)
        len = CZL_LOG_BUF_SIZE - 1;

    memcpy(log_buf, error, len);
    log_buf[len] = '\0';
}

char czl_sql_open(czl_gp *gp, czl_fun *fun)
{
    char *database = CZL_STR(fun->vars->val.str.Obj)->str;
    char *engine = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (czl_strcmp("sqlite3", engine))
    {
        sqlite3 *db;
        int rc;
        if ((rc=sqlite3_open(database, &db)) != SQLITE_OK)
            czl_sql_error_log(gp->log_buf, sqlite3_errstr(rc));
        else if (!(fun->ret.val.Obj=czl_extsrc_create(gp, db, sqlite3_close,
                                                      CZL_LIB_SQL_NAME, czl_lib_sql)))
        {
            sqlite3_close(db);
            return 0;
        }
        else
        {
            czl_extsrc *extsrc = CZL_SRC(fun->ret.val.Obj);
            extsrc->type = CZL_SQL_SRC_DB;
            extsrc->engine = CZL_SQL_ENGINE_SQLITE3;
            fun->ret.type = CZL_SOURCE;
        }
    }
    else if (czl_strcmp("mysql", engine))
    {
        MYSQL *db = mysql_init(NULL);
        char *host = CZL_STR(fun->vars[2].val.str.Obj)->str;
        int port = fun->vars[3].val.inum;
        char *usr = CZL_STR(fun->vars[4].val.str.Obj)->str;
        char *pwd = CZL_STR(fun->vars[5].val.str.Obj)->str;
        unsigned long timeout = fun->vars[6].val.inum; //单位: s
        unsigned long size;
        char *buf;
        if (!db ||
            mysql_options(db, MYSQL_OPT_CONNECT_TIMEOUT, &timeout) ||
            !mysql_real_connect(db, host, usr, pwd, NULL, port, NULL, CLIENT_MULTI_STATEMENTS))
            return 1;
        size = sizeof("create database ;") + CZL_STR(fun->vars->val.str.Obj)->len;
        if (!(buf=CZL_TMP_MALLOC(gp, size)))
        {
            mysql_close(db);
            return 0;
        }
        strcpy(buf, "create database ");
        strcat(buf, database);
        strcat(buf, ";");
        mysql_query(db, buf);
        strcpy(buf, "use ");
        strcat(buf, database);
        strcat(buf, ";");
        if (mysql_query(db, buf))
            ;
        else if (!(fun->ret.val.Obj=czl_extsrc_create(gp, db, mysql_close,
                                                      CZL_LIB_SQL_NAME, czl_lib_sql)))
        {
            mysql_close(db);
            CZL_TMP_FREE(gp, buf, size);
            return 0;
        }
        else
        {
            czl_extsrc *extsrc = CZL_SRC(fun->ret.val.Obj);
            extsrc->type = CZL_SQL_SRC_DB;
            extsrc->engine = CZL_SQL_ENGINE_MYSQL;
        #ifdef CZL_SYSTEM_WINDOWS
            extsrc->stdcall_flag = 1;
        #endif
            fun->ret.type = CZL_SOURCE;
        }
        CZL_TMP_FREE(gp, buf, size);
        if (!fun->ret.val.Obj && db)
        {
            czl_sql_error_log(gp->log_buf, mysql_error(db));
            mysql_close(db);
        }
    }
    else
        czl_sql_error_log(gp->log_buf, "SQL engine just surport sqlite3 and mysql");

    return 1;
}

char czl_sql_error(czl_gp *gp, czl_fun *fun)
{
    const char *error;

    if (CZL_SOURCE == fun->vars->type)
    {
        czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);
        if (!extsrc || extsrc->type != CZL_SQL_SRC_DB)
            return 1;
        else
        {
            if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
                error  = sqlite3_errmsg(extsrc->src);
            else //CZL_SQL_ENGINE_MYSQL
                error = mysql_error(extsrc->src);
        }
    }
    else
        error = gp->log_buf;

    return czl_set_ret_str(gp, &fun->ret, error, strlen(error));
}

char czl_sqlite3_bind_paras(czl_gp *gp, sqlite3_stmt *stmt, czl_var *var)
{
    czl_array *arr = CZL_ARR(var->val.Obj);
    unsigned long i, cnt = arr->cnt;
    czl_var *vars = arr->vars;

    if (cnt != sqlite3_bind_parameter_count(stmt))
        return 0;

    for (i = 0; i < cnt; ++i)
    {
        const unsigned char type = vars[i].type;
        if (CZL_INT == type)
        {
        #ifdef CZL_NUMBER_64bit
            sqlite3_bind_int64(stmt, i+1, vars[i].val.inum);
        #else
            sqlite3_bind_int(stmt, i+1, vars[i].val.inum);
        #endif
        }
        else if (CZL_FLOAT == type)
        {
            sqlite3_bind_double(stmt, i+1, vars[i].val.fnum);
        }
        else if (CZL_STRING == type)
        {
            czl_string *s = CZL_STR(vars[i].val.str.Obj);
            sqlite3_bind_blob(stmt, i+1, s->str, s->len, SQLITE_STATIC);
        }
        else
        {
            unsigned long size = 4;
            char buf[CZL_BUF_SIZE];
            char *tmp = buf;
            if (!czl_sizeof_obj(gp, 1, vars+i, &size) ||
                (size > CZL_BUF_SIZE && !(tmp=CZL_TMP_MALLOC(gp, size))))
                return 0;
            memcpy(tmp, &CZL_CHECK_SUM, 4);
            czl_get_obj_buf(gp, vars+i, tmp+4);
            sqlite3_bind_blob(stmt, i+1, tmp, size, SQLITE_STATIC);
            if (size > CZL_BUF_SIZE)
                CZL_TMP_FREE(gp, tmp, size);
        }
    }

    return 1;
}

char czl_mysql_bind_paras(czl_gp *gp, MYSQL_STMT *stmt, czl_var *var)
{
    char ret = 0;
    czl_array *arr = CZL_ARR(var->val.Obj);
    unsigned long i, j = arr->cnt, cnt = 0;
    czl_var *vars = arr->vars;
    MYSQL_BIND paras[CZL_MYSQL_MAX_FIELDS];
    struct czl_mysql_buf
    {
        char *buf;
        unsigned long size;
    } b[CZL_MYSQL_MAX_FIELDS];

    if (j > CZL_MYSQL_MAX_FIELDS || j != mysql_stmt_param_count(stmt))
        return 0;

    memset(paras, 0, sizeof(MYSQL_BIND)*j);

    for (i = 0; i < j; ++i)
    {
        const unsigned char type = vars[i].type;
        if (CZL_INT == type)
        {
        #ifdef CZL_NUMBER_64bit
            paras[i].buffer_type = MYSQL_TYPE_LONGLONG;
        #else
            paras[i].buffer_type = MYSQL_TYPE_LONG;
        #endif
            paras[i].buffer = &vars[i].val.inum;
        }
        else if (CZL_FLOAT == type)
        {
        #ifdef CZL_NUMBER_64bit
            paras[i].buffer_type = MYSQL_TYPE_DOUBLE;
        #else
            paras[i].buffer_type = MYSQL_TYPE_FLOAT;
        #endif
            paras[i].buffer = &vars[i].val.fnum;
        }
        else if (CZL_STRING == type)
        {
            czl_string *s = CZL_STR(vars[i].val.str.Obj);
            paras[i].buffer_type = MYSQL_TYPE_LONG_BLOB;
            paras[i].buffer = s->str;
            paras[i].buffer_length = s->len;
        }
        else
        {
            b[cnt].size = 4;
            if (!czl_sizeof_obj(gp, 1, vars+i, &b[cnt].size) ||
                !(b[cnt].buf=CZL_TMP_MALLOC(gp, b[cnt].size)))
                goto CZL_OOM;
            memcpy(b[cnt].buf, &CZL_CHECK_SUM, 4);
            czl_get_obj_buf(gp, vars+i, b[cnt].buf+4);
            paras[i].buffer_type = MYSQL_TYPE_LONG_BLOB;
            paras[i].buffer = b[cnt].buf;
            paras[i].buffer_length = b[cnt].size;
            ++cnt;
        }
    }

    ret = !mysql_stmt_bind_param(stmt, paras);

CZL_OOM:
    for (i = 0; i < cnt; ++i)
        CZL_TMP_FREE(gp, b[i].buf, b[i].size);
    return ret;
}

char czl_sql_exec(czl_gp *gp, czl_fun *fun)
{
    if (CZL_STRING == fun->vars[1].type)
    {
        czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);
        if (!extsrc || extsrc->type != CZL_SQL_SRC_DB)
            return 1;
        if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
        {
            char *sql = CZL_STR(fun->vars[1].val.str.Obj)->str;
            int rc;
            while (SQLITE_BUSY == (rc=sqlite3_exec(extsrc->src, sql, NULL, NULL, NULL)))
                CZL_SLEEP(1);
            fun->ret.val.inum = (SQLITE_OK == rc ? 1 : 0);
        }
        else //CZL_SQL_ENGINE_MYSQL
        {
            czl_string *sql = CZL_STR(fun->vars[1].val.str.Obj);
            fun->ret.val.inum = !mysql_real_query(extsrc->src, sql->str, sql->len);
        }
    }
    else if (CZL_ARRAY == fun->vars[1].type)
    {
        czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);
        if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
            return 1;
        if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
        {
            sqlite3_stmt *stmt = extsrc->src;
            int rc;
            while (SQLITE_BUSY == (rc=sqlite3_reset(stmt)))
                CZL_SLEEP(1);
            if (SQLITE_OK == rc && czl_sqlite3_bind_paras(gp, stmt, fun->vars+1))
            {
                while (SQLITE_BUSY == (rc=sqlite3_step(stmt)))
                    CZL_SLEEP(1);
                if (SQLITE_DONE == rc)
                    fun->ret.val.inum = 1;
            }
        }
        else //CZL_SQL_ENGINE_MYSQL
        {
            MYSQL_STMT *stmt = extsrc->src;
            if (czl_mysql_bind_paras(gp, stmt, fun->vars+1) && !mysql_stmt_execute(stmt))
                fun->ret.val.inum = 1;
        }
    }

    fun->ret.type = CZL_INT;
    return 1;
}

char czl_sql_prepare(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);
    char *sql = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (!extsrc || extsrc->type != CZL_SQL_SRC_DB)
        return 1;

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt;
        //多线程下用sqlite3_prepare_v2
        if (sqlite3_prepare_v2(extsrc->src, sql, -1, &stmt, NULL) != SQLITE_OK)
            return 1;
        else if (!(fun->ret.val.Obj=czl_extsrc_create(gp, stmt, sqlite3_finalize,
                                                      CZL_LIB_SQL_NAME, czl_lib_sql)))
        {
            sqlite3_finalize(stmt);
            return 0;
        }
        else
        {
            czl_extsrc *extsrc = CZL_SRC(fun->ret.val.Obj);
            extsrc->type = CZL_SQL_SRC_STMT;
            extsrc->engine = CZL_SQL_ENGINE_SQLITE3;
            fun->ret.type = CZL_SOURCE;
        }
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        MYSQL_STMT *stmt = mysql_stmt_init(extsrc->src);
        if (!stmt || mysql_stmt_prepare(stmt, sql, strlen(sql)))
            return 1;
        else if (!(fun->ret.val.Obj=czl_extsrc_create(gp, stmt, mysql_stmt_close,
                                                      CZL_LIB_SQL_NAME, czl_lib_sql)))
        {
            mysql_stmt_close(stmt);
            return 0;
        }
        else
        {
            czl_extsrc *extsrc = CZL_SRC(fun->ret.val.Obj);
            extsrc->type = CZL_SQL_SRC_STMT;
            extsrc->engine = CZL_SQL_ENGINE_MYSQL;
        #ifdef CZL_SYSTEM_WINDOWS
            extsrc->stdcall_flag = 1;
        #endif
            extsrc->state = CZL_MYSQL_STATE_EXEC;
            fun->ret.type = CZL_SOURCE;
        }
    }

    return 1;
}

char czl_sql_query(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
        return 1;

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt = extsrc->src;
        int rc;
        while (SQLITE_BUSY == (rc=sqlite3_step(stmt)))
            CZL_SLEEP(1);
        if (SQLITE_ROW == rc)
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
                #ifdef CZL_NUMBER_64bit
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
                    {
                        czl_array_delete(gp, obj);
                        return 0;
                    }
                    vars[i].type = CZL_STRING;
                    break;
                }
            }
            fun->ret.type = CZL_ARRAY;
            fun->ret.val.Obj = obj;
        }
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        MYSQL_STMT *stmt = extsrc->src;
        MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
        MYSQL_BIND paras[CZL_MYSQL_MAX_FIELDS];
        unsigned long lengths[CZL_MYSQL_MAX_FIELDS];
        unsigned long i, j, cnt, size = 0;
        MYSQL_FIELD *fields;
        char buf[CZL_MYSQL_BUF_SIZE];
        char *tmp = buf;
        void **obj;
        czl_var *vars;
        if (!res)
            return 1;
        if (CZL_MYSQL_STATE_EXEC == extsrc->state)
        {
            if (mysql_stmt_execute(stmt))
                return 1;
            extsrc->state = CZL_MYSQL_STATE_RESULT;
        }
        if (CZL_MYSQL_STATE_RESULT == extsrc->state)
        {
            if (mysql_stmt_store_result(stmt))
                return 1;
            extsrc->state = CZL_MYSQL_STATE_FETCH;
            if (mysql_stmt_num_rows(stmt) == 0)
            {
                extsrc->state = CZL_MYSQL_STATE_EXEC;
                return 1;
            }
        }
        else if (CZL_MYSQL_STATE_END == extsrc->state)
        {
            extsrc->state = CZL_MYSQL_STATE_EXEC;
            return 1;
        }
        cnt = mysql_num_fields(res);
        fields = mysql_fetch_fields(res);
        for (i = 0; i < cnt; ++i)
        {
            if (fields[i].type > MYSQL_TYPE_INT24)
                size += fields[i].length;
        }
        if (!(obj=czl_array_create(gp, cnt, cnt)))
            return 0;
        if (size > CZL_MYSQL_BUF_SIZE && !(tmp=CZL_TMP_MALLOC(gp, size)))
        {
            czl_array_delete(gp, obj);
            return 0;
        }
        vars = CZL_ARR(obj)->vars;
        memset(paras, 0, sizeof(MYSQL_BIND)*cnt);
        for (i = j = 0; i < cnt; ++i)
        {
            const enum enum_field_types type = fields[i].type;
            paras[i].buffer_type = type;
            if (type <= MYSQL_TYPE_INT24)
            {
                paras[i].buffer = &vars[i].val.inum;
                if (MYSQL_TYPE_DECIMAL == type ||
                    MYSQL_TYPE_FLOAT == type || MYSQL_TYPE_DOUBLE == type)
                    vars[i].type = CZL_FLOAT;
            }
            else
            {
                vars[i].type = CZL_STRING;
                paras[i].buffer = tmp+j;
                paras[i].buffer_length = fields[i].length;
                paras[i].length = lengths+i;
                j += fields[i].length;
            }
        }
        if (mysql_stmt_bind_result(stmt, paras) || mysql_stmt_fetch(stmt))
        {
            czl_array_delete(gp, obj);
            if (size > CZL_MYSQL_BUF_SIZE)
                CZL_TMP_FREE(gp, tmp, size);
            return 1;
        }
        for (i = j = 0; i < cnt; ++i)
        {
            const enum enum_field_types type = fields[i].type;
        #ifdef CZL_NUMBER_64bit
            if (MYSQL_TYPE_FLOAT == type)
                vars[i].val.fnum = vars[i].val.ext.v1.f32;
            else
        #endif
            if (type > MYSQL_TYPE_INT24)
            {
                if (!czl_str_create(gp, &vars[i].val.str, lengths[i]+1, lengths[i], tmp+j))
                {
                    czl_array_delete(gp, obj);
                    if (size > CZL_MYSQL_BUF_SIZE)
                        CZL_TMP_FREE(gp, tmp, size);
                    return 0;
                }
                vars[i].type = CZL_STRING;
                j += fields[i].length;
            }
        }
        if (!mysql_stmt_row_tell(stmt) && !mysql_stmt_free_result(extsrc->src))
            extsrc->state = CZL_MYSQL_STATE_END;
        fun->ret.type = CZL_ARRAY;
        fun->ret.val.Obj = obj;
        if (size > CZL_MYSQL_BUF_SIZE)
            CZL_TMP_FREE(gp, tmp, size);
    }

    return 1;
}

char czl_sql_reset(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
        return 1;

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        int rc;
        while (SQLITE_BUSY == (rc=sqlite3_reset(extsrc->src)))
            CZL_SLEEP(1);
        fun->ret.val.inum = (SQLITE_OK == rc ? 1 : 0);
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        fun->ret.val.inum = !mysql_stmt_free_result(extsrc->src);
        if (fun->ret.val.inum)
            extsrc->state = CZL_MYSQL_STATE_EXEC;
    }

    fun->ret.type = CZL_INT;
    return 1;
}

char czl_sql_bind(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
        return 1;

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt = extsrc->src;
        int rc;
        while (SQLITE_BUSY == (rc=sqlite3_reset(stmt)))
            CZL_SLEEP(1);
        if (SQLITE_OK == rc)
            fun->ret.val.inum = czl_sqlite3_bind_paras(gp, stmt, fun->vars+1);
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        fun->ret.val.inum = czl_mysql_bind_paras(gp, extsrc->src, fun->vars+1);
    }

    fun->ret.type = CZL_INT;
    return 1;
}
#endif //CZL_LIB_SQL

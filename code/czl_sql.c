#include "czl_sql.h"

#ifdef CZL_LIB_SQL

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_sql_open(czl_gp *gp, czl_fun *fun);    //打开数据库
char czl_sql_close(czl_gp *gp, czl_fun *fun);   //关闭数据库
char czl_sql_exec(czl_gp *gp, czl_fun *fun);    //执行sql批量获取结果
char czl_sql_prepare(czl_gp *gp, czl_fun *fun); //编译sql
char czl_sql_query(czl_gp *gp, czl_fun *fun);   //执行编译sql查询
char czl_sql_reset(czl_gp *gp, czl_fun *fun);   //重置query光标
char czl_sql_bind(czl_gp *gp, czl_fun *fun);    //执行编译sql参数绑定
char czl_sql_free(czl_gp *gp, czl_fun *fun);    //释放编译资源

//库函数表定义
const czl_sys_fun czl_lib_sql[] =
{
    //函数名,      函数指针,           参数个数,  参数声明
    {"open",      czl_sql_open,      7,        "str_v1,str_v2=\"sqlite3\",str_v3=\"\",int_v4=0,str_v5=\"\",str_v6=\"\",int_v7=-1"},
    {"close",     czl_sql_close,     1,        "int_v1"},
    {"exec",      czl_sql_exec,      2,        "int_v1"},
    {"prepare",   czl_sql_prepare,   2,        "int_v1,str_v2"},
    {"query",     czl_sql_query,     1,        "int_v1"},
    {"reset",     czl_sql_reset,     1,        "int_v1"},
    {"bind",      czl_sql_bind,      2,        "int_v1,arr_v2"},
    {"free",      czl_sql_free,      1,        "int_v1"},
};

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

char czl_sql_open(czl_gp *gp, czl_fun *fun)
{
    char *database = CZL_STR(fun->vars->val.str.Obj)->str;
    char *engine = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (czl_strcmp("sqlite3", engine))
    {
        sqlite3 *db;
        if (sqlite3_open(database, &db) != SQLITE_OK)
            fun->ret.val.inum = 0;
        else if (!(fun->ret.val.inum=czl_extsrc_create(gp, db, sqlite3_close, czl_lib_sql)))
            return 0;
        else
        {
            czl_extsrc *extsrc = (czl_extsrc*)((unsigned long)fun->ret.val.inum);
            extsrc->engine = CZL_SQL_ENGINE_SQLITE3;
            extsrc->type = CZL_SQL_SRC_DB;
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
        if (!db ||
            mysql_options(db, MYSQL_OPT_CONNECT_TIMEOUT, &timeout) ||
            !mysql_real_connect(db, host, usr, pwd, database, port,
                                NULL, CLIENT_MULTI_STATEMENTS))
        {
            fun->ret.val.inum = 0;
            if (db)
                mysql_close(db);
        }
        else
        {
            unsigned long size = sizeof("create database ;") +
                                 CZL_STR(fun->vars->val.str.Obj)->len;
            char *buf = CZL_TMP_MALLOC(gp, size);
            if (!buf)
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
            {
                fun->ret.val.inum = 0;
                mysql_close(db);
            }
            else if (!(fun->ret.val.inum=czl_extsrc_create(gp, db, mysql_close, czl_lib_sql)))
            {
                CZL_TMP_FREE(gp, buf, size);
                return 0;
            }
            else
            {
                czl_extsrc *extsrc = (czl_extsrc*)((unsigned long)fun->ret.val.inum);
                extsrc->engine = CZL_SQL_ENGINE_MYSQL;
                extsrc->type = CZL_SQL_SRC_DB;
            #ifdef CZL_SYSTEM_WINDOWS
                extsrc->stdcall_flag = 1;
            #endif
            }
            CZL_TMP_FREE(gp, buf, size);
        }
    }
    else
        fun->ret.val.inum = 0;

    return 1;
}

char czl_sql_close(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_extsrc_delete(gp, fun->vars->val.inum);
    return 1;
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
    czl_array *arr = CZL_ARR(var->val.Obj);
    unsigned long i, cnt = arr->cnt;
    czl_var *vars = arr->vars;
    MYSQL_BIND paras[1024];

    if (cnt > 1024 || cnt != mysql_stmt_param_count(stmt))
        return 0;

    memset(paras, 0, sizeof(MYSQL_BIND)*cnt);

    for (i = 0; i < cnt; ++i)
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
            unsigned long size = 4;
            char buf[CZL_BUF_SIZE];
            char *tmp = buf;
            if (!czl_sizeof_obj(gp, 1, vars+i, &size) ||
                (size > CZL_BUF_SIZE && !(tmp=CZL_TMP_MALLOC(gp, size))))
                return 0;
            memcpy(tmp, &CZL_CHECK_SUM, 4);
            czl_get_obj_buf(gp, vars+i, tmp+4);
            paras[i].buffer_type = MYSQL_TYPE_LONG_BLOB;
            paras[i].buffer = tmp;
            paras[i].buffer_length = size;
            if (size > CZL_BUF_SIZE)
                CZL_TMP_FREE(gp, tmp, size);
        }
    }

    return !mysql_stmt_bind_param(stmt, paras);
}

char czl_sql_exec(czl_gp *gp, czl_fun *fun)
{
    if (CZL_STRING == fun->vars[1].type)
    {
        czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);
        if (!extsrc || extsrc->type != CZL_SQL_SRC_DB)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
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
        czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);
        if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
        {
            sqlite3_stmt *stmt = extsrc->src;
            int rc;
            while (SQLITE_BUSY == (rc=sqlite3_reset(stmt)))
                CZL_SLEEP(1);
            if (rc != SQLITE_OK)
                fun->ret.val.inum = 0;
            else
            {
                if (!czl_sqlite3_bind_paras(gp, stmt, fun->vars+1))
                    fun->ret.val.inum = 0;
                else
                {
                    while (SQLITE_BUSY == (rc=sqlite3_step(stmt)))
                        CZL_SLEEP(1);
                    fun->ret.val.inum = (SQLITE_DONE == rc ? 1 : 0);
                }
            }
        }
        else //CZL_SQL_ENGINE_MYSQL
        {
            MYSQL_STMT *stmt = extsrc->src;
            if (!czl_mysql_bind_paras(gp, stmt, fun->vars+1) || mysql_stmt_execute(stmt))
                fun->ret.val.inum = 0;
            else
                fun->ret.val.inum = 1;
        }
    }
    else
        fun->ret.val.inum = 0;

    return 1;
}

char czl_sql_prepare(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);
    char *sql = CZL_STR(fun->vars[1].val.str.Obj)->str;

    if (!extsrc || extsrc->type != CZL_SQL_SRC_DB)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt;
        //多线程下用sqlite3_prepare_v2
        if (sqlite3_prepare_v2(extsrc->src, sql, -1, &stmt, NULL) != SQLITE_OK)
            fun->ret.val.inum = 0;
        else if (!(fun->ret.val.inum=czl_extsrc_create(gp, stmt, sqlite3_finalize, czl_lib_sql)))
            return 0;
        else
        {
            czl_extsrc *extsrc = (czl_extsrc*)((unsigned long)fun->ret.val.inum);
            extsrc->engine = CZL_SQL_ENGINE_SQLITE3;
            extsrc->type = CZL_SQL_SRC_STMT;
        }
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        MYSQL_STMT *stmt = mysql_stmt_init(extsrc->src);
        if (!stmt || mysql_stmt_prepare(stmt, sql, strlen(sql)))
            fun->ret.val.inum = 0;
        else if (!(fun->ret.val.inum=czl_extsrc_create(gp, stmt, mysql_stmt_close, czl_lib_sql)))
            return 0;
        else
        {
            czl_extsrc *extsrc = (czl_extsrc*)((unsigned long)fun->ret.val.inum);
            extsrc->engine = CZL_SQL_ENGINE_MYSQL;
            extsrc->type = CZL_SQL_SRC_STMT;
        #ifdef CZL_SYSTEM_WINDOWS
            extsrc->stdcall_flag = 1;
        #endif
            extsrc->state = CZL_MYSQL_STATE_EXEC;
        }
    }

    return 1;
}

char czl_sql_query(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt = extsrc->src;
        int rc;
        while (SQLITE_BUSY == (rc=sqlite3_step(stmt)))
            CZL_SLEEP(1);
        if (rc != SQLITE_ROW)
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
        MYSQL_BIND paras[1024];
        unsigned long lengths[1024];
        unsigned long i, cnt;
        MYSQL_FIELD *fields;
        void **obj;
        czl_var *vars;
        fun->ret.val.inum = 0;
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
        if (!(obj=czl_array_create(gp, cnt, cnt)))
            return 0;
        vars = CZL_ARR(obj)->vars;
        fields = mysql_fetch_fields(res);
        memset(paras, 0, sizeof(MYSQL_BIND)*cnt);
        for (i = 0; i < cnt; ++i)
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
                if (!czl_str_create(gp, &vars[i].val.str, fields[i].length+1, 0, NULL))
                {
                    czl_array_delete(gp, obj);
                    return 0;
                }
                vars[i].type = CZL_STRING;
                paras[i].buffer = CZL_STR(vars[i].val.str.Obj)->str;
                paras[i].buffer_length = fields[i].length;
                paras[i].length = lengths+i;
            }
        }
        if (mysql_stmt_bind_result(stmt, paras) || mysql_stmt_fetch(stmt))
        {
            czl_array_delete(gp, obj);
            return 1;
        }
        for (i = 0; i < cnt; ++i)
        {
            const enum enum_field_types type = fields[i].type;
        #ifdef CZL_NUMBER_64bit
            if (MYSQL_TYPE_FLOAT == type)
                vars[i].val.fnum = vars[i].val.ext.v1.f32;
            else
        #endif
            if (type > MYSQL_TYPE_INT24)
            {
                void **obj = CZL_SR(gp, vars[i].val.str, lengths[i]+1);
                if (!obj)
                {
                    czl_array_delete(gp, obj);
                    return 0;
                }
                vars[i].val.str.Obj = obj;
                CZL_STR(vars[i].val.str.Obj)->len = lengths[i];
                vars[i].val.str.size = lengths[i]+1;
            }
        }
        if (!mysql_stmt_row_tell(stmt) && !mysql_stmt_free_result(extsrc->src))
            extsrc->state = CZL_MYSQL_STATE_END;
        fun->ret.type = CZL_ARRAY;
        fun->ret.val.Obj = obj;
    }

    return 1;
}

char czl_sql_reset(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

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

    return 1;
}

char czl_sql_bind(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(gp, fun->vars->val.inum, czl_lib_sql);

    if (!extsrc || extsrc->type != CZL_SQL_SRC_STMT)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (CZL_SQL_ENGINE_SQLITE3 == extsrc->engine)
    {
        sqlite3_stmt *stmt = extsrc->src;
        int rc;
        while (SQLITE_BUSY == (rc=sqlite3_reset(stmt)))
            CZL_SLEEP(1);
        if (rc != SQLITE_OK)
            fun->ret.val.inum = 0;
        else
            fun->ret.val.inum = czl_sqlite3_bind_paras(gp, stmt, fun->vars+1);
    }
    else //CZL_SQL_ENGINE_MYSQL
    {
        fun->ret.val.inum = czl_mysql_bind_paras(gp, extsrc->src, fun->vars+1);
    }

    return 1;
}

char czl_sql_free(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_extsrc_delete(gp, fun->vars->val.inum);
    return 1;
}
#endif //CZL_LIB_SQL

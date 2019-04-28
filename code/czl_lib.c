#include "czl_lib.h"
#include "czl_opt.h"
#include "czl_paser.h"
///////////////////////////////////////////////////////////////
char czl_sys_echo(czl_gp*, czl_fun*);
char czl_sys_print(czl_gp*, czl_fun*);
#ifdef CZL_CONSOLE
char czl_sys_input(czl_gp*, czl_fun*);
#endif //CZL_CONSOLE
//
char czl_sys_open(czl_gp*, czl_fun*);
char czl_sys_close(czl_gp*, czl_fun*);
char czl_sys_write(czl_gp*, czl_fun*);
char czl_sys_read(czl_gp*, czl_fun*);
//
char czl_sys_srand(czl_gp*, czl_fun*);
char czl_sys_rand(czl_gp*, czl_fun*);
char czl_sys_rrand(czl_gp*, czl_fun*);
//
char czl_sys_int(czl_gp*, czl_fun*);
char czl_sys_float(czl_gp*, czl_fun*);
char czl_sys_abs(czl_gp*, czl_fun*);
char czl_sys_exp(czl_gp*, czl_fun*);
char czl_sys_log(czl_gp*, czl_fun*);
char czl_sys_log10(czl_gp*, czl_fun*);
char czl_sys_pow(czl_gp*, czl_fun*);
char czl_sys_sqrt(czl_gp*, czl_fun*);
char czl_sys_ceil(czl_gp*, czl_fun*);
char czl_sys_floor(czl_gp*, czl_fun*);
char czl_sys_sin(czl_gp*, czl_fun*);
char czl_sys_asin(czl_gp*, czl_fun*);
char czl_sys_sinh(czl_gp*, czl_fun*);
char czl_sys_cos(czl_gp*, czl_fun*);
char czl_sys_acos(czl_gp*, czl_fun*);
char czl_sys_cosh(czl_gp*, czl_fun*);
char czl_sys_tan(czl_gp*, czl_fun*);
char czl_sys_atan(czl_gp*, czl_fun*);
char czl_sys_tanh(czl_gp*, czl_fun*);
//
char czl_sys_str(czl_gp*, czl_fun*);
char czl_sys_split(czl_gp*, czl_fun*);
char czl_sys_upper(czl_gp*, czl_fun*);
char czl_sys_lower(czl_gp*, czl_fun*);
//
char czl_sys_abort(czl_gp*, czl_fun*);
char czl_sys_assert(czl_gp*, czl_fun*);
char czl_sys_errline(czl_gp*, czl_fun*);
char czl_sys_errfile(czl_gp*, czl_fun*);
char czl_sys_errcode(czl_gp*, czl_fun*);
char czl_sys_setfun(czl_gp*, czl_fun*);
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_sleep(czl_gp*, czl_fun*);
char czl_sys_clock(czl_gp*, czl_fun*);
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_ltime(czl_gp*, czl_fun*);
char czl_sys_usemem(czl_gp*, czl_fun*);
char czl_sys_maxmem(czl_gp*, czl_fun*);
char czl_sys_setmem(czl_gp*, czl_fun*);
#ifdef CZL_MM_MODULE
#ifdef CZL_MM_CACHE
char czl_sys_setcache(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MM_CACHE
char czl_sys_setrank(czl_gp*, czl_fun*);
char czl_sys_setsa(czl_gp*, czl_fun*);
char czl_sys_setgc(czl_gp*, czl_fun*);
char czl_sys_gc(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MM_MODULE
char czl_sys_runshl(czl_gp*, czl_fun*);
//
char czl_sys_hcac(czl_gp*, czl_fun*);
char czl_sys_hdod(czl_gp*, czl_fun*);
//
char czl_sys_tp(czl_gp*, czl_fun*);
char czl_sys_ot(czl_gp*, czl_fun*);
char czl_sys_sz(czl_gp*, czl_fun*);
char czl_sys_rc(czl_gp*, czl_fun*);
//
char czl_sys_push(czl_gp*, czl_fun*);
char czl_sys_pop(czl_gp*, czl_fun*);
char czl_sys_in(czl_gp*, czl_fun*);
char czl_sys_out(czl_gp*, czl_fun*);
char czl_sys_ins(czl_gp*, czl_fun*);
//
char czl_sys_coroutine(czl_gp*, czl_fun*);
char czl_sys_resume(czl_gp*, czl_fun*);
char czl_sys_corsta(czl_gp*, czl_fun*);
char czl_sys_reset(czl_gp*, czl_fun*);
char czl_sys_kill(czl_gp*, czl_fun*);
//
#ifdef CZL_MULT_THREAD
char czl_sys_thread(czl_gp*, czl_fun*);
char czl_sys_wait(czl_gp*, czl_fun*);
char czl_sys_listen(czl_gp*, czl_fun*);
char czl_sys_waitfor(czl_gp*, czl_fun*);
char czl_sys_report(czl_gp*, czl_fun*);
char czl_sys_notify(czl_gp*, czl_fun*);
char czl_sys_notifyall(czl_gp*, czl_fun*);
char czl_sys_thrsta(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MULT_THREAD
//
#ifndef CZL_CONSOLE
char czl_sys_cleanlog(czl_gp*, czl_fun*);
char czl_sys_openlog(czl_gp*, czl_fun*);
char czl_sys_closelog(czl_gp*, czl_fun*);
char czl_sys_insert(czl_gp*, czl_fun*);
char czl_sys_get(czl_gp*, czl_fun*);
char czl_sys_delete(czl_gp*, czl_fun*);
char czl_sys_count(czl_gp*, czl_fun*);
char czl_sys_clean(czl_gp*, czl_fun*);
#endif //#ifndef CZL_CONSOLE
///////////////////////////////////////////////////////////////
const czl_sys_fun czl_lib_os[] =
{
    //命令行输入输出函数
    {"echo",      czl_sys_echo,       -1, NULL},
    {"print",     czl_sys_print,      -1, NULL},
#ifdef CZL_CONSOLE
    {"input",     czl_sys_input,      0,  NULL},
#endif //CZL_CONSOLE
    //文件操作函数
    {"open",      czl_sys_open,       4,  "str_v1,str_v2=\"wb+\",int_v3=1,int_v4=32"},
    {"close",     czl_sys_close,      1,  "&v1"},
    {"write",     czl_sys_write,      -2, NULL},
    {"read",      czl_sys_read,       2,  "&v2"},
    //随机数函数
    {"srand",     czl_sys_srand,      1,  "int_v1"},
    {"rand",      czl_sys_rand,       0,  NULL},
    {"rrand",     czl_sys_rrand,      2,  "int_v1,int_v2"},
    //数值处理函数
    {"int",       czl_sys_int,        1,  NULL},
    {"float",     czl_sys_float,      1,  NULL},
    {"abs",       czl_sys_abs,        1,  "int_v1"},
    {"exp",       czl_sys_exp,        1,  "float_v1"},
    {"log",       czl_sys_log,        1,  "float_v1"},
    {"log10",     czl_sys_log10,      1,  "float_v1"},
    {"pow",       czl_sys_pow,        2,  "float_v1,int_v2"},
    {"sqrt",      czl_sys_sqrt,       1,  "float_v1"},
    {"ceil",      czl_sys_ceil,       1,  "float_v1"},
    {"floor",     czl_sys_floor,      1,  "float_v1"},
    {"sin",       czl_sys_sin,        1,  "float_v1"},
    {"asin",      czl_sys_asin,       1,  "float_v1"},
    {"sinh",      czl_sys_sinh,       1,  "float_v1"},
    {"cos",       czl_sys_cos,        1,  "float_v1"},
    {"acos",      czl_sys_acos,       1,  "float_v1"},
    {"cosh",      czl_sys_cosh,       1,  "float_v1"},
    {"tan",       czl_sys_tan,        1,  "float_v1"},
    {"atan",      czl_sys_atan,       1,  "float_v1"},
    {"tanh",      czl_sys_tanh,       1,  "float_v1"},
    //字符串处理函数
    {"str",       czl_sys_str,        2,  "num_v1,int_v2=6"},
    {"split",     czl_sys_split,      2,  "str_v1,str_v2=\" \""},
    {"upper",     czl_sys_upper,      3,  "&str_v1,int_v2=0,int_v3=-1"},
    {"lower",     czl_sys_lower,      3,  "&str_v1,int_v2=0,int_v3=-1"},
//    {"memset",    czl_sys_memset,     3,  "&str_v1,int_v2,int_v3"},
//    {"memget",    czl_sys_memget,     3,  "str_v1,int_v2,int_v3"},
//    {"memspn",    czl_sys_memspn,     3,  "str_v1,str_v2,int_v3=0"},
//    {"memrep",    czl_sys_memrep,     3,  "&str_v1,str_v2,str_v3"},
//    {"memcmp",    czl_sys_memcmp,     2,  "str_v1,str_v2"},
    //系统函数
    {"abort",     czl_sys_abort,      0,  NULL},
    {"assert",    czl_sys_assert,     1,  NULL},
    {"errline",   czl_sys_errline,    0,  NULL},
    {"errfile",   czl_sys_errfile,    0,  NULL},
    {"errcode",   czl_sys_errcode,    0,  NULL},
    {"setfun",    czl_sys_setfun,     2,  "fun_v1,int_v2=0"},
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"sleep",     czl_sys_sleep,      1,  "int_v1"},
    {"clock",     czl_sys_clock,      1,  "int_v1=0"},
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"ltime",     czl_sys_ltime,      1,  "str_v1=\"Y-M-D h:m:s\""},
    {"usemem",    czl_sys_usemem,     0,  NULL},
    {"maxmem",    czl_sys_maxmem,     0,  NULL},
    {"setmem",    czl_sys_setmem,     1,  "int_v1"},
#ifdef CZL_MM_MODULE
    #ifdef CZL_MM_CACHE
    {"setcache",  czl_sys_setcache,   1,  "int_v1"},
    #endif //#ifdef CZL_MM_CACHE
    {"setrank",   czl_sys_setrank,    1,  "int_v1"},
    {"setsa",     czl_sys_setsa,      1,  "int_v1"},
    {"setgc",     czl_sys_setgc,      1,  "int_v1"},
    {"gc",        czl_sys_gc,         0,  NULL},
#endif //#ifdef CZL_MM_MODULE
    {"runshl",    czl_sys_runshl,     3,  "str_v1,&v2,v3=0"},
    //排序函数
    //{"sort",      czl_sys_sort,       2,  "int_v2=0"},
    //哈希表内建函数
    {"hcac",      czl_sys_hcac,       1,  "table_v1"},
    {"hdod",      czl_sys_hdod,       3,  "table_v1,int_v2,int_v3"},
    //变量内建函数
    {"tp",        czl_sys_tp,         1,  NULL},
    {"ot",        czl_sys_ot,         1,  "&v1"},
    {"sz",        czl_sys_sz,         1,  NULL},
    {"rc",        czl_sys_rc,         1,  "&v1"},
    //复合类型变量内建函数
    {"push",      czl_sys_push,       2,  "&stack_v1"},
    {"pop",       czl_sys_pop,        1,  "&stack_v1"},
    {"in",        czl_sys_in,         2,  "&queue_v1"},
    {"out",       czl_sys_out,        1,  "&queue_v1"},
    {"ins",       czl_sys_ins,        4,  "&table_v1"},
    //协程框架接口函数
    {"coroutine", czl_sys_coroutine,  2,  "fun_v1,int_v2=0"},
    {"resume",    czl_sys_resume,     -1, NULL},
    {"corsta",    czl_sys_corsta,     1,  "int_v1"},
    {"reset",     czl_sys_reset,      1,  NULL},
    //线程和协程共用kill函数
    {"kill",      czl_sys_kill,       1,  "int_v1"},
#ifdef CZL_MULT_THREAD
    //多线程框架接口函数
    #ifdef CZL_CONSOLE
    {"thread",    czl_sys_thread,     2,  "str_v1,v2=0"},
    #else
    {"thread",    czl_sys_thread,     3,  "str_v1,v2=0,str_v3=\"\""},
    #endif
    {"wait",      czl_sys_wait,       1,  "int_v1=0"},
    {"listen",    czl_sys_listen,     1,  "int_v1=0"},
    {"waitfor",   czl_sys_waitfor,    1,  "int_v1"},
    {"report",    czl_sys_report,     1,  NULL},
    {"notify",    czl_sys_notify,     2,  "int_v1,v2=0"},
    {"notifyall", czl_sys_notifyall,  1,  "v1=0"},
    {"thrsta",    czl_sys_thrsta,     1,  "int_v1=0"},
#endif //#ifdef CZL_MULT_THREAD
#ifndef CZL_CONSOLE
    //与C/C++栈交互接口函数
    {"cleanlog",  czl_sys_cleanlog,   0,  NULL},
    {"openlog",   czl_sys_openlog,    1,  "str_v1"},
    {"closelog",  czl_sys_closelog,   0,  NULL},
    {"insert",    czl_sys_insert,     2,  "int_v1"},
    {"get",       czl_sys_get,        1,  "int_v1"},
    {"delete",    czl_sys_delete,     1,  "int_v1"},
    {"count",     czl_sys_count,      0,  NULL},
    {"clean",     czl_sys_clean,      0,  NULL},
#endif //#ifndef CZL_CONSOLE
};
///////////////////////////////////////////////////////////////
#include "czl_com.h" //串口通信库头文件
const czl_sys_lib czl_syslibs[] =
{
    //库名,             库指针,       库函数个数
    {CZL_SYS_LIB_NAME, czl_lib_os,  sizeof(czl_lib_os)/sizeof(czl_sys_fun)},
#ifdef CZL_LIB_COM
    {"com",            czl_lib_com, 5}, //串口通信库
#endif //CZL_LIB_COM
};
const unsigned long czl_syslibs_num = sizeof(czl_syslibs)/sizeof(czl_sys_lib);
///////////////////////////////////////////////////////////////
char czl_print_obj(czl_gp *gp, const czl_var*, FILE*);
char czl_sizeof_obj(czl_gp*, char, const czl_var*, unsigned long*);
char* czl_get_obj_buf(czl_gp*, const czl_var*, char*);
char* czl_analysis_ele_buf(czl_gp*, char*, czl_var*);
///////////////////////////////////////////////////////////////
czl_var* CZL_GCRV(czl_var *ref)
{
    czl_var *var = CZL_GRV(ref);
    return (ref->ot == var->type ? var : NULL);
}

char czl_ret_clean(czl_gp *gp, czl_var *var)
{
    if (!czl_val_del(gp, var))
        return 0;

    var->type = CZL_INT;
    var->val.inum = 0;
    return 1;
}

char czl_set_ret_str
(
    czl_gp *gp,
    czl_var *ret,
    const char *str,
    unsigned long len
)
{
    if (!czl_str_create(gp, &ret->val.str, len+1, len, str))
        return 0;
    ret->type = CZL_STRING;

    return 1;
}
///////////////////////////////////////////////////////////////
int czl_get_int_mode(char *mode)
{
    while (*(++mode) != 'd') ;

#ifdef CZL_SYSTEM_LINUX
    #ifdef CZL_SYSTEM_64bit
        strcpy(mode, "lld");
        return 4;
    #else
        strcpy(mode, "ld");
        return 3;
    #endif
#elif defined CZL_SYSTEM_WINDOWS
    #ifdef CZL_SYSTEM_64bit
        strcpy(mode, "I64d");
        return 5;
    #else
        strcpy(mode, "ld");
        return 3;
    #endif
#else
    strcpy(mode, "ld");
    return 3;
#endif
}

char* czl_print_modify_check
(
    char *s,
    char *modify,
    char *modify_type,
    char value_type
)
{
    //%-1.2d
    //%-1.2f
    //%o %x %b %c %d %f

    switch (*s)
    {
    case 'o':
        s++;
        *modify_type = CZL_OBJ_REF;
        break;
    case 'x': case 'X': case 'b': case 'B':
        if (value_type != CZL_STRING)
            return NULL;
        *modify_type = *s++;
        break;
    case 'c':
        if (value_type != CZL_INT && value_type != CZL_FLOAT)
            return NULL;
        *(modify++) = *(s++);
        *modify_type = CZL_CHAR;
        break;
    default: //%d %f
        if (value_type != CZL_INT && value_type != CZL_FLOAT)
            return NULL;
        if ('-' == *s)
            *(modify++) = *(s++);
        if (*s >= '0' && *s <= '9')
        {
            *(modify++) = *(s++);
            if (*s >= '0' && *s <= '9')
                *(modify++) = *(s++);
            if ('.' == *s)
            {
                *(modify++) = *(s++);
                if (*s >= '0' && *s <= '9')
                {
                    *(modify++) = *(s++);
                    if (*s >= '0' && *s <= '9')
                        *(modify++) = *(s++);
                }
            }
        }
        else if ('.' == *s)
        {
            *(modify++) = *(s++);
            if (*s >= '0' && *s <= '9')
                *(modify++) = *(s++);
        }

        if ('d' == *s)
        {
            *(modify++) = *(s++);
            *modify_type = CZL_INT;
        }
        else if ('f' == *s)
        {
            *(modify++) = 'l';
            *(modify++) = *(s++);
            *modify_type = CZL_FLOAT;
        }
        else
            return NULL;
        break;
    }

    *modify = '\0';

    return s-1;
}

void czl_print_ins(czl_gp *gp, czl_ins *ins, FILE *fout)
{
    czl_var *var = CZL_GIV(ins);
    czl_class_var *v = ins->pclass->vars;
    char *name = ins->pclass->name;
    void ***parents = ins->parents;
	czl_class_parent *p;

    fprintf(fout, "%s(", name);
    while (v)
    {
        fprintf(fout, "%s=", v->Name);
        if (CZL_STATIC_VAR == v->quality)
            czl_print_obj(gp, (czl_var*)v, fout);
        else
            czl_print_obj(gp, var++, fout);
        v = v->next;
        if (v)
            fprintf(fout, ", ");
    }
    fprintf(fout, ")");

    for (p = ins->pclass->parents; p; p = p->next)
    {
        fprintf(fout, ", %s:", name);
        if (CZL_PROTECTED == p->permission)
            fprintf(fout, "*");
        else if (CZL_PRIVATE == p->permission)
            fprintf(fout, "!");
        czl_print_ins(gp, CZL_INS(*parents++), fout);
    }
}

void czl_print_tab(czl_gp *gp, const czl_table *tab, FILE *fout)
{
	czl_tabkv *p;

    fprintf(fout, "{");

    for (p = tab->eles_head; p; p = p->next)
    {
        if (CZL_STRING == p->kt)
            fprintf(fout, "\"%s\":", CZL_STR(p->key.str.Obj)->str);
        else
        {
            char mode[8] = "%d";
            int len = czl_get_int_mode(mode);
            mode[len] = ':';
            mode[len+1] = '\0';
            fprintf(fout, mode, p->key.inum);
        }
        czl_print_obj(gp, (czl_var*)p, fout);
        if (p->next)
            fprintf(fout, ", ");
    }

    fprintf(fout, "}");
}

void czl_print_arr(czl_gp *gp, const czl_array *arr, FILE *fout)
{
    unsigned long i;
    czl_var *var;

    fprintf(fout, "[");
    for (i = 0; i < arr->cnt; i++)
    {
        var = arr->vars + i;
        czl_print_obj(gp, var, fout);
        if (i+1 < arr->cnt)
            fprintf(fout, ", ");
    }
    fprintf(fout, "]");
}

void czl_print_sq(czl_gp *gp, const czl_sq *sq, FILE *fout)
{
    czl_glo_var *ele;

    fprintf(fout, "<");
    for (ele = sq->eles_head; ele; ele = ele->next)
    {
        czl_print_obj(gp, (czl_var*)ele, fout);
        if (ele->next)
            fprintf(fout, ", ");
    }
    fprintf(fout, ">");
}

char czl_print_arr_list(czl_gp *gp, const czl_array_list *list, FILE *fout)
{
    czl_var *ret;
    czl_para *para;

    fprintf(fout, "[");
    for (para = list->paras; para; para = para->next)
    {
        if (!(ret=czl_exp_cac(gp, para->para)))
            return 0;
        czl_print_obj(gp, ret, fout);
        if (para->next)
            fprintf(fout, ", ");
    }
    fprintf(fout, "]");

    return 1;
}

char czl_print_tab_list(czl_gp *gp, const czl_table_list *list, FILE *fout)
{
    czl_var *ret, tmp;
    czl_table_node *ele = list->paras;

    fprintf(fout, "{");
    while (ele)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return 0;
        tmp = *ret;
        if (CZL_FLOAT == tmp.type)
        {
            tmp.type = CZL_INT;
            tmp.val.inum = (czl_long)tmp.val.fnum;
        }
        else if (tmp.type != CZL_INT && tmp.type != CZL_STRING)
            return 0;
        if (CZL_STRING == tmp.type)
            fprintf(fout, "\"");
        czl_print_obj(gp, &tmp, fout);
        if (CZL_STRING == tmp.type)
            fprintf(fout, "\"");
        fprintf(fout, ":");
        if (!(ret=czl_exp_cac(gp, ele->val)))
            return 0;
        czl_print_obj(gp, ret, fout);
        if (ele->next)
            fprintf(fout, ", ");
        ele = ele->next;
    }
    fprintf(fout, "}");

    return 1;
}

char czl_print_obj(czl_gp *gp, const czl_var *obj, FILE *fout)
{
    char mode[8] = "%d";

    switch (obj->type)
    {
    case CZL_INT:
        czl_get_int_mode(mode);
        fprintf(fout, mode, obj->val.inum);
        break;
    case CZL_FLOAT:
        fprintf(fout, "%lf", obj->val.fnum);
        break;
    case CZL_STRING:
        fprintf(fout, "%s", CZL_STR(obj->val.str.Obj)->str);
        break;
    case CZL_INSTANCE:
        czl_print_ins(gp, CZL_INS(obj->val.Obj), fout);
        break;
    case CZL_TABLE:
        czl_print_tab(gp, CZL_TAB(obj->val.Obj), fout);
        break;
    case CZL_ARRAY:
        czl_print_arr(gp, CZL_ARR(obj->val.Obj), fout);
        break;
    case CZL_STACK: case CZL_QUEUE:
        czl_print_sq(gp, CZL_SQ(obj->val.Obj), fout);
        break;
    case CZL_FUN_REF:
        fprintf(fout, "%d", (int)obj->val.fun);
        break;
    case CZL_FILE:
        fprintf(fout, "%d[%d][%d]", (int)CZL_FIL(obj->val.Obj)->fp,
                CZL_FIL(obj->val.Obj)->mode, CZL_FIL(obj->val.Obj)->sign);
        break;
    case CZL_ARRAY_LIST:
        return czl_print_arr_list(gp, CZL_ARR_LIST(obj->val.Obj), fout);
    case CZL_TABLE_LIST:
        return czl_print_tab_list(gp, CZL_TAB_LIST(obj->val.Obj), fout);
    default:
        break;
    }

    return 1;
}

void czl_print_str_hex(czl_string *str, char modify_type, FILE *fout)
{
    char *s = str->str;
    unsigned long i, j = str->len;

    for (i = 0; i < j; i++, s++)
    {
        unsigned char ch = *s;
        char h = ch/16;
        char l = ch - h*16;
        if (h >= 0 && h <= 9)
            h += '0';
        else
            h = h - 10 + 'A';
        if (l >= 0 && l <= 9)
            l += '0';
        else
            l = l - 10 + 'A';
        if ('x' == modify_type && i+1 < j)
            fprintf(fout, "%c%c ", h, l);
        else
            fprintf(fout, "%c%c", h, l);
    }
}

void czl_print_str_bin(czl_string *str, char modify_type, FILE *fout)
{
    char *s = str->str;
    unsigned long i, j = str->len;

    for (i = 0; i < j; i++, s++)
    {
        unsigned char ch = *s;
        unsigned char cnt;
        for (cnt = 0; cnt < 8; cnt++, ch <<= 1)
            fprintf(fout, "%d", (ch&0x80 ? 1:0));
        if ('b' == modify_type && i+1 < j)
            fprintf(fout, " ");
    }
}

char czl_modify_print
(
    czl_gp *gp,
    char *modify,
    char modify_type,
    czl_var *res,
    FILE *fout
)
{
    switch (modify_type)
    {
    case CZL_CHAR: case CZL_INT:
        if (CZL_INT == modify_type)
            czl_get_int_mode(modify);
        if (CZL_INT == res->type)
            fprintf(fout, modify, res->val.inum);
        else
            fprintf(fout, modify, res->val.fnum);
        return 1;
    case CZL_FLOAT:
        if (CZL_INT == res->type)
            fprintf(fout, modify, (double)res->val.inum);
        else
            fprintf(fout, modify, res->val.fnum);
        return 1;
    case 'x': case 'X':
        czl_print_str_hex(CZL_STR(res->val.str.Obj), modify_type, fout);
        return 1;
    case 'b': case 'B':
        czl_print_str_bin(CZL_STR(res->val.str.Obj), modify_type, fout);
        return 1;
    default: //CZL_OBJ_REF
        return czl_print_obj(gp, res, fout);
    }
}

char czl_sys_echo(czl_gp *gp, czl_fun *fun)
{
    FILE *fout;
    czl_var *res;
    czl_para *p;
    char ret = 1;

#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        //多线程控制台工作方式下echo递归会导致死锁
        czl_print_lock();
    #endif
    fout = stdout;
    fun->ret.val.inum = 1;
#else
    if (!gp->log_path)
        return 1;
    if (!(fout=fopen(gp->log_path, "a")))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
    fun->ret.val.inum = 1;
#endif

    for (p = (czl_para*)fun->vars; p; p = p->next)
        if (!(res=czl_exp_cac(gp, p->para)) ||
            !czl_print_obj(gp, res, fout))
        {
            ret = 0;
            break;
        }

    if (ret)
        fprintf(fout, "\n");

#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        czl_print_unlock();
    #endif
#else
    fclose(fout);
#endif
    return ret;
}

char czl_sys_print(czl_gp *gp, czl_fun *fun)
{
	FILE *fout;
	char *s;
    char modify_type;
    char modify[16] = "%";
    czl_var *res;
    czl_str str;
    czl_para *p;
    char ret = 1;

#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        //多线程控制台工作方式下print递归会导致死锁
        czl_print_lock();
    #endif
	fout = stdout;
    fun->ret.val.inum = 1;
#else
    if (!gp->log_path)
        return 1;
    if (!(fout=fopen(gp->log_path, "a")))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
    fun->ret.val.inum = 1;
#endif

    for (p = (czl_para*)fun->vars; p; p = p->next)
    {
        if (!(res=czl_exp_cac(gp, p->para)))
        {
            ret = 0;
            goto CZL_END;
        }
        if (CZL_STRING == res->type)
        {
            str = res->val.str;
            CZL_SRCA1(str); //引用计数加一保证不被释放
            for (s = CZL_STR(str.Obj)->str; *s; s++)
            {
                if ('%' == *s)
                {
                    if ('%' == *(s+1))
                        fputc(*(s++), fout);
                    else
                    {
                        if (!(p=p->next) ||
                            !(res=czl_exp_cac(gp, p->para)) ||
                            !(s=czl_print_modify_check(s+1, modify+1,
                                                       &modify_type,
                                                       res->type)))
                        {
                            ret = 0;
                            CZL_SRCD1(gp, str);
                            goto CZL_END;
                        }
                        if (!czl_modify_print(gp, modify, modify_type, res, fout))
                        {
                            ret = 0;
                            CZL_SRCD1(gp, str);
                            goto CZL_END;
                        }
                    }
                }
                else
                    fputc(*s, fout);
            }
            CZL_SRCD1(gp, str);
        }
        else if (!czl_print_obj(gp, res, fout))
        {
            ret = 0;
            goto CZL_END;
        }
    }

CZL_END:
#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        czl_print_unlock();
    #endif
#else
    fclose(fout);
#endif
    return ret;
}

#ifdef CZL_CONSOLE
char czl_sys_input(czl_gp *gp, czl_fun *fun)
{
    char tmp[1024];
	unsigned long len;

    if (!fgets(tmp, 1024, stdin))
        return 0;
    len = strlen(tmp) - 1;
    tmp[len] = '\0';

    return czl_set_ret_str(gp, &fun->ret, tmp, len);
}
#endif //CZL_CONSOLE
///////////////////////////////////////////////////////////////
unsigned long czl_sizeof_int(czl_long num)
{
    if (num >= -128 && num <= 127)
        return 2;
    else if (num >= -32768 && num <= 32767)
        return 3;
    else if (num >= -2147483648 && num <= 2147483647)
        return 5;
    else
        return 9;
}

void czl_sizeof_ins
(
    czl_gp *gp,
    char flag,
    czl_ins *ins,
    unsigned long *sum
)
{
	czl_var *var;
	unsigned long i, j;

	var = CZL_GIV(ins);
	for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        czl_sizeof_obj(gp, flag, var++, sum);

    for (i = 0, j = ins->pclass->parents_count; i < j; i++)
        czl_sizeof_ins(gp, flag, CZL_INS(ins->parents[i]), sum);
}

void czl_sizeof_tab
(
    czl_gp *gp,
    char flag,
    const czl_table *tab,
    unsigned long *sum
)
{
    czl_tabkv *p;
    for (p = tab->eles_head; p; p = p->next)
    {
        if (CZL_STRING == p->kt)
            *sum += (flag ? CZL_STR(p->key.str.Obj)->len +
                            czl_sizeof_int(CZL_STR(p->key.str.Obj)->len) + 5 :
                            CZL_STR(p->key.str.Obj)->len);
        else
            *sum += (flag ? czl_sizeof_int(p->key.inum)+1 : sizeof(czl_long));
        czl_sizeof_obj(gp, flag, (czl_var*)p, sum);
    }
}

void czl_sizeof_arr
(
    czl_gp *gp,
    char flag,
    const czl_array *arr,
    unsigned long *sum
)
{
    unsigned long i;
    czl_var *var;

    for (i = 0; i < arr->cnt; i++)
    {
        var = arr->vars + i;
        czl_sizeof_obj(gp, flag, var, sum);
    }
}

void czl_sizeof_sq
(
    czl_gp *gp,
    char flag,
    const czl_sq *sq,
    unsigned long *sum
)
{
    czl_glo_var *ele;

    for (ele = sq->eles_head; ele; ele = ele->next)
        czl_sizeof_obj(gp, flag, (czl_var*)ele, sum);
}

char czl_sizeof_arr_list
(
    czl_gp *gp,
    char flag,
    const czl_array_list *list,
    unsigned long *sum
)
{
    czl_var *ret;
    czl_para *para;

    for (para = list->paras; para; para = para->next)
    {
        if (!(ret=czl_exp_cac(gp, para->para)))
            return 0;
        czl_sizeof_obj(gp, flag, ret, sum);
    }

    return 1;
}

char czl_sizeof_tab_list
(
    czl_gp *gp,
    char flag,
    const czl_table_list *list,
    unsigned long *sum
)
{
    czl_var *ret;
    czl_table_node *ele = list->paras;

    while (ele)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return 0;
        switch (ret->type)
        {
        case CZL_STRING:
            *sum += (flag ? CZL_STR(ret->val.str.Obj)->len +
                            czl_sizeof_int(CZL_STR(ret->val.str.Obj)->len) + 5 :
                            CZL_STR(ret->val.str.Obj)->len);
            break;
        case CZL_INT:
            *sum += (flag ? czl_sizeof_int(ret->val.inum)+1 : sizeof(czl_long));
            break;
        case CZL_FLOAT:
            *sum += (flag ? czl_sizeof_int(ret->val.fnum)+1 : sizeof(czl_long));
            break;
        default:
            return 0;
        }
        if (!(ret=czl_exp_cac(gp, ele->val)))
            return 0;
        czl_sizeof_obj(gp, flag, ret, sum);
        ele = ele->next;
    }

    return 1;
}

char czl_sizeof_obj
(
    czl_gp *gp,
    char flag,
    const czl_var *obj,
    unsigned long *sum
)
{
    switch (obj->type)
    {
    case CZL_INT:
        *sum += flag ? czl_sizeof_int(obj->val.inum)+1 : sizeof(czl_long);
        return 1;
    case CZL_FLOAT:
        *sum += flag ? sizeof(double)+1 : sizeof(double);
        return 1;
    case CZL_STRING:
        *sum += flag ? CZL_STR(obj->val.str.Obj)->len +
                       czl_sizeof_int(CZL_STR(obj->val.str.Obj)->len) + 1 :
                       obj->val.str.size;
        return 1;
    case CZL_FUN_REF:
        *sum += flag ? 5 : 4;
        return  1;
    case CZL_FILE:
        *sum += flag ? 7 : sizeof(czl_file);
        return 1;
    case CZL_INSTANCE:
        if (flag)
            *sum += 5;
        czl_sizeof_ins(gp, flag, CZL_INS(obj->val.Obj), sum);
        return 1;
    case CZL_TABLE:
        czl_sizeof_tab(gp, flag, CZL_TAB(obj->val.Obj), sum);
        if (flag)
            *sum += 14;
        return 1;
    case CZL_ARRAY:
        czl_sizeof_arr(gp, flag, CZL_ARR(obj->val.Obj), sum);
        if (flag)
            *sum += 6;
        return 1;
    case CZL_STACK: case CZL_QUEUE:
        czl_sizeof_sq(gp, flag, CZL_SQ(obj->val.Obj), sum);
        if (flag)
            *sum += 2;
        return 1;
    case CZL_ARRAY_LIST:
        if (!czl_sizeof_arr_list(gp, flag, CZL_ARR_LIST(obj->val.Obj), sum))
            return 0;
        if (flag)
            *sum += 6;
        return 1;
    case CZL_TABLE_LIST:
        if (!czl_sizeof_tab_list(gp, flag, CZL_TAB_LIST(obj->val.Obj), sum))
            return 0;
        if (flag)
            *sum += 14;
        return 1;
    default:
        return 1;
    }
}
///////////////////////////////////////////////////////////////
char czl_sys_open(czl_gp *gp, czl_fun *fun)
{
    void **obj = NULL;
    czl_file *file;
    czl_var *path = fun->vars;
    czl_var *mode1 = fun->vars+1;
    czl_var *mode2 = fun->vars+2;
    czl_var *sign = fun->vars+3;

    if (mode2->val.inum != 1 && mode2->val.inum != 2)
        goto CZL_END;

    //wb+ 覆盖， rb+ 追加
    if (1 == mode2->val.inum &&
        strcmp("wb+", CZL_STR(mode1->val.str.Obj)->str) &&
        strcmp("rb+", CZL_STR(mode1->val.str.Obj)->str))
        goto CZL_END;

    if (!(obj=(void**)CZL_FILE_MALLOC(gp)))
        goto CZL_END;

    file = CZL_FIL(obj);
    if (!(file->fp = fopen(CZL_STR(path->val.str.Obj)->str,
                           CZL_STR(mode1->val.str.Obj)->str)))
        goto CZL_END;

    file->rc = 1;
    file->mode = mode2->val.inum;
    file->sign = sign->val.inum;

    fun->ret.type = CZL_FILE;
    fun->ret.val.Obj = obj;
    return 1;

CZL_END:
    fun->ret.type = CZL_INT;
    fun->ret.val.inum = 0;
    CZL_FILE_FREE(gp, obj);
    return 1;
}

char czl_sys_close(czl_gp *gp, czl_fun *fun)
{
    czl_var *var = CZL_GRV(fun->vars);

    if (var->type != CZL_FILE)
        fun->ret.val.inum = 0;
    else
    {
        if (0 == CZL_ORCD1(var->val.Obj))
            czl_file_delete(gp, var->val.Obj);
        var->type = CZL_INT;
        fun->ret.val.inum = 1;
    }
    return 1;
}

char* czl_get_int_buf(czl_long num, char *buf)
{

    if (num >= -128 && num <= 127)
    {
        *buf = 2;
        *((char*)(buf+1)) = num;
    }
    else if (num >= -32768 && num <= 32767)
    {
        *buf = 3;
        *((short*)(buf+1)) = num;
    }
    else if (num >= -2147483648 && num <= 2147483647)
    {
        *buf = 5;
        *((long*)(buf+1)) = num;
    }
    else
    {
        *buf = 9;
        *((czl_long*)(buf+1)) = num;
    }

    return buf+*buf;
}

char* czl_get_str_buf(czl_string *str, char *buf)
{
    if (str->len <= 255)
    {
        *buf = 2;
        *((char*)(buf+1)) = str->len;
    }
    else if (str->len <= 65535)
    {
        *buf = 3;
        *((short*)(buf+1)) = str->len;
    }
    else
    {
        *buf = 5;
        *((long*)(buf+1)) = str->len;
    }

    memcpy(buf+*buf, str->str, str->len);
    return buf+*buf+str->len;
}

char* czl_get_ins_buf(czl_gp *gp, czl_ins *ins, char *buf)
{
	czl_var *var;
	unsigned long i, j;

    var = CZL_GIV(ins);
	for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        buf = czl_get_obj_buf(gp, var++, buf);

    for (i = 0, j = ins->pclass->parents_count; i < j; i++)
        buf = czl_get_ins_buf(gp, CZL_INS(ins->parents[i]), buf);

    return buf;
}

char* czl_get_tab_buf(czl_gp *gp, czl_table *tab, char *buf)
{
	czl_tabkv *p;

    *((unsigned long*)buf) = tab->count;
    buf += 4;
    *((unsigned long*)buf) = tab->key;
    buf += 4;
    *((unsigned long*)buf) = tab->attack_cnt;
    buf += 4;

    for (p = tab->eles_head; p; p = p->next)
    {
        *(buf++) = p->kt;
        if (CZL_STRING == p->kt)
        {
            *((unsigned long*)buf) = p->key.str.size;
            buf = czl_get_str_buf(CZL_STR(p->key.str.Obj), buf+4);
        }
        else
        {
            buf = czl_get_int_buf(p->key.inum, buf);
        }
        buf = czl_get_obj_buf(gp, (czl_var*)p, buf);
    }

    *(buf++) = CZL_NIL;

    return buf;
}

char* czl_get_arr_buf(czl_gp *gp, czl_array *arr, char *buf)
{
	czl_var *var;
    unsigned long i;

    *((unsigned long*)buf) = arr->cnt;
    buf += 4;

    for (i = 0; i < arr->cnt; i++)
    {
        var = arr->vars + i;
        buf = czl_get_obj_buf(gp, var, buf);
    }

    *(buf++) = CZL_NIL;

    return buf;
}

char* czl_get_sq_buf(czl_gp *gp, czl_sq *sq, char *buf)
{
    czl_glo_var *ele;

    if (0 == sq->count)
        return buf-1;

    for (ele = sq->eles_head; ele; ele = ele->next)
        buf = czl_get_obj_buf(gp, (czl_var*)ele, buf);

    *(buf++) = CZL_NIL;

    return buf;
}

char* czl_get_arr_list_buf(czl_gp *gp, czl_array_list *list, char *buf)
{
    czl_var *ret;
    czl_para *para;

    *((unsigned long*)buf) = list->paras_count;
    buf += 4;

    for (para = list->paras; para; para = para->next)
    {
        if (!(ret=czl_exp_cac(gp, para->para)))
            return NULL;
        buf = czl_get_obj_buf(gp, ret, buf);
    }

    *(buf++) = CZL_NIL;

    return buf;
}

char* czl_get_tab_list_buf(czl_gp *gp, czl_table_list *list, char *buf)
{
    unsigned long key;
	unsigned long len;
	czl_var *ret;
    czl_table_node *ele;

    *((unsigned long*)buf) = list->paras_count;
    buf += 4;

    key = czl_hash_key_create((unsigned long)gp->mm_cnt, 0);
    *((unsigned long*)buf) = key;
    buf += 4;

    *((unsigned long*)buf) = 0;
    buf += 4;

    for (ele = list->paras; ele; ele = ele->next)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return NULL;
        *((char*)buf++) = ret->type;
        switch (ret->type)
        {
        case CZL_STRING:
            len = CZL_STR(ret->val.str.Obj)->len;
            *((unsigned long*)buf) =
                    czl_str_hash(CZL_STR(ret->val.str.Obj)->str, len, key, 0);
            buf = czl_get_str_buf(CZL_STR(ret->val.str.Obj), buf+4);
            break;
        case CZL_INT:
            buf = czl_get_int_buf(ret->val.inum, buf);
            break;
        case CZL_FLOAT:
            buf = czl_get_int_buf(ret->val.fnum, buf);
            break;
        default:
            return 0;
        }
        //
        if (!(ret=czl_exp_cac(gp, ele->val)))
            return NULL;
        buf = czl_get_obj_buf(gp, ret, buf);
    }

    *(buf++) = CZL_NIL;

    return buf;
}

char* czl_get_obj_buf
(
    czl_gp *gp,
    const czl_var *obj,
    char *buf
)
{
    *((char*)buf++) = obj->type;

    switch (obj->type)
    {
    case CZL_INT:
        return czl_get_int_buf(obj->val.inum, buf);
    case CZL_FLOAT:
        *((double*)buf) = obj->val.fnum;
        return buf+sizeof(double);
    case CZL_STRING:
        return czl_get_str_buf(CZL_STR(obj->val.str.Obj), buf);
    case CZL_INSTANCE:
        *((unsigned long*)buf) = CZL_INS(obj->val.Obj)->pclass->hash;
        return czl_get_ins_buf(gp, CZL_INS(obj->val.Obj), buf+4);
    case CZL_TABLE:
        return czl_get_tab_buf(gp, CZL_TAB(obj->val.Obj), buf);
    case CZL_ARRAY:
        return czl_get_arr_buf(gp, CZL_ARR(obj->val.Obj), buf);
    case CZL_STACK: case CZL_QUEUE:
        return czl_get_sq_buf(gp, CZL_SQ(obj->val.Obj), buf);
    case CZL_FUN_REF:
        *((unsigned long*)buf) = (unsigned long)obj->val.fun;
        return buf+4;
    case CZL_FILE:
        *((unsigned long*)buf) = (unsigned long)CZL_FIL(obj->val.Obj)->fp;
        *(buf+4) = CZL_FIL(obj->val.Obj)->mode;
        *(buf+5) = CZL_FIL(obj->val.Obj)->sign;
        return buf+6;
    case CZL_ARRAY_LIST:
        *(buf-1) = CZL_ARRAY;
        return czl_get_arr_list_buf(gp, CZL_ARR_LIST(obj->val.Obj), buf);
    case CZL_TABLE_LIST:
        *(buf-1) = CZL_TABLE;
        return czl_get_tab_list_buf(gp, CZL_TAB_LIST(obj->val.Obj), buf);
    default:
        return NULL;
    }
}

char czl_struct_write(czl_gp *gp, FILE *fp, czl_para *p)
{
	czl_var *res;
    unsigned long obj_size;
    unsigned long file_size;
    unsigned long obj_cnt;
    unsigned char header[16];
    char *buf;

    if (!ftell(fp))
    {
        *((unsigned long*)header) = CZL_FILE_CHECK_SUM;
        *((unsigned long*)(header+4)) = file_size = 0;
        *((unsigned long*)(header+8)) = obj_cnt = 0;
        if (!fwrite(header, 12, 1, fp))
            return 0;
    }
    else
    {
        rewind(fp);
        if (!fread(header, 12, 1, fp))
            return 0;
        if (*((unsigned long*)header) != CZL_FILE_CHECK_SUM)
            return 0;
        file_size = *((unsigned long*)(header+4));
        obj_cnt = *((unsigned long*)(header+8));
        if (fseek(fp, 0, SEEK_END))
            return 0;
    }

    while (p)
    {
        obj_size = 4;
        if (!(res=czl_exp_cac(gp, p->para)) ||
            !czl_sizeof_obj(gp, 1, res, &obj_size) ||
            obj_size+file_size >= CZL_FILE_MAX_SZIE ||
            !(buf=(char*)CZL_TMP_MALLOC(gp, obj_size)))
            goto CZL_END;
        *((unsigned long*)buf) = obj_size-4;
        czl_get_obj_buf(gp, res, buf+4);
        if (!fwrite(buf, obj_size, 1, fp))
        {
            CZL_TMP_FREE(gp, buf, obj_size);
            goto CZL_END;
        }
        CZL_TMP_FREE(gp, buf, obj_size);
        file_size += obj_size;
        ++obj_cnt;
        p = p->next;
    }

    *((unsigned long*)(header+4)) = file_size;
    *((unsigned long*)(header+8)) = obj_cnt;
    rewind(fp);
    if (!fwrite(header, 12, 1, fp))
        return 0;

    return 1;

CZL_END:
    return 0;
}

char czl_line_write(czl_gp *gp, FILE *fp, char sign, czl_para *p)
{
    char flag[2];
    char tmp[128];
    char *item;
    czl_var *ret;

    flag[1] = '\0';

    for (; p; p = p->next)
    {
        if (!(ret=czl_exp_cac(gp, p->para)))
            return 0;
        switch (ret->type)
        {
        case CZL_STRING:
            item = CZL_STR(ret->val.str.Obj)->str;
            break;
        case CZL_INT:
            item = tmp;
            czl_itoa(ret->val.inum, tmp);
            break;
        case CZL_FLOAT:
            item = tmp;
            czl_ftoa(ret->val.fnum, tmp, 6);
            break;
        default:
            continue;
        }
        if (EOF == fputs(item, fp))
            return 0;
        flag[0] = p->next ? sign : '\n';
        if (EOF == fputs(flag, fp))
            return 0;
    }

    return 1;
}

char czl_sys_write(czl_gp *gp, czl_fun *fun)
{
    czl_file *f;
    czl_var *file;
	czl_para* paras = (czl_para*)fun->vars;

    if (!(file=czl_exp_cac(gp, paras->para)))
        return 0;

    if (file->type != CZL_FILE)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    f = CZL_FIL(file->val.Obj);
    if (fseek(f->fp, 0, SEEK_END))
        fun->ret.val.inum = 0;
    else if (1 == f->mode)
        fun->ret.val.inum = czl_struct_write(gp, f->fp, paras->next);
    else
        fun->ret.val.inum = czl_line_write(gp, f->fp, f->sign, paras->next);

    return 1;
}

char* czl_analysis_int_buf(char *buf, czl_value *val)
{
    switch (*buf)
    {
    case 2: val->inum = *((char*)(buf+1)); break;
    case 3: val->inum = *((short*)(buf+1)); break;
    case 5: val->inum = *((long*)(buf+1)); break;
    default: val->inum = *((czl_long*)(buf+1)); break;
    }

    return buf+*buf;
}

char* czl_analysis_ins(czl_gp *gp, char *buf, czl_ins *ins)
{
	czl_var *var;
	unsigned long i, j;

    var = CZL_GIV(ins);
    for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        if (!(buf=czl_analysis_ele_buf(gp, buf, var++)))
            return NULL;

    for (i = 0, j = ins->pclass->parents_count; i < j; i++)
        if (!(buf=czl_analysis_ins(gp, buf, CZL_INS(ins->parents[i]))))
            return NULL;

    return buf;
}

char* czl_analysis_ins_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_class *pclass = (czl_class*)czl_sys_hash_find(CZL_STRING, CZL_INT,
                                                      buf, &gp->class_hash);

    if (!pclass || !(obj->val.Obj=czl_instance_fork(gp, pclass, 0)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

    obj->type = CZL_INSTANCE;

    return czl_analysis_ins(gp, buf+4, CZL_INS(obj->val.Obj));
}

char* czl_analysis_tab_buf(czl_gp *gp, char *buf, czl_var *obj)
{
	char kt;
    czl_value tmp;
	czl_table *tab;
    czl_tabkv *ele;
    unsigned long hash;
	unsigned long len;
	unsigned long key;
	unsigned long attack_cnt;

    len = *((unsigned long*)buf);
    buf += 4;
    key = *((unsigned long*)buf);
    buf += 4;
    attack_cnt = *((unsigned long*)buf);
    buf += 4;

    if (!(obj->val.Obj=czl_table_create(gp, key, attack_cnt)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

    tab = CZL_TAB(obj->val.Obj);
    if (!czl_hash_init(gp, tab, len))
    {
        obj->type = CZL_INT;
        czl_table_delete(gp, obj->val.Obj);
        return NULL;
    }

    obj->type = CZL_TABLE;

    while ((kt=*buf++) != CZL_NIL)
    {
        if (CZL_STRING == kt)
        {
            hash = *((unsigned long*)buf);
            buf = czl_analysis_int_buf(buf+4, &tmp);
            if (!(ele=czl_create_str_tabkv(gp, tab, hash, tmp.inum, buf)))
                return NULL;
            buf += tmp.inum;
        }
        else
        {
            buf = czl_analysis_int_buf(buf, &tmp);
            if (!(ele=czl_create_num_tabkv(gp, tab, tmp.inum)))
                return NULL;
        }
        if (!(buf=czl_analysis_ele_buf(gp, buf, (czl_var*)ele)))
            return NULL;
    }

    return buf;
}

char* czl_analysis_arr_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_var *var;
    czl_array *arr;

    if (!(obj->val.Obj=czl_array_create(gp, *((unsigned long*)buf), 0)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

    arr = CZL_ARR(obj->val.Obj);

    buf += 4;
    obj->type = CZL_ARRAY;

    while (*buf != CZL_NIL)
    {
        var = arr->vars + arr->cnt++;
        buf = czl_analysis_ele_buf(gp, buf, var);
        if (!buf)
            return NULL;
    }

    return buf+1;
}

char* czl_analysis_sq_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_sq *sq;

    if (!(obj->val.Obj=czl_sq_create(gp, 0)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

     sq = CZL_SQ(obj->val.Obj);

    while (*buf != CZL_NIL)
    {
        czl_glo_var *ele = czl_sqele_node_create(gp
                                                  #ifdef CZL_MM_MODULE
                                                  , &sq->pool
                                                  #endif
                                                  );
        if (!ele)
            return NULL;
        if (sq->eles_head)
            sq->eles_tail->next = ele;
        else
            sq->eles_head = ele;
        sq->eles_tail = ele;
        sq->count++;
        if (!(buf=czl_analysis_ele_buf(gp, buf, (czl_var*)ele)))
            return NULL;
    }

    return buf+1;
}

char* czl_analysis_ele_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_value len;

    obj->type = *buf++;

    switch (obj->type)
    {
    case CZL_INT:
        return czl_analysis_int_buf(buf, &obj->val);
    case CZL_FLOAT:
        obj->val.fnum = *((double*)buf);
        return buf+sizeof(double);
    case CZL_STRING:
        buf = czl_analysis_int_buf(buf, &len);
        if (!czl_str_create(gp, &obj->val.str, len.inum+1, len.inum, buf))
        {
            obj->type = CZL_INT;
            return 0;
        }
        return buf+len.inum;
    case CZL_FUN_REF:
        obj->val.fun = (czl_fun*)(*((unsigned long*)buf));
        return buf+4;
    case CZL_FILE:
        CZL_FIL(obj->val.Obj)->fp = (FILE*)(*((unsigned long*)buf));
        CZL_FIL(obj->val.Obj)->mode = *(buf+4);
        CZL_FIL(obj->val.Obj)->sign = *(buf+5);
        return buf+6;
    case CZL_INSTANCE:
        return czl_analysis_ins_buf(gp, buf, obj);
    case CZL_TABLE:
        return czl_analysis_tab_buf(gp, buf, obj);
    case CZL_ARRAY:
        return czl_analysis_arr_buf(gp, buf, obj);
    default: //CZL_STACK/CZL_QUEUE
        return czl_analysis_sq_buf(gp, buf, obj);
    }
}

void** czl_analysis_obj_buf(czl_gp *gp, char *buf, unsigned long obj_cnt)
{
    void **obj;
    czl_array *arr;
    const char end = 0xFF;

    if (!(obj=czl_array_create(gp, obj_cnt, 0)))
        return NULL;

    arr = CZL_ARR(obj);

    while (*buf != end)
    {
        if (!(buf=czl_analysis_ele_buf(gp, buf+4, arr->vars+arr->cnt++)))
        {
            czl_array_delete(gp, obj);
            return NULL;
        }
    }

    return obj;
}

char czl_struct_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
    char *buf;
    unsigned long file_size;
    unsigned long obj_cnt;
    unsigned char header[16];

    rewind(fp);
    if (!fread(header, 12, 1, fp))
        return 0;
    if (*((unsigned long*)header) != CZL_FILE_CHECK_SUM)
        return 0;
    file_size = *((unsigned long*)(header+4));
    obj_cnt = *((unsigned long*)(header+8));

    if (!(buf=(char*)CZL_TMP_MALLOC(gp, file_size+1)))
        return 0;
    buf[file_size] = (char)0xFF; //注意不能等于 czl_opr_type_enum 中任何一个

    if (!fread(buf, file_size, 1, fp) ||
        !(ret->val.Obj=czl_analysis_obj_buf(gp, buf, obj_cnt)))
    {
        CZL_TMP_FREE(gp, buf, file_size+1);
        return 0;
    }

    CZL_TMP_FREE(gp, buf, file_size+1);
    ret->type = CZL_ARRAY;
    return 1;
}

char czl_obj_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
    char *buf;
    unsigned long size;
    unsigned char header[16];

    if (!ftell(fp))
    {
        if (!fread(header, 12, 1, fp))
            return 0;
        if (*((unsigned long*)header) != CZL_FILE_CHECK_SUM)
            return 0;
    }

    if (!fread(header, 4, 1, fp))
        return 0;

    size = *((unsigned long*)header);
    if (!(buf=(char*)CZL_TMP_MALLOC(gp, size)))
        return 0;

    if (!fread(buf, size, 1, fp) || !czl_analysis_ele_buf(gp, buf, ret))
    {
        CZL_TMP_FREE(gp, buf, size);
        return 0;
    }

    CZL_TMP_FREE(gp, buf, size);
    return 1;
}

char czl_line_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
    void **obj;
    czl_str str;
    czl_string *s;
    char flag = 1;
    unsigned long len;
    char tmp[1024];

    if (!czl_str_create(gp, &str, 1, 0, NULL))
        return 0;

    do {
        if (!fgets(tmp, 1024, fp))
        {
            CZL_SF(gp, str);
            return 0;
        }
        len = strlen(tmp);
        if ('\n' == tmp[len-1])
        {
            flag = 0;
            tmp[--len] = '\0';
        }
        if (!(obj=CZL_SR(gp, str, len+str.size)))
        {
            CZL_SF(gp, str);
            return 0;
        }
        s = CZL_STR(obj);
        strcpy(s->str + s->len, tmp);
        s->len += len;
        str.size += len;
        str.Obj = obj;
    } while (flag);

    ret->type = CZL_STRING;
    ret->val.str = str;
    return 1;
}

char czl_sys_read(czl_gp *gp, czl_fun *fun)
{
    czl_file *f;
    czl_var *ret;
    czl_var *file = fun->vars;

    if (file->type != CZL_FILE)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    ret = CZL_GRV(fun->vars+1);
    if (!czl_ret_clean(gp, ret))
        return 0;

    f = CZL_FIL(file->val.Obj);
    fun->ret.val.inum = (1 == f->mode ?
                         czl_struct_read(gp, f->fp, ret) :
                         czl_line_read(gp, f->fp, ret));

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_srand(czl_gp *gp, czl_fun *fun)
{
    srand((int)fun->vars->val.inum);
    return 1;
}

char czl_sys_rand(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = rand();
    return 1;
}

char czl_sys_rrand(czl_gp *gp, czl_fun *fun)
{
    int s = (int)fun->vars->val.inum;
    int e = (int)fun->vars[1].val.inum;
    int t;

    if (s > e) { t = s; s = e; e = t; }

    if (s < 0) { t = -s; s = 0; e += t; }
    else t = 0;

    fun->ret.type = CZL_INT;
    fun->ret.val.inum = rand() % (e-s+1) + s;

    if (t != 0) fun->ret.val.inum -= t;

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_int(czl_gp *gp, czl_fun *fun)
{
    switch (fun->vars->type)
    {
    case CZL_FLOAT:
        fun->ret.val.inum = fun->vars->val.fnum;
        break;
    case CZL_STRING:
        if ('\0' == *CZL_STR(fun->vars->val.str.Obj)->str ||
            !czl_get_number_from_str(CZL_STR(fun->vars->val.str.Obj)->str, &fun->ret))
            fun->ret.val.inum = 0;
        if (CZL_FLOAT == fun->ret.type)
        {
            fun->ret.type = CZL_INT;
            fun->ret.val.inum = fun->ret.val.fnum;
        }
        break;
    case CZL_INT:
        fun->ret.val.inum = fun->vars->val.inum;
        break;
    default:
        fun->ret.val.inum = 0;
        break;
    }

    return 1;
}

char czl_sys_float(czl_gp *gp, czl_fun *fun)
{
    switch (fun->vars->type)
    {
    case CZL_INT:
        fun->ret.val.fnum = fun->vars->val.inum;
        break;
    case CZL_STRING:
        if ('\0' == *CZL_STR(fun->vars->val.str.Obj)->str ||
            !czl_get_number_from_str(CZL_STR(fun->vars->val.str.Obj)->str, &fun->ret))
            fun->ret.val.fnum = 0;
        if (CZL_INT == fun->ret.type)
            fun->ret.val.fnum = fun->ret.val.inum;
        break;
    case CZL_FLOAT:
        fun->ret.val.fnum = fun->vars->val.fnum;
        break;
    default:
        fun->ret.val.fnum = 0;
        break;
    }

    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_abs(czl_gp *gp, czl_fun *fun)
{
	if (fun->vars->val.inum >= 0)
        fun->ret.val.inum = fun->vars->val.inum;
	else
        fun->ret.val.inum = -fun->vars->val.inum;
    return 1;
}

char czl_sys_exp(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = exp(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_log(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = log(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_log10(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = log10(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_pow(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = pow(fun->vars->val.fnum, fun->vars[1].val.inum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_sqrt(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = sqrt(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_ceil(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = ceil(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_floor(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = floor(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_sin(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = sin(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_asin(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = asin(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_sinh(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = sinh(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_cos(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = cos(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_acos(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = acos(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_cosh(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = cosh(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_tan(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = tan(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_atan(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = atan(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}

char czl_sys_tanh(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.fnum = tanh(fun->vars->val.fnum);
    fun->ret.type = CZL_FLOAT;
    return 1;
}
///////////////////////////////////////////////////////////////
char czl_get_num_by_char(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else
        return 0;
}

unsigned long czl_itoa(czl_long num, char *dst)
{
	char ch;
	czl_long tmp;
    unsigned long i = 0, j = 0, k, l;

    if (num < 0)
    {
        dst[i++] = '-';
        num = -num;
        j = 1;
    }
    
    do {
        tmp = num;
        dst[i++] = (char)(tmp - (num/=10)*10) + '0';
    } while (num);
    
    dst[i] = '\0';
    l = i;
    k = (i--/2) + j;

    //翻转
    while (j < k)
    {
        ch = dst[j];
        dst[j++] = dst[i];
        dst[i--] = ch;
    }

    return l;
}

unsigned long czl_ftoa(double num, char *dst, int _NumOfDec)
{
    unsigned long i = 0;
    int j, dec, sig;
    char *s = fcvt(num, _NumOfDec, &dec, &sig);

    if (sig)
        dst[i++] = '-';

    for (j = 0; j < dec; j++)
        dst[i++] = s[j];
    dst[i++] = '.';

    while (s[j] != '\0')
        dst[i++] = s[j++];
    dst[i] = '\0';

    return i;
}

czl_long czl_pow(const czl_long b, const czl_long e)
{
    czl_long i;
    czl_long ret = 1;

    for (i = 0; i < e; i++)
        ret *= b;

    return ret;
}

czl_long czl_atoi(char *s, const char radix)
{
    czl_long b; //base
    czl_long e; //exp
    char *h = s;
    char *t;
    char i;

    if ('\0' == *s)
        return 0;

    //是否有exp
    while (*s != '\0' && *s != 'e' && *s != 'E')
        s++;

    //计算exp
    if ('\0' == *s)
        e = 1;
    else
    {
        t = s;
        while (*t != '\0')
            t++;
        e = i = 0;
        for (--t; t != s; t--)
            e += czl_get_num_by_char(*t) * czl_pow(radix, i++);
        e = czl_pow(radix, e);
    }

    //计算base
    s--;
    b = 0;
    for (i = 0; s != h; i++)
    {
        b += czl_get_num_by_char(*s) * czl_pow(radix, i);
        --s;
    }
    b += czl_get_num_by_char(*s) * czl_pow(radix, i);

    return b*e;
}

char* czl_get_dec_number_from_str
(
    char *s,
    czl_var *res,
    char mark
)
{
    char flag_point = 0;
    char flag_e = 0;
    char flag_f = 0;
    char value[128];
    char *tmp = value;

    for (;;)
    {
        if ('.' == *s)
        {
            if (flag_point || flag_e)
                break;
            flag_point = 1;
        }
        else if ('-' == *s)
        {
            if (*(s-1) != 'e' && *(s-1) != 'E')
                break;
            flag_f = 1;
        }
        else if ('e' == *s || 'E' == *s)
        {
            if (flag_e)
                break;
            flag_e = 1;
        }
        else if (*s < '0' || *s > '9')
            break;
        *(tmp++) = *(s++);
        if (tmp - value == 31)
            break;
    }

    if ('-' == *(s-1))
        s--;
    if ('e' == *(s-1) || 'E' == *(s-1))
        s--;

    *tmp = '\0';

    if (flag_point || flag_f)
    {
        res->type = CZL_FLOAT;
        res->val.fnum = mark*atof(value);
    }
    else
    {
        res->type = CZL_INT;
        res->val.inum = mark*czl_atoi(value, 10);
    }

    return s;
}

char* czl_get_hex_number_from_str
(
    char *s,
    czl_var *res,
    char mark
)
{
    char value[128];
    char *tmp = value;

    for (;;)
    {
        if ((*s >= '0' && *s <= '9') ||
                (*s >= 'a' && *s <= 'f') ||
                (*s >= 'A' && *s <= 'F'))
            *(tmp++) = *(s++);
        else
            break;
        if (tmp - value == 31)
            break;
    }

    if (tmp == value)
        return NULL;

    *tmp = '\0';

    res->type = CZL_INT;
    res->val.inum = mark*czl_atoi(value, 16);

    return s;
}

char* czl_get_bin_number_from_str
(
    char *s,
    czl_var *res,
    char mark
)
{
    char value[128];
    char *tmp = value;

    for (;;)
    {
        if (*s >= '0' && *s <= '1')
            *(tmp++) = *(s++);
        else
            break;
        if (tmp - value == 127)
            break;
    }

    if (tmp == value)
        return NULL;

    *tmp = '\0';

    res->type = CZL_INT;
    res->val.inum = mark*czl_atoi(value, 2);

    return s;
}

char* czl_find_number_header_from_str
(
    char *s,
    char *radix,
    char *mark
)
{
    //- . 0~9
    if ('-' == *s)
    {
        *mark = -1;
        s++;
    }
    else
        *mark = 1;

    if ('.' == *s && (*(s+1) >= '0' && *(s+1) <= '9'))
    {
        *radix = 10;
        return s;
    }

    if ('0' == *s)
    {
        if ('x' == *(s+1) || 'X' == *(s+1))
        {
            *radix = 16;
            return s+2;
        }
        else if ('b' == *(s+1) || 'B' == *(s+1))
        {
            *radix = 2;
            return s+2;
        }
        else
        {
            *radix = 10;
            return s;
        }
    }
    else if (*s >= '1' && *s <= '9')
    {
        *radix = 10;
        return s;
    }
    else
        return NULL;
}

char* czl_get_number_from_str(char *s, czl_var *res)
{
    char mark = 1;
    char radix = 0;

    if (!(s=czl_find_number_header_from_str(s, &radix, &mark)))
        return NULL;
    switch (radix)
    {
    case 10:
        return czl_get_dec_number_from_str(s, res, mark);
    case 16:
        return czl_get_hex_number_from_str(s, res, mark);
    case 2:
        return czl_get_bin_number_from_str(s, res, mark);
    default:
        return NULL;
    }
}

char czl_sys_str(czl_gp *gp, czl_fun *fun)
{
    char tmp[128];
    unsigned long len;

    switch (fun->vars->type)
    {
    case CZL_INT:
        len = czl_itoa(fun->vars->val.inum, tmp);
        break;
    default: //CZL_FLOAT
        len = czl_ftoa(fun->vars->val.fnum, tmp, (int)fun->vars[1].val.inum);
        break;
    }

    return czl_set_ret_str(gp, &fun->ret, tmp, len);
}

char czl_sys_split(czl_gp *gp, czl_fun *fun)
{
	char flag;
	char *s, *f;
	unsigned long sl, fl;
	unsigned long i = 0, j, k = 0, len = 0;
    void **obj = NULL;
    czl_array *arr = NULL;
    czl_var *str = fun->vars;
	czl_var *sign = fun->vars + 1;

    if (!str)
        return 0;
    
    s = CZL_STR(str->val.str.Obj)->str;
    f = CZL_STR(sign->val.str.Obj)->str;
    sl = CZL_STR(str->val.str.Obj)->len;
    fl = CZL_STR(sign->val.str.Obj)->len;

    while (i < sl)
    {
        flag = 0;
        do {
            for (j = 0; j < fl; j++) {
                if (s[i] == f[j])
                {
                    if (!flag)
                    {
                        flag = 1;
                        k = i;
                    }
                    i++;
                    break;
                }
            }
        } while (j < fl && i < sl);
        if ((flag && len > 0) || (!flag && i == sl-1))
        {
            if (!flag && i == sl-1)
            {
                k = ++i;
                len++;
            }
            if (!obj)
            {
                if (!(obj=czl_array_create(gp, 1, 0)))
                    return 0;
                arr = CZL_ARR(obj);
            }
            if (++arr->cnt > arr->sum &&
                !czl_array_resize(gp, arr, arr->cnt))
            {
                czl_array_delete(gp, obj);
                return 0;
            }
            if (!czl_str_create(gp, &arr->vars[arr->cnt-1].val.str,
                                len+1, len, s+k-len))
            {
                czl_array_delete(gp, obj);
                return 0;
            }
            arr->vars[arr->cnt-1].type = CZL_STRING;
            len = 0;
        }
        else
        {
            i++;
            len++;
        }
    }

    if (!obj)
        return 1;

    sign = CZL_TMP_REALLOC(gp, arr->vars,
                           arr->cnt*sizeof(czl_var),
                           arr->sum*sizeof(czl_var));
    if (sign)
    {
        arr->vars = sign;
        arr->sum = arr->cnt;
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;
}

char czl_str_toul(czl_gp *gp, czl_fun *fun, char flag)
{
	char d;
    char *ps;
	unsigned long i, j;
    czl_var *str = CZL_GCRV(fun->vars);
	czl_var *begin = fun->vars + 1;
    czl_var *end = fun->vars + 2;
    czl_string *s;

    if (!str)
        return 0;

    s = CZL_STR(str->val.str.Obj);

    if (s->rc > 1)
    {
        czl_str tmp;
        if (!czl_str_create(gp, &tmp, s->len+1, s->len, s->str))
            return 0;
        str->val.str = tmp;
        s = CZL_STR(tmp.Obj);
    }

    if (begin->val.inum < 0 || (unsigned long)begin->val.inum >= s->len)
        return 1;

    i = (unsigned long)begin->val.inum;
    j = end->val.inum < 0 ? s->len-1 : (unsigned long)end->val.inum;
    if (i > j) CZL_INT_SWAP(i, j);
    d = 'a' - 'A';
    ps = s->str + i;
    while (i++ <= j)
    {
        if (flag)
        {
            if (*ps >= 'a' && *ps <= 'z')
                *ps++ -= d;
        }
        else
        {
            if (*ps >= 'A' && *ps <= 'Z')
                *ps++ += d;
        }
    }

    return 1;
}

char czl_sys_upper(czl_gp *gp, czl_fun *fun)
{
    return czl_str_toul(gp, fun, 1);
}

char czl_sys_lower(czl_gp *gp, czl_fun *fun)
{
    return czl_str_toul(gp, fun, 0);
}
///////////////////////////////////////////////////////////////
char czl_sys_abort(czl_gp *gp, czl_fun *fun)
{
    gp->exit_code = CZL_EXIT_ABORT;
    return 0;
}

char czl_sys_assert(czl_gp *gp, czl_fun *fun)
{
    if (CZL_EIT(fun->vars))
        return 1;

    gp->exit_code = CZL_EXIT_ASSERT;
    return 0;
}

char czl_sys_errline(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->error_line;
    return 1;
}

char czl_sys_errfile(czl_gp *gp, czl_fun *fun)
{
    return (gp->error_file ?
            czl_set_ret_str(gp, &fun->ret, gp->error_file, strlen(gp->error_file)) : 1);
}

char czl_sys_errcode(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->exceptionCode;
    return 1;
}

char czl_sys_setfun(czl_gp *gp, czl_fun *fun)
{
    if (fun->vars->val.fun->enter_vars_count)
        return 1;

    if (0 == fun->vars[1].val.inum)
    {
        unsigned long i;
        for (i = 0; i < CZL_EXCEPTION_CODE_NUM; ++i)
            gp->exceptionFuns[i] = fun->vars->val.fun;
    }
    else if (fun->vars[1].val.inum >= 1 &&
             fun->vars[1].val.inum <= CZL_EXCEPTION_CODE_NUM)
        gp->exceptionFuns[fun->vars[1].val.inum-1] = fun->vars->val.fun;

    return 1;
}

#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_sleep(czl_gp *gp, czl_fun* fun)
{
#ifdef CZL_SYSTEM_LINUX
    usleep(fun->vars->val.inum*1000);
#elif defined CZL_SYSTEM_WINDOWS
    Sleep(fun->vars->val.inum);
#endif
    return 1;
}

char czl_sys_clock(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = CZL_CLOCK - gp->runtime;
    if (fun->vars->val.inum)
        fun->ret.type = CZL_INT;
    else
    {
        fun->ret.val.fnum = fun->ret.val.inum / 1000.0;
        fun->ret.type = CZL_FLOAT;
    }
    return 1;
}
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)

char czl_sys_ltime(czl_gp *gp, czl_fun *fun)
{
    char *modify = CZL_STR(fun->vars->val.str.Obj)->str;
    time_t now;
    struct tm *tm_now;
    char time_str[128];
    unsigned long len = 0;

    time(&now);
    tm_now = localtime(&now);

    //Y-M-D h:m:s
    while (*modify && len < 128)
    {
        switch (*modify)
        {
        case 'Y':
            len += czl_itoa(tm_now->tm_year+1900, time_str+len);
            break;
        case 'M':
            len += czl_itoa(tm_now->tm_mon+1, time_str+len);
            break;
        case 'D':
            len += czl_itoa(tm_now->tm_mday, time_str+len);
            break;
        case 'h':
            len += czl_itoa(tm_now->tm_hour, time_str+len);
            break;
        case 'm':
            len += czl_itoa(tm_now->tm_min, time_str+len);
            break;
        case 's':
            len += czl_itoa(tm_now->tm_sec, time_str+len);
            break;
        default:
            if ('\\' == *modify &&
                ('Y' == *(modify+1) || 'M' == *(modify+1) || 'D' == *(modify+1) ||
                 'h' == *(modify+1) || 'm' == *(modify+1) || 's' == *(modify+1)))
                ++modify;
            time_str[len++] = *modify;
            break;
        }
        modify++;
    }

    time_str[len] = '\0';

    return czl_set_ret_str(gp, &fun->ret, time_str, len);
}

char czl_sys_usemem(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->mm_cnt;
    return 1;
}

char czl_sys_maxmem(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->mm_max;
    return 1;
}

char czl_sys_setmem(czl_gp *gp, czl_fun *fun)
{
    czl_ulong limit = (fun->vars->val.inum > CZL_MM_3GB ?
                       CZL_MM_3GB : fun->vars->val.inum);

    if (CZL_MM_4GB == gp->mm_limit)
        gp->mm_limit_backup = limit;
    else
        gp->mm_limit = limit;

    return 1;
}

#ifdef CZL_MM_MODULE

#ifdef CZL_MM_CACHE
char czl_sys_setcache(czl_gp *gp, czl_fun *fun)
{
    gp->mm_cache_size = fun->vars->val.inum;
    return 1;
}
#endif //#ifdef CZL_MM_CACHE

char czl_sys_setrank(czl_gp *gp, czl_fun *fun)
{
    if (fun->vars->val.inum >= 0 && gp->mmp_rank <= CZL_MM_SP_HEAP_NUM_MAX)
        gp->mmp_rank = fun->vars->val.inum;
    return 1;
}

char czl_sys_setsa(czl_gp *gp, czl_fun *fun)
{
    gp->mmp_selfAdapt = fun->vars->val.inum;
    return 1;
}

char czl_sys_setgc(czl_gp *gp, czl_fun *fun)
{
    gp->mmp_gc_size = fun->vars->val.inum;
    return 1;
}

char czl_sys_gc(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_mm_gc(gp);
    return 1;
}
#endif //#ifdef CZL_MM_MODULE

//被调脚本与主脚本共享内存空间
void czl_gp_copy(czl_gp *a, czl_gp *b)
{
#ifdef CZL_MM_MODULE
	unsigned long i;
#ifdef CZL_MM_CACHE
    a->mm_cache_size = b->mm_cache_size;
    a->mm_cache = b->mm_cache;
#endif //#ifdef CZL_MM_CACHE
    for (i = 0; i < CZL_MM_SP_RANGE; ++i)
    {
        a->mmp_tmp[i] = b->mmp_tmp[i];
        a->mmp_rt[i] = b->mmp_rt[i];
        a->mmp_stack[i] = b->mmp_stack[i];
        a->mmp_str[i] = b->mmp_str[i];
    }
    a->mmp_obj = b->mmp_obj;
    a->mmp_tab = b->mmp_tab;
    a->mmp_arr = b->mmp_arr;
    a->mmp_sq = b->mmp_sq;
    a->mmp_file = b->mmp_file;
    a->mmp_ref = b->mmp_ref;
    a->mmp_rank = b->mmp_rank;
    a->mmp_selfAdapt = b->mmp_selfAdapt;
    a->mmp_gc_size = b->mmp_gc_size;
#endif //#ifdef CZL_MM_MODULE
    a->mm_limit = b->mm_limit;
    a->mm_cnt = b->mm_cnt;
    a->mm_max = b->mm_max;
#ifndef CZL_CONSOLE
    a->table = b->table;
    a->log_path = b->log_path;
    a->pclass = b->pclass;
#endif //#ifndef CZL_CONSOLE
    a->ch_head = b->ch_head;
    a->fun_deep = b->fun_deep;
}

char czl_sys_runshl(czl_gp *gp, czl_fun *fun)
{
	czl_gp new_gp;
    czl_var *path = fun->vars;
    czl_var *ret = CZL_GRV(fun->vars+1);
    if (!czl_ret_clean(gp, ret))
        return 0;

    fun->ret.val.inum = 0;

    memset(&new_gp, 0, sizeof(czl_gp));
    if (!(new_gp.tmp=(czl_analysis_gp*)malloc(sizeof(czl_analysis_gp))))
        return 1;
    memset(new_gp.tmp, 0, sizeof(czl_analysis_gp));
    new_gp.enter_var = fun->vars[2];
    czl_gp_copy(&new_gp, gp);

    if (!czl_global_paras_init(&new_gp))
    {
        czl_init_free(&new_gp, 0);
        czl_gp_copy(gp, &new_gp);
        return 1;
    }

    if (czl_exec_shell(&new_gp, CZL_STR(path->val.str.Obj)->str, 1))
    {
        ret->type = new_gp.cur_fun->ret.type;
        ret->val = new_gp.cur_fun->ret.val;
        new_gp.cur_fun->ret.type = CZL_INT;
        fun->ret.val.inum = 1;
    }
    czl_shell_free(&new_gp);
    czl_gp_copy(gp, &new_gp);

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_hcac(czl_gp *gp, czl_fun *fun)
{
    czl_table *tab = CZL_TAB(fun->vars->val.Obj);
    fun->ret.val.inum = tab->attack_cnt;
    return 1;
}

char czl_sys_hdod(czl_gp *gp, czl_fun *fun)
{
	czl_tabkv *p;
	unsigned long i, j, sum;
	unsigned long s = (unsigned long)fun->vars[1].val.inum;
    unsigned long e = (unsigned long)fun->vars[2].val.inum;
    czl_table *tab = CZL_TAB(fun->vars->val.Obj);

	sum = 0;
    j = tab->size;
    for (i = 0; i < j; i++)
    {
        unsigned long cnt = 0;
        for (p = tab->buckets[i]; p; p = p->clsNext)
            ++cnt;
        if (cnt >= s && cnt <= e)
            sum += cnt;
    }

    fun->ret.type = CZL_FLOAT;
    fun->ret.val.fnum = (double)sum/tab->count;

    return 1;
}

void czl_get_obj_type(unsigned char type, const czl_var *obj, char *ret)
{
    switch (type)
    {
    case CZL_NIL: strcpy(ret, "var"); break;
    case CZL_INT: strcpy(ret, "int"); break;
    case CZL_FLOAT: strcpy(ret, "float"); break;
    case CZL_STRING: strcpy(ret, "string"); break;
    case CZL_FILE: strcpy(ret, "file"); break;
    case CZL_FUN_REF: strcpy(ret, "fun"); break;
    case CZL_TABLE: strcpy(ret, "table"); break;
    case CZL_ARRAY: strcpy(ret, "array"); break;
    case CZL_STACK: strcpy(ret, "stack"); break;
    case CZL_QUEUE: strcpy(ret, "queue"); break;
    default: //CZL_INSTANCE
        strcpy(ret, CZL_INS(obj->val.Obj)->pclass->name);
        break;
    }
}

char czl_sys_tp(czl_gp *gp, czl_fun *fun)
{
    char type[8];
    czl_get_obj_type(fun->vars->type, fun->vars, type);
    return czl_set_ret_str(gp, &fun->ret, type, strlen(type));
}

char czl_sys_ot(czl_gp *gp, czl_fun *fun)
{
    char ot[8];
    czl_var *var = CZL_GRV(fun->vars);
    czl_get_obj_type(var->ot, var, ot);
    return czl_set_ret_str(gp, &fun->ret, ot, strlen(ot));
}

char czl_sys_sz(czl_gp *gp, czl_fun *fun)
{
    unsigned long sum = 0;

    fun->ret.val.inum = 0;

    if (!czl_sizeof_obj(gp, 0, fun->vars, &sum))
        return 0;

    fun->ret.val.inum = sum;

    return 1;
}

char czl_sys_rc(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = CZL_GRV(fun->vars);

    switch (obj->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FUN_REF:
        fun->ret.val.inum = 1;
        break;
    case CZL_STRING:
        fun->ret.val.inum = CZL_STR(obj->val.str.Obj)->rc;
        break;
    default:
        fun->ret.val.inum = CZL_INS(obj->val.Obj)->rc;
        break;
    }

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_push(czl_gp *gp, czl_fun *fun)
{
	czl_var *data;
    czl_sq *stack;
	czl_glo_var *ele;
    czl_var *ref = CZL_GCRV(fun->vars);

    if (!ref)
        return 0;

    stack = CZL_SQ(ref->val.Obj);
    if (stack->rc > 1)
    {
        if (!czl_sq_fork(gp, stack, &ref->val, 1))
            return 0;
        stack = CZL_SQ(ref->val.Obj);
    }

    if (!(ele=czl_sqele_node_create(gp
                                  #ifdef CZL_MM_MODULE
                                  , &stack->pool
                                  #endif
                                  )))
        return 0;
    ele->next = stack->eles_head;
    stack->eles_head = ele;
    stack->count++;

    data = fun->vars + 1;
    ele->type = data->type;
    ele->val = data->val;
    data->type = CZL_INT;

    return 1;
}

char czl_stapop_queout(czl_gp *gp, czl_fun *fun)
{
	czl_glo_var *p;
    czl_var *ref = CZL_GRV(fun->vars);
    czl_sq *sq = CZL_SQ(ref->val.Obj);
    if (sq->rc > 1)
    {
        if (!czl_sq_fork(gp, sq, &ref->val, 1))
            return 0;
        sq = CZL_SQ(ref->val.Obj);
    }

    if (!sq->count)
    {
        fun->ret.type = CZL_INT;
        fun->ret.val.inum = 0;
        return 1;
    }

    p = sq->eles_head;
    if (CZL_OBJ_IS_LOCK(p))
        return 0;
    if (p == sq->foreach_inx)
        sq->foreach_inx = p->next;

    sq->eles_head = p->next;

    if (CZL_VAR_EXIST_REF(p))
        czl_ref_obj_delete(gp, (czl_var*)p);

    fun->ret.type = p->type;
    fun->ret.val = p->val;

    czl_free(gp, p, sizeof(czl_glo_var)
             #ifdef CZL_MM_MODULE
             , &sq->pool
             #endif
             );

    --sq->count;
    return 1;
}

char czl_sys_pop(czl_gp *gp, czl_fun *fun)
{
    return czl_stapop_queout(gp, fun);
}

char czl_sys_in(czl_gp *gp, czl_fun *fun)
{
	czl_var *data;
    czl_sq *queue;
	czl_glo_var *ele;
    czl_var *ref = CZL_GCRV(fun->vars);

    if (!ref)
        return 0;

    queue = CZL_SQ(ref->val.Obj);
    if (queue->rc > 1)
    {
        if (!czl_sq_fork(gp, queue, &ref->val, 1))
            return 0;
        queue = CZL_SQ(ref->val.Obj);
    }

    if (!(ele=czl_sqele_node_create(gp
                                  #ifdef CZL_MM_MODULE
                                  , &queue->pool
                                  #endif
                                  )))
        return 0;
    if (queue->eles_head)
        queue->eles_tail->next = ele;
    else
        queue->eles_head = ele;
    queue->eles_tail = ele;
    queue->count++;

    data = fun->vars + 1;
    ele->type = data->type;
    ele->val = data->val;
    data->type = CZL_INT;

    return 1;
}

char czl_sys_out(czl_gp *gp, czl_fun *fun)
{
    return czl_stapop_queout(gp, fun);
}

char czl_sys_ins(czl_gp *gp, czl_fun *fun)
{
    czl_table *tab;
    czl_tabkv *p, *q;
    czl_var *ref = CZL_GCRV(fun->vars);

    if (!ref)
        return 0;

    tab = CZL_TAB(ref->val.Obj);
    if (tab->rc > 1)
    {
        if (!czl_table_fork(gp, tab, &ref->val, 1))
            return 0;
        tab = CZL_TAB(ref->val.Obj);
    }

    if (!(p=czl_find_tabkv(tab, fun->vars+1)) ||
        !(q=czl_create_tabkv(gp, tab, fun->vars+2, p)))
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    czl_val_del(gp, (czl_var*)q);
    q->type = fun->vars[3].type;
    q->val = fun->vars[3].val;
    fun->vars[3].type = CZL_INT;

    fun->ret.val.inum = 1;

    return 1;
}
///////////////////////////////////////////////////////////////
void** czl_coroutine_create(czl_gp *gp, czl_fun *fun, unsigned char type)
{
    void **obj;
	czl_coroutine *p;
	unsigned long cnt;

    if (!fun || !fun->yeild_flag)
        return 0;

    if (!(obj=(void**)CZL_COR_MALLOC(gp)))
        return NULL;

    p = CZL_COR(obj);

    if (!czl_sys_hash_insert(gp, CZL_INT, obj, &gp->coroutines_hash))
    {
        CZL_COR_FREE(gp, obj);
        return NULL;
    }

	cnt = fun->dynamic_vars_cnt + fun->foreach_cnt;
    if (0 == cnt)
        p->vars = NULL;
    else if (!(p->vars=(czl_var*)CZL_STACK_CALLOC(gp, cnt, sizeof(czl_var))))
    {
        CZL_COR_FREE(gp, obj);
        czl_sys_hash_delete(gp, (unsigned long)obj, &gp->coroutines_hash);
        return NULL;
    }
    czl_vars_init(p->vars, fun->vars, fun->dynamic_vars_cnt);

    p->type = type;
    p->fun = fun;
    p->pc = NULL;

    p->last = NULL;
    p->next = gp->coroutines_head;
    if (gp->coroutines_head)
        CZL_COR(gp->coroutines_head)->last = obj;
    gp->coroutines_head = obj;

    return obj;
}

char czl_sys_coroutine(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum =
            (unsigned long)czl_coroutine_create(gp, fun->vars->val.fun,
                                                fun->vars[1].val.inum);
    return 1;
}

char czl_sys_resume(czl_gp *gp, czl_fun *fun)
{
    void **obj;
	unsigned long id;
	czl_coroutine *c;
    czl_para *p = (czl_para*)fun->vars;
    czl_var *ret = czl_exp_cac(gp, p->para);
    if (!ret)
        return 0;

    fun->ret.val.inum = 0;

    if (ret->type != CZL_INT)
        return 1;
    id = (unsigned long)ret->val.inum;
    if (!(obj=czl_sys_hash_find(CZL_INT, CZL_NIL, (char*)id, &gp->coroutines_hash)))
        return 1;

    c = CZL_COR(obj);

    if ((unsigned long)c->fun->enter_vars_count < fun->paras_cnt-1)
        return 1;

    if (!(ret=czl_coroutine_run(gp, p->next, fun->paras_cnt-1, obj)))
        return 0;

    fun->ret.type = ret->type;
    fun->ret.val = ret->val;
    ret->type = CZL_INT;

    return gp->yeild_end ? 1 : 2;
}

char czl_sys_corsta(czl_gp *gp, czl_fun *fun)
{
    unsigned long id = (unsigned long)fun->vars->val.inum;
    if (czl_sys_hash_find(CZL_INT, CZL_NIL, (char*)id, &gp->coroutines_hash))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    return 1;
}

char czl_sys_reset(czl_gp *gp, czl_fun *fun)
{
    if (CZL_FUN_REF == fun->vars->type)
    {
        czl_fun *f = fun->vars->val.fun;
        if (!f || !f->yeild_flag || CZL_IN_BUSY == f->state)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        f->pc = NULL;
        czl_coroutine_paras_reset(gp, (czl_var*)f->backup_vars, f->dynamic_vars_cnt);
    }
    else if (CZL_INT == fun->vars->type)
    {
        unsigned long id = (unsigned long)fun->vars->val.inum;
        void **obj = czl_sys_hash_find(CZL_INT, CZL_NIL,
                                       (char*)id, &gp->coroutines_hash);
        czl_coroutine *c = (obj ? CZL_COR(obj) : NULL);
        if (!c || CZL_IN_BUSY == c->fun->state)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        c->pc = NULL;
        czl_coroutine_paras_reset(gp, c->vars, c->fun->dynamic_vars_cnt);
    }
    else
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    fun->ret.val.inum = 1;
    return 1;
}

char czl_sys_kill(czl_gp *gp, czl_fun *fun)
{
    unsigned long id = (unsigned long)fun->vars->val.inum;
    void **obj = czl_sys_hash_find(CZL_INT, CZL_NIL, (char*)id, &gp->coroutines_hash);

    if (obj)
    {
        czl_coroutine *c = CZL_COR(obj);
        if (CZL_IN_BUSY == c->fun->state)
            fun->ret.val.inum = 0;
        else
        {
            fun->ret.val.inum = 1;
            czl_coroutine_delete(gp, (czl_var*)c->vars, obj);
        }
    }
    else
    {
    #ifdef CZL_MULT_THREAD
        czl_thread *t = (czl_thread*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                                       (char*)id, &gp->threads_hash);
        if (!t)
            fun->ret.val.inum = 0;
        else
        {
            fun->ret.val.inum = 1;
            t->pipe->alive = 0;
            t->pipe->kill = 1;
            czl_event_send(&t->pipe->notify_event);
            t->pipe = NULL;
            extern void czl_thread_delete(czl_gp*, czl_thread*);
            czl_thread_delete(gp, t);
        }
    #else
        fun->ret.val.inum = 0;
    #endif //#ifdef CZL_MULT_THREAD
    }

    return 1;
}
///////////////////////////////////////////////////////////////
#ifdef CZL_MULT_THREAD
void czl_thread_lock
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION *lock
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t *lock
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    EnterCriticalSection(lock);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_lock(lock);
#endif
}

void czl_thread_unlock
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION *lock
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t *lock
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    LeaveCriticalSection(lock);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_unlock(lock);
#endif
}

void czl_event_send
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE *event
#elif defined CZL_SYSTEM_LINUX
    sem_t *event
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    SetEvent(*event);
#elif defined CZL_SYSTEM_LINUX
    int cnt;
    sem_getvalue(event, &cnt);
    if (!cnt)
        sem_post(event);
#endif
}

void czl_event_wait
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE *event
#elif defined CZL_SYSTEM_LINUX
    sem_t *event
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    WaitForSingleObject(*event, INFINITE);
#elif defined CZL_SYSTEM_LINUX
    sem_wait(event);
#endif
}

void czl_lock_init
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION *lock
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t *lock
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    InitializeCriticalSection(lock);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_init(lock, NULL);
#endif
}

char czl_event_init
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE *event
#elif defined CZL_SYSTEM_LINUX
    sem_t *event
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    if ((*event=CreateEvent(NULL, FALSE, FALSE, NULL)))
        return 1;
    else
        return 0;
#elif defined CZL_SYSTEM_LINUX
    if (sem_init(event, 0, 0))
        return 0;
    else
        return 1;
#endif
}

void czl_lock_destroy
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION *lock
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t *lock
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    DeleteCriticalSection(lock);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_destroy(lock);
#endif
}

void czl_event_destroy
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE *event
#elif defined CZL_SYSTEM_LINUX
    sem_t *event
#endif
)
{
#ifdef CZL_SYSTEM_WINDOWS
    CloseHandle(*event);
#elif defined CZL_SYSTEM_LINUX
    sem_destroy(event);
#endif
}

void czl_pipe_buf_delete(czl_pipe_buf *p)
{
    czl_pipe_buf *q;
    while (p)
    {
        q = p->next;
        free(p);
        p = q;
    }
}

char czl_report_buf_create
(
    czl_gp *gp,
    czl_thread_pipe *pipe,
    czl_var *obj
)
{
    czl_pipe_buf *p;
    unsigned long obj_size = 0;

    if (!czl_sizeof_obj(gp, 1, obj, &obj_size) || !(p=malloc(4+obj_size)))
        return 0;

    czl_thread_lock(&pipe->report_lock); //lock

    p->next = NULL;
    if (NULL == pipe->rb_head)
        pipe->rb_head = p;
    else
        pipe->rb_tail->next = p;
    pipe->rb_tail = p;

    ++pipe->rb_cnt;
    czl_get_obj_buf(gp, obj, p->buf);

    czl_thread_unlock(&pipe->report_lock); //unlock
    return 1;
}

char czl_notify_buf_create
(
    czl_gp *gp,
    czl_thread_pipe *pipe,
    czl_var *obj
)
{
    czl_pipe_buf *p;
    unsigned long obj_size = 0;

    if (!czl_sizeof_obj(gp, 1, obj, &obj_size) || !(p=malloc(4+obj_size)))
        return 0;

    czl_thread_lock(&pipe->notify_lock); //lock

    p->next = NULL;
    if (NULL == pipe->nb_head)
        pipe->nb_head = p;
    else
        pipe->nb_tail->next = p;
    pipe->nb_tail = p;

    ++pipe->nb_cnt;
    czl_get_obj_buf(gp, obj, p->buf);

    czl_thread_unlock(&pipe->notify_lock); //unlock
    return 1;
}

char czl_thread_para_create
(
    czl_gp *gp,
    czl_thread_pipe *pipe,
    czl_var *obj
)
{
    unsigned long obj_size = 0;

    if (!czl_sizeof_obj(gp, 1, obj, &obj_size) || !(pipe->para=malloc(obj_size)))
        return 0;

    czl_get_obj_buf(gp, obj, pipe->para);
    return 1;
}

char czl_report_buf_get
(
    czl_gp *gp,
    czl_var *res,
    czl_thread_pipe *pipe
)
{
    void **obj;
    czl_array *arr;
    czl_pipe_buf *p;

    if (!(obj=czl_array_create(gp, pipe->rb_cnt, 0)))
        return 0;

    arr = CZL_ARR(obj);

    for (p = pipe->rb_head; p; p = p->next)
        if (!czl_analysis_ele_buf(gp, p->buf, arr->vars+arr->cnt++))
        {
            czl_array_delete(gp, obj);
            return 0;
        }

    czl_pipe_buf_delete(pipe->rb_head);
    pipe->rb_head = NULL;
    pipe->rb_cnt = 0;

    res->type = CZL_ARRAY;
    res->val.Obj = obj;
    return 1;
}

char czl_notify_buf_get
(
    czl_gp *gp,
    czl_var *res,
    czl_thread_pipe *pipe
)
{
    void **obj;
    czl_array *arr;
    czl_pipe_buf *p;

    if (!(obj=czl_array_create(gp, pipe->nb_cnt, 0)))
        return 0;

    arr = CZL_ARR(obj);

    for (p = pipe->nb_head; p; p = p->next)
        if (!czl_analysis_ele_buf(gp, p->buf, arr->vars+arr->cnt++))
        {
            czl_array_delete(gp, obj);
            return 0;
        }

    czl_pipe_buf_delete(pipe->nb_head);
    pipe->nb_head = NULL;
    pipe->nb_cnt = 0;

    res->type = CZL_ARRAY;
    res->val.Obj = obj;
    return 1;
}

char czl_thread_para_get
(
    czl_gp *gp,
    czl_var *res,
    czl_thread_pipe *pipe
)
{
    if (!czl_analysis_ele_buf(gp, pipe->para, res))
        return 0;

    free(pipe->para);
    pipe->para = NULL;
    return 1;
}

void czl_thread_pipe_delete(czl_thread_pipe *pipe)
{
    free(pipe->para);
    czl_pipe_buf_delete(pipe->rb_head);
    czl_pipe_buf_delete(pipe->nb_head);
    czl_lock_destroy(&pipe->notify_lock);
    czl_lock_destroy(&pipe->report_lock);
    czl_event_destroy(&pipe->notify_event);
    czl_event_destroy(&pipe->report_event);
    free(pipe);
}

void czl_thread_delete(czl_gp *gp, czl_thread *p)
{
#ifdef CZL_SYSTEM_WINDOWS
    CloseHandle(p->id);
#endif
    if (p->pipe)
        czl_thread_pipe_delete(p->pipe);

    czl_sys_hash_delete(gp, (unsigned long)p, &gp->threads_hash);

    if (p->last)
        p->last->next = p->next;
    else
        gp->threads_head = p->next;

    if (p->next)
        p->next->last = p->last;

    CZL_THREAD_FREE(gp, p);
}

czl_thread* czl_thread_create(czl_gp *gp)
{
    czl_thread *p = (czl_thread*)CZL_THREAD_MALLOC(gp);
    if (!p)
        return NULL;

    if (!czl_sys_hash_insert(gp, CZL_INT, p, &gp->threads_hash))
    {
        CZL_THREAD_FREE(gp, p);
        return NULL;
    }

#ifdef CZL_SYSTEM_WINDOWS
    p->id = NULL;
#endif
    p->pipe = NULL;

    p->last = NULL;
    p->next = gp->threads_head;
    if (gp->threads_head)
        gp->threads_head->last = p;
    gp->threads_head = p;

    return p;
}

czl_thread_pipe* czl_thread_pipe_create(czl_gp *gp, czl_var *obj)
{
    czl_thread_pipe *pipe = (czl_thread_pipe*)malloc(sizeof(czl_thread_pipe));
    if (!pipe)
        return NULL;

    memset(pipe, 0, sizeof(czl_thread_pipe));
    czl_lock_init(&pipe->notify_lock);

    if (!czl_thread_para_create(gp, pipe, obj))
        goto CZL_END;

    if (!czl_event_init(&pipe->notify_event) ||
        !czl_event_init(&pipe->report_event))
    {
        free(pipe->para);
        goto CZL_END;
    }

    czl_lock_init(&pipe->report_lock);
    pipe->alive = 1;

    return pipe;

CZL_END:
    czl_lock_destroy(&pipe->notify_lock);
    free(pipe);
    return NULL;
}

#ifdef CZL_SYSTEM_WINDOWS
DWORD WINAPI
#elif defined CZL_SYSTEM_LINUX
void*
#endif
czl_run_thread(void *argv)
{
	czl_gp *gp;
    czl_thread_pipe *pipe = (czl_thread_pipe*)((void**)argv)[0];
    char *shell_path = (char*)((void**)argv)[1];
#ifndef CZL_CONSOLE
    char *log_path = (char*)((void**)argv)[2];
#endif
    free(argv);

    if (!(gp=(czl_gp*)malloc(sizeof(czl_gp))))
        goto CZL_END;
    memset(gp, 0, sizeof(czl_gp));

    gp->thread_pipe = pipe;
#ifndef CZL_CONSOLE
    if (log_path)
    {
        gp->log_path = log_path;
        log_path = NULL;
    }
#endif
    if (!czl_sys_init(gp))
    {
        czl_event_send(&gp->thread_pipe->report_event);
        czl_val_del(gp, &gp->enter_var);
        czl_init_free(gp, 1);
        free(gp);
        goto CZL_END;
    }

    czl_exec_shell(gp, shell_path, 0);
    czl_memory_free(gp);
    free(gp);

CZL_END:
    free(shell_path);
#ifndef CZL_CONSOLE
    free(log_path);
#endif
    czl_event_send(&pipe->report_event); //防止主线程阻塞等待已经退出的子线程
    if (pipe->alive)
        pipe->alive = 0;
    else
        czl_thread_pipe_delete(pipe);

    return 0;
}

char czl_sys_thread(czl_gp *gp, czl_fun *fun)
{
	czl_thread *p;
	char *shell_path = NULL;
#ifndef CZL_CONSOLE
    char *log_path = NULL;
#endif //#ifndef CZL_CONSOLE
    void **argv = NULL;
    FILE *out = fopen(CZL_STR(fun->vars->val.str.Obj)->str, "r");

    fun->ret.val.inum = 0;

    if (!out)
        return 1;

    if (!(p=czl_thread_create(gp)))
        return 1;

    if (!(p->pipe=czl_thread_pipe_create(gp, fun->vars+1)))
        goto CZL_END;

    if (!(argv=(void**)calloc(3, sizeof(void*))))
        goto CZL_END;

    argv[0] = p->pipe;

    if (!(shell_path=(char*)malloc(CZL_STR(fun->vars->val.str.Obj)->len+1)))
        goto CZL_END;
    memcpy(shell_path,
           CZL_STR(fun->vars->val.str.Obj)->str,
           CZL_STR(fun->vars->val.str.Obj)->len);
    shell_path[CZL_STR(fun->vars->val.str.Obj)->len] = '\0';
    argv[1] = shell_path;

#ifndef CZL_CONSOLE
    if (CZL_STR(fun->vars[2].val.str.Obj)->len)
    {
        if (!(log_path=(char*)malloc(CZL_STR(fun->vars[2].val.str.Obj)->len+1)))
            goto CZL_END;
        memcpy(log_path,
               CZL_STR(fun->vars[2].val.str.Obj)->str,
               CZL_STR(fun->vars[2].val.str.Obj)->len);
        log_path[CZL_STR(fun->vars[2].val.str.Obj)->len] = '\0';
    }
    argv[2] = log_path;
#endif //#ifndef CZL_CONSOLE

#ifdef CZL_SYSTEM_WINDOWS
    if (!(p->id=CreateThread(NULL, 0, czl_run_thread, argv, 0, NULL)))
        goto CZL_END;
#elif defined CZL_SYSTEM_LINUX
    if (pthread_create(&p->id, NULL, czl_run_thread, argv))
        goto CZL_END;
#endif

    fun->ret.val.inum = (unsigned long)p;
    return 1;

CZL_END:
    free(argv);
    free(shell_path);
#ifndef CZL_CONSOLE
    free(log_path);
#endif
    czl_thread_delete(gp, p);

    return 1;
}

czl_thread* czl_thread_find(czl_gp *gp, unsigned long id, czl_fun *fun)
{
    czl_thread *p = (czl_thread*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                                   (char*)id, &gp->threads_hash);
    if (!p)
        return NULL;

    if (!p->pipe->alive)
    {
        if (fun && p->pipe->rb_cnt)
            czl_report_buf_get(gp, &fun->ret, p->pipe);
        czl_thread_delete(gp, p);
        return NULL;
    }

    return p;
}

char czl_sys_wait(czl_gp *gp, czl_fun *fun)
{
    if (0 == fun->vars->val.inum)
    {
        if (!gp->thread_pipe)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        if (gp->thread_pipe->kill)
        {
            gp->exit_code = CZL_EXIT_ABORT;
            return 0;
        }
        czl_thread_lock(&gp->thread_pipe->notify_lock); //lock
        if (gp->thread_pipe->nb_cnt)
            czl_notify_buf_get(gp, &fun->ret, gp->thread_pipe);
        else
        {
            czl_thread_unlock(&gp->thread_pipe->notify_lock); //unlock
            czl_event_wait(&gp->thread_pipe->notify_event);
            czl_thread_lock(&gp->thread_pipe->notify_lock); //lock
            if (gp->thread_pipe->nb_cnt)
                czl_notify_buf_get(gp, &fun->ret, gp->thread_pipe);
            else
                fun->ret.val.inum = 0;
        }
        czl_thread_unlock(&gp->thread_pipe->notify_lock); //unlock
    }
    else
    {
        czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, fun);
        if (!p)
        {
            if (CZL_INT == fun->ret.type)
                fun->ret.val.inum = 0;
            return 1;
        }
        czl_thread_lock(&p->pipe->report_lock); //lock
        if (p->pipe->rb_cnt)
            czl_report_buf_get(gp, &fun->ret, p->pipe);
        else
        {
            czl_thread_unlock(&p->pipe->report_lock); //unlock
            czl_event_wait(&p->pipe->report_event);
            czl_thread_lock(&p->pipe->report_lock); //lock
            if (p->pipe->rb_cnt)
                czl_report_buf_get(gp, &fun->ret, p->pipe);
            else
                fun->ret.val.inum = 0;
        }
        czl_thread_unlock(&p->pipe->report_lock); //unlock
    }

    return 1;
}

char czl_sys_listen(czl_gp *gp, czl_fun *fun)
{
    if (0 == fun->vars->val.inum)
    {
        if (!gp->thread_pipe)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        if (gp->thread_pipe->kill)
        {
            gp->exit_code = CZL_EXIT_ABORT;
            return 0;
        }
        czl_thread_lock(&gp->thread_pipe->notify_lock); //lock
        if (gp->thread_pipe->nb_cnt)
            czl_notify_buf_get(gp, &fun->ret, gp->thread_pipe);
        else
            fun->ret.val.inum = 0;
        czl_thread_unlock(&gp->thread_pipe->notify_lock); //unlock
    }
    else
    {
        czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, fun);
        if (!p)
        {
            if (CZL_INT == fun->ret.type)
                fun->ret.val.inum = 0;
            return 1;
        }
        czl_thread_lock(&p->pipe->report_lock); //lock
        if (p->pipe->rb_cnt)
            czl_report_buf_get(gp, &fun->ret, p->pipe);
        else
            fun->ret.val.inum = 0;
        czl_thread_unlock(&p->pipe->report_lock); //unlock
    }

    return 1;
}

char czl_sys_waitfor(czl_gp *gp, czl_fun *fun)
{
    czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL);
    if (p)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        WaitForSingleObject(p->id, INFINITE);
    #elif defined CZL_SYSTEM_LINUX
        pthread_join(p->id, NULL);
    #endif
        if (p->pipe->rb_cnt)
            czl_report_buf_get(gp, &fun->ret, p->pipe);
        czl_thread_delete(gp, p);
    }
    else
        fun->ret.val.inum = 0;

    return 1;
}

char czl_sys_report(czl_gp *gp, czl_fun *fun)
{
    if (!gp->thread_pipe)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (czl_report_buf_create(gp, gp->thread_pipe, fun->vars))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    czl_event_send(&gp->thread_pipe->report_event);

    return 1;
}

char czl_sys_notify(czl_gp *gp, czl_fun *fun)
{
    czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL);
    if (!p)
    {
        fun->ret.val.inum = 0;
        return 1;
    }
    if (czl_notify_buf_create(gp, p->pipe, fun->vars+1))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    czl_event_send(&p->pipe->notify_event);

    return 1;
}

char czl_sys_notifyall(czl_gp *gp, czl_fun *fun)
{
	czl_thread *p = gp->threads_head;

    fun->ret.val.inum = 1;

    while (p)
    {
        if (!p->pipe->alive)
        {
            czl_thread *q = p->next;
            czl_thread_delete(gp, p);
            p = q;
            continue;
        }
        if (!czl_notify_buf_create(gp, p->pipe, fun->vars))
            fun->ret.val.inum = 0;
        czl_event_send(&p->pipe->notify_event);
        p = p->next;
    }

    return 1;
}

char czl_sys_thrsta(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = 1;

    if (fun->vars->val.inum)
    {
        if (!czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL))
            fun->ret.val.inum = 0;
    }
    else
    {
        czl_thread *p = gp->threads_head;
        while (p)
        {
            if (!p->pipe->alive)
            {
                czl_thread *q = p->next;
                czl_thread_delete(gp, p);
                p = q;
                fun->ret.val.inum = 0;
                continue;
            }
            p = p->next;
        }
    }

    return 1;
}
#endif //#ifdef CZL_MULT_THREAD
///////////////////////////////////////////////////////////////
#ifndef CZL_CONSOLE
char czl_sys_cleanlog(czl_gp *gp, czl_fun *fun)
{
	FILE *fout;

    if (!gp->log_path)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (!(fout=fopen(gp->log_path, "w")))
        fun->ret.val.inum = 0;
    else
    {
        fun->ret.val.inum = 1;
        fclose(fout);
    }

    return 1;
}

char czl_sys_openlog(czl_gp *gp, czl_fun *fun)
{
    free(gp->log_path);
    if (!(gp->log_path=malloc(CZL_STR(fun->vars->val.str.Obj)->len+1)))
        return 0;
    strcpy(gp->log_path, CZL_STR(fun->vars->val.str.Obj)->str);
    return 1;
}

char czl_sys_closelog(czl_gp *gp, czl_fun *fun)
{
    free(gp->log_path);
    gp->log_path = NULL;
    return 1;
}

char czl_sys_insert(czl_gp *gp, czl_fun *fun)
{
    czl_tabkv *ele;
    void **obj = fun->vars[1].val.Obj;

    if (fun->vars[1].type != CZL_INSTANCE || CZL_INS(obj)->pclass != gp->pclass ||
        !(ele=czl_tabkv_create(gp, CZL_TAB(gp->table), fun->vars->val.inum)))
        fun->ret.val.inum = 0;
    else
    {
        czl_val_del(gp, (czl_var*)ele);
        ele->type = CZL_INSTANCE;
        ele->val.Obj = obj;
        fun->vars[1].type = CZL_INT;
        fun->ret.val.inum = 1;
    }

    return 1;
}

char czl_sys_get(czl_gp *gp, czl_fun *fun)
{
    czl_tabkv *ele;

    if (!gp->table || !(ele=czl_tabkv_find(CZL_TAB(gp->table), fun->vars->val.inum)))
    {
        fun->ret.type = CZL_INT;
        fun->ret.val.inum = 0;
    }
    else
    {
        fun->ret.type = CZL_INSTANCE;
        fun->ret.val.Obj = ele->val.Obj;
        CZL_ORCA1(ele->val.Obj);
    }

    return 1;
}

char czl_sys_delete(czl_gp *gp, czl_fun *fun)
{
    if (!gp->table)
        fun->ret.val.inum = 0;
    else
        fun->ret.val.inum = czl_tabkv_delete(gp, CZL_TAB(gp->table), fun->vars->val.inum);
    return 1;
}

char czl_sys_count(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_count(gp);
    return 1;
}

char czl_sys_clean(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_clean(gp);
    return 1;
}

char czl_insert(czl_gp *gp, int key, void *val)
{
    czl_ins *ins;
    czl_var *var;
    czl_tabkv *ele;
    unsigned long i, cnt;
    char *tmp = (char*)val;

    if (!gp->table || !gp->pclass) return 0;

    if (!(ele=czl_tabkv_create(gp, CZL_TAB(gp->table), key)))
        return 0;
    czl_val_del(gp, (czl_var*)ele);

    if (!(ele->val.Obj=czl_instance_fork(gp, gp->pclass, 0)))
        goto CZL_END;

    ele->type = CZL_INSTANCE;

    ins = CZL_INS(ele->val.Obj);
    var = CZL_GIV(ins);
    cnt = ins->pclass->vars_count;
    for (i = 0; i < cnt; ++i, ++var)
    {
        switch (var->ot)
        {
        case CZL_INT:
            var->val.inum = *((int*)tmp);
            tmp += 4;
            break;
        case CZL_FLOAT:
            var->type = CZL_FLOAT;
            var->val.fnum = *((float*)tmp);
            tmp += 4;
            break;
        case CZL_STRING:
            if (!czl_set_ret_str(gp, var, *((char**)tmp), strlen(*((char**)tmp))))
                goto CZL_END;
            tmp += 4;
            break;
        default:
            break;
        }
    }

    return 1;

CZL_END:
    czl_tabkv_delete(gp, CZL_TAB(gp->table), key);
    return 0;
}

char czl_get(czl_gp *gp, int key, void *val)
{
    czl_ins *ins;
    czl_var *var;
    unsigned long i, cnt;
    char *tmp = (char*)val;
    czl_tabkv *ele;

    if (!gp->table || !(ele=czl_tabkv_find(CZL_TAB(gp->table), key)))
        return 0;

    ins = CZL_INS(ele->val.Obj);
    var = CZL_GIV(ins);
    cnt = ins->pclass->vars_count;
    for (i = 0; i < cnt; ++i, ++var)
    {
        switch (var->type)
        {
        case CZL_INT:
            *((int*)tmp) = var->val.inum;
            tmp += 4;
            break;
        case CZL_FLOAT:
            *((float*)tmp) = var->val.fnum;
            tmp += 4;
            break;
        case CZL_STRING:
            *((char**)tmp) = CZL_STR(var->val.str.Obj)->str;
            tmp += 4;
            break;
        default:
            break;
        }
    }

    return 1;
}

char czl_delete(czl_gp *gp, int key)
{
    if (!gp->table || !gp->pclass) return 0;

    return czl_tabkv_delete(gp, CZL_TAB(gp->table), key);
}

unsigned long czl_count(czl_gp *gp)
{
    if (!gp->table) return 0;

    return CZL_TAB(gp->table)->count;
}

char czl_clean(czl_gp *gp)
{
    void **table;

    if (!gp->table || !(table=czl_empty_table_create(gp)))
        return 0;

    czl_table_delete(gp, gp->table);
    gp->table = table;
    return 1;
}

char czl_exec(czl_gp *gp)
{
    char ret = czl_resume_shell(gp, gp->cur_fun);

    if (ret != 2)
        czl_memory_free(gp);

    return ret;
}

czl_gp* czl_open(char *shell_path, char *log_path, char *class_name)
{
    czl_gp *gp;

    if (!shell_path)
        return NULL;

    if (!(gp=(czl_gp*)malloc(sizeof(czl_gp))))
        return NULL;
    memset(gp, 0, sizeof(czl_gp));

    if (!(gp->shell_path=malloc(strlen(shell_path)+1)))
        goto CZL_END;
    strcpy(gp->shell_path, shell_path);

    if (log_path)
    {
       if (!(gp->log_path=malloc(strlen(log_path)+1)))
           goto CZL_END;
       strcpy(gp->log_path, log_path);
    }
    else
        gp->log_path = NULL;

    if (class_name)
    {
       if (!(gp->class_name=malloc(strlen(class_name)+1)))
           goto CZL_END;
       strcpy(gp->class_name, class_name);
    }
    else
        gp->class_name = NULL;

    if (!czl_sys_init(gp))
    {
        czl_init_free(gp, 1);
        goto CZL_END;
    }
    else if (!czl_exec_prepare(gp))
    {
        czl_memory_free(gp);
        goto CZL_END;
    }

    return gp;

CZL_END:
    free(gp->shell_path);
    free(gp->log_path);
    free(gp->class_name);
    free(gp);
    return NULL;
}

void czl_close(czl_gp *gp)
{
    if (gp->table)
        czl_memory_free(gp);
    free(gp->shell_path);
    free(gp->log_path);
    free(gp->class_name);
    free(gp);
}
#endif //#ifndef CZL_CONSOLE

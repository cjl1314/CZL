#include "czl_lib.h"
#include "czl_paser.h"
/**************************************************************
库函数命名规则:
    1. 统一驼峰命名方式
    2. 动词在前，名词在后

库函数参数顺序:
    1. 操作对象在第一个
    2. 无默认参数在中间
    3. 有默认参数在后面
**************************************************************/
///////////////////////////////////////////////////////////////
char czl_sys_echo(czl_gp*, czl_fun*);
char czl_sys_print(czl_gp*, czl_fun*);
#ifdef CZL_CONSOLE
char czl_sys_input(czl_gp*, czl_fun*);
#endif //CZL_CONSOLE
//
char czl_sys_fopen(czl_gp*, czl_fun*);
char czl_sys_fclose(czl_gp*, czl_fun*);
char czl_sys_fwrite(czl_gp*, czl_fun*);
char czl_sys_fread(czl_gp*, czl_fun*);
char czl_sys_fseek(czl_gp*, czl_fun*);
char czl_sys_ftell(czl_gp*, czl_fun*);
char czl_sys_fprint(czl_gp*, czl_fun*);
char czl_sys_fclean(czl_gp*, czl_fun*);
char czl_sys_read(czl_gp*, czl_fun*);
char czl_sys_close(czl_gp*, czl_fun*);
//
char czl_sys_srand(czl_gp*, czl_fun*);
char czl_sys_rand(czl_gp*, czl_fun*);
char czl_sys_rands(czl_gp*, czl_fun*);
//
char czl_sys_int(czl_gp*, czl_fun*);
char czl_sys_float(czl_gp*, czl_fun*);
char czl_sys_num(czl_gp*, czl_fun*);
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
char czl_sys_memset(czl_gp*, czl_fun*);
char czl_sys_memget(czl_gp*, czl_fun*);
char czl_sys_memcmp(czl_gp*, czl_fun*);
char czl_sys_memspn(czl_gp*, czl_fun*);
char czl_sys_memrep(czl_gp*, czl_fun*);
//
char czl_sys_abort(czl_gp*, czl_fun*);
char czl_sys_assert(czl_gp*, czl_fun*);
char czl_sys_errLine(czl_gp*, czl_fun*);
char czl_sys_errfile(czl_gp*, czl_fun*);
char czl_sys_errcode(czl_gp*, czl_fun*);
char czl_sys_setFun(czl_gp*, czl_fun*);
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_sleep(czl_gp*, czl_fun*);
char czl_sys_clock(czl_gp*, czl_fun*);
#ifdef CZL_TIMER
char czl_sys_timer(czl_gp*, czl_fun*);
#endif //#ifdef CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_date(czl_gp*, czl_fun*);
char czl_sys_useMem(czl_gp*, czl_fun*);
char czl_sys_maxMem(czl_gp*, czl_fun*);
char czl_sys_setMem(czl_gp*, czl_fun*);
#ifdef CZL_MM_MODULE
#ifdef CZL_MM_CACHE
char czl_sys_setCache(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MM_CACHE
char czl_sys_setRank(czl_gp*, czl_fun*);
char czl_sys_setSa(czl_gp*, czl_fun*);
char czl_sys_setGc(czl_gp*, czl_fun*);
char czl_sys_gc(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MM_MODULE
char czl_sys_runShell(czl_gp*, czl_fun*);
//
char czl_sys_sort(czl_gp*, czl_fun*);
//
char czl_sys_toBin(czl_gp*, czl_fun*);
char czl_sys_toObj(czl_gp*, czl_fun*);
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
char czl_sys_reset(czl_gp*, czl_fun*);
char czl_sys_corsta(czl_gp*, czl_fun*);
char czl_sys_resume(czl_gp*, czl_fun*);
char czl_sys_kill(czl_gp*, czl_fun*);
//
#if (defined CZL_LIB_TCP || defined CZL_LIB_UDP)
char czl_sys_dns(czl_gp*, czl_fun*);
#endif //#if (defined CZL_LIB_TCP || defined CZL_LIB_UDP)
//
#ifdef CZL_MULT_THREAD
char czl_sys_thread(czl_gp*, czl_fun*);
char czl_sys_wait(czl_gp*, czl_fun*);
char czl_sys_listen(czl_gp*, czl_fun*);
char czl_sys_waitfor(czl_gp*, czl_fun*);
char czl_sys_report(czl_gp*, czl_fun*);
char czl_sys_notify(czl_gp*, czl_fun*);
char czl_sys_notifyAll(czl_gp*, czl_fun*);
char czl_sys_thrsta(czl_gp*, czl_fun*);
char czl_sys_suspend(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MULT_THREAD
//
#ifndef CZL_CONSOLE
char czl_sys_cleanLog(czl_gp*, czl_fun*);
char czl_sys_openLog(czl_gp*, czl_fun*);
char czl_sys_closeLog(czl_gp*, czl_fun*);
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
    {"fopen",     czl_sys_fopen,      2,  "str_v1,str_v2=\"1r\""},
    {"fclose",    czl_sys_fclose,     1,  "&file_v1"},
    {"fwrite",    czl_sys_fwrite,     -2, NULL},
    {"fread",     czl_sys_fread,      2,  "file_v1,int_v2=0"},
    {"fseek",     czl_sys_fseek,      2,  "&file_v1,int_v2"},
    {"ftell",     czl_sys_ftell,      1,  "file_v1"},
    {"fprint",    czl_sys_fprint,     -2, NULL},
    {"fclean",    czl_sys_fclean,     1,  "str_v1"},
    {"read",      czl_sys_read,       2,  "str_v1,int_v2=-1"},
    {"close",     czl_sys_close,      1,  "str_v1"},
    //随机数函数
    {"srand",     czl_sys_srand,      1,  "int_v1"},
    {"rand",      czl_sys_rand,       0,  NULL},
    {"rands",     czl_sys_rands,      0,  NULL},
    //数值处理函数
    {"int",       czl_sys_int,        1,  NULL},
    {"float",     czl_sys_float,      1,  NULL},
    {"num",       czl_sys_num,        1,  NULL},
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
    {"memset",    czl_sys_memset,     4,  "&str_v1,int_v2,int_v3=0,int_v4=-1"},
    {"memget",    czl_sys_memget,     3,  "str_v1,int_v2=0,int_v3=-1"},
    {"memcmp",    czl_sys_memcmp,     4,  "str_v1,str_v2,int_v3=-1,int_v4=-1"},
    {"memspn",    czl_sys_memspn,     4,  "str_v1,str_v2,int_v3=0,int_v4=-1"},
    {"memrep",    czl_sys_memrep,     5,  "&str_v1,str_v2,str_v3,int_v4=0,int_v5=-1"},
    //系统函数
    {"abort",     czl_sys_abort,      1,  "v1=1"},
    {"assert",    czl_sys_assert,     1,  NULL},
    {"errLine",   czl_sys_errLine,    0,  NULL},
    {"errfile",   czl_sys_errfile,    0,  NULL},
    {"errcode",   czl_sys_errcode,    0,  NULL},
    {"setFun",    czl_sys_setFun,     2,  "fun_v1,int_v2=0"},
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"sleep",     czl_sys_sleep,      1,  "int_v1"},
    {"clock",     czl_sys_clock,      1,  "int_v1=0"},
#ifdef CZL_TIMER
    {"timer",     czl_sys_timer,      2,  "int_v1,fun_v2"},
#endif //#ifdef CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"date",      czl_sys_date,       1,  "str_v1=\"Y-M-D h:m:s\""},
    {"useMem",    czl_sys_useMem,     0,  NULL},
    {"maxMem",    czl_sys_maxMem,     0,  NULL},
    {"setMem",    czl_sys_setMem,     1,  "int_v1"},
#ifdef CZL_MM_MODULE
    #ifdef CZL_MM_CACHE
    {"setCache",  czl_sys_setCache,   1,  "int_v1"},
    #endif //#ifdef CZL_MM_CACHE
    {"setRank",   czl_sys_setRank,    1,  "int_v1"},
    {"setSa",     czl_sys_setSa,      1,  "int_v1"},
    {"setGc",     czl_sys_setGc,      1,  "int_v1"},
    {"gc",        czl_sys_gc,         0,  NULL},
#endif //#ifdef CZL_MM_MODULE
    {"runShell",  czl_sys_runShell,   3,  "str_v1,&v2,v3=0"},
    //排序函数
    {"sort",      czl_sys_sort,       3,  "&v1,str_v2=\"asc\",v3=0"},
    //序列化、反序列化函数
    {"toBin",     czl_sys_toBin,      1,  NULL},
    {"toObj",     czl_sys_toObj,      1,  "str_v1"},
    //哈希表内建函数
    {"hcac",      czl_sys_hcac,       1,  "map_v1"},
    {"hdod",      czl_sys_hdod,       3,  "map_v1,int_v2,int_v3"},
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
    {"ins",       czl_sys_ins,        4,  "&map_v1"},
    //协程框架接口函数
    {"coroutine", czl_sys_coroutine,  2,  "fun_v1,int_v2=0"},
    {"reset",     czl_sys_reset,      1,  NULL},
    {"corsta",    czl_sys_corsta,     1,  "int_v1"},
    //线程和协程共用kill函数
    {"resume",    czl_sys_resume,     -1, NULL},
    {"kill",      czl_sys_kill,       1,  "int_v1"},
#if (defined CZL_LIB_TCP || defined CZL_LIB_UDP)
    //域名解析函数
    {"dns",       czl_sys_dns,        1,  "str_v1"},
#endif //#if (defined CZL_LIB_TCP || defined CZL_LIB_UDP)
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
    {"notifyAll", czl_sys_notifyAll,  1,  "v1=0"},
    {"thrsta",    czl_sys_thrsta,     1,  "int_v1=0"},
    {"suspend",   czl_sys_suspend,    1,  "int_v1"},
#endif //#ifdef CZL_MULT_THREAD
#ifndef CZL_CONSOLE
    //与C/C++栈交互接口函数
    {"cleanLog",  czl_sys_cleanLog,   0,  NULL},
    {"openLog",   czl_sys_openLog,    1,  "str_v1"},
    {"closeLog",  czl_sys_closeLog,   0,  NULL},
    {"insert",    czl_sys_insert,     2,  "int_v1"},
    {"get",       czl_sys_get,        1,  "int_v1"},
    {"delete",    czl_sys_delete,     1,  "int_v1"},
    {"count",     czl_sys_count,      0,  NULL},
    {"clean",     czl_sys_clean,      0,  NULL},
#endif //#ifndef CZL_CONSOLE
};
///////////////////////////////////////////////////////////////
#ifdef CZL_LIB_COM
#include "czl_com.h" //COM库
#endif //CZL_LIB_COM

#ifdef CZL_LIB_TCP
#include "czl_tcp.h" //TCP库
#endif //CZL_LIB_TCP

#ifdef CZL_LIB_UDP
#include "czl_udp.h" //UDP库
#endif //CZL_LIB_UDP

#ifdef CZL_LIB_HTTP
#include "czl_http.h" //HTTP库
#endif //CZL_LIB_HTTP

#ifdef CZL_LIB_REG
#include "czl_pcre.h" //PCRE库
#endif //CZL_LIB_REG

const czl_sys_lib czl_syslibs[] =
{
    //库名,             库指针,           库函数个数
    {"os",             czl_lib_os,      sizeof(czl_lib_os)/sizeof(czl_sys_fun)},

#ifdef CZL_LIB_COM
    {"com",            czl_lib_com,     CZL_LIB_COM_CNT}, //COM库
#endif //CZL_LIB_COM

#ifdef CZL_LIB_TCP
    {"tcp",            czl_lib_tcp,     CZL_LIB_TCP_CNT}, //TCP库
#endif //CZL_LIB_TCP

#ifdef CZL_LIB_UDP
    {"udp",            czl_lib_udp,     CZL_LIB_UDP_CNT}, //UDP库
#endif //CZL_LIB_UDP

#ifdef CZL_LIB_HTTP
    {"http",           czl_lib_http,    CZL_LIB_HTTP_CNT}, //HTTP库
#endif //CZL_LIB_HTTP

#ifdef CZL_LIB_REG
    {"reg",           czl_lib_reg,    CZL_LIB_REG_CNT}, //PCRE库
#endif //CZL_LIB_REG
};
const unsigned long czl_syslibs_num = sizeof(czl_syslibs)/sizeof(czl_sys_lib);
///////////////////////////////////////////////////////////////
#define CZL_FILE_BUF_SIZE   10*1024
const unsigned long czl_check_sum = 0xF1E2C3B4;
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
            fprintf(fout, "%s:", CZL_STR(p->key.str.Obj)->str);
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
    czl_var *ret;
    czl_table_node *ele = list->paras;

    fprintf(fout, "{");
    while (ele)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return 0;
        if (CZL_FLOAT == ret->type)
        {
            ret->type = CZL_INT;
            ret->val.inum = (czl_long)ret->val.fnum;
        }
        else if (ret->type != CZL_INT && ret->type != CZL_STRING)
            return 0;
        czl_print_obj(gp, ret, fout);
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

char czl_modify_print_obj
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
        break;
    case CZL_FLOAT:
        if (CZL_INT == res->type)
            fprintf(fout, modify, (double)res->val.inum);
        else
            fprintf(fout, modify, res->val.fnum);
        break;
    case 'x': case 'X':
        czl_print_str_hex(CZL_STR(res->val.str.Obj), modify_type, fout);
        break;
    case 'b': case 'B':
        czl_print_str_bin(CZL_STR(res->val.str.Obj), modify_type, fout);
        break;
    default: //CZL_OBJ_REF
        return czl_print_obj(gp, res, fout);
    }
    return 1;
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
        czl_global_lock();
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
        czl_global_unlock();
    #endif
#else
    fclose(fout);
#endif
    return ret;
}

char czl_modify_print(czl_gp *gp, czl_para *p, FILE *fout)
{
    char *s;
    char modify_type;
    char modify[16] = "%";
    czl_var *res;
    czl_str str;

    while (p)
    {
        if (!(res=czl_exp_cac(gp, p->para)))
            return 0;

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
                                                       &modify_type, res->type)))
                        {
                            CZL_SRCD1(gp, str);
                            return 0;
                        }
                        if (!czl_modify_print_obj(gp, modify, modify_type, res, fout))
                        {
                            CZL_SRCD1(gp, str);
                            return 0;
                        }
                    }
                }
                else
                    fputc(*s, fout);
            }
            CZL_SRCD1(gp, str);
        }
        else if (!czl_print_obj(gp, res, fout))
            return 0;

        p = p->next;
    }

    return 1;
}

char czl_sys_print(czl_gp *gp, czl_fun *fun)
{
    char ret;
	FILE *fout;

#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        //多线程控制台工作方式下print递归会导致死锁
        czl_global_lock();
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

    ret = czl_modify_print(gp, (czl_para*)fun->vars, fout);

#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        czl_global_unlock();
    #endif
#else
    fclose(fout);
#endif
    return ret;
}

#ifdef CZL_CONSOLE
char czl_sys_input(czl_gp *gp, czl_fun *fun)
{
    char buf[CZL_FILE_BUF_SIZE];
	unsigned long len;

    if (!fgets(buf, CZL_FILE_BUF_SIZE, stdin))
        return 0;
    len = strlen(buf) - 1;
    buf[len] = '\0';

    return czl_set_ret_str(gp, &fun->ret, buf, len);
}
#endif //CZL_CONSOLE
///////////////////////////////////////////////////////////////
unsigned long czl_sizeof_int(czl_long num)
{
    if (num >= -128 && num <= 127)
        return 2;
    else if (num >= -32768 && num <= 32767)
        return 3;
    else if (num >= -2147483647 && num <= 2147483647)
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
        *sum += flag ? sizeof(czl_float)+1 : sizeof(czl_float);
        return 1;
    case CZL_STRING:
        *sum += flag ? CZL_STR(obj->val.str.Obj)->len +
                       czl_sizeof_int(CZL_STR(obj->val.str.Obj)->len) + 1 :
                       obj->val.str.size;
        return 1;
    case CZL_FILE:
        *sum += flag ?
         czl_sizeof_int(czl_get_file_size(CZL_FIL(obj->val.Obj)->fp))+1 : 4;
        return 1;
    case CZL_FUN_REF:
        *sum += flag ? 5 : 4;
        return 1;
    case CZL_INSTANCE:
        czl_sizeof_ins(gp, flag, CZL_INS(obj->val.Obj), sum);
        if (flag)
            *sum += (2+strlen(CZL_INS(obj->val.Obj)->pclass->name));
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
char czl_sys_fopen(czl_gp *gp, czl_fun *fun)
{
    czl_file *file;
    void **obj = NULL;
    char *path = CZL_STR(fun->vars->val.str.Obj)->str;
    char *mode = CZL_STR(fun->vars[1].val.str.Obj)->str;
    char structMode = *(mode++) - '0';

    if (structMode != 1 && structMode != 2 && structMode != 3)
        goto CZL_END;

    if (!(obj=(void**)CZL_FILE_MALLOC(gp)))
        return 0;

    file = CZL_FIL(obj);
    if (!(file->fp = fopen(path, mode)))
        goto CZL_END;

    file->rc = 1;
    file->mode = structMode;
    file->sign = ',';
#ifdef CZL_SYSTEM_WINDOWS
    file->txt = ('b' == mode[1] ? 0 : 1);
#endif
    file->addr = 0;

    fun->ret.type = CZL_FILE;
    fun->ret.val.Obj = obj;
    return 1;

CZL_END:
    fun->ret.type = CZL_INT;
    fun->ret.val.inum = 0;
    CZL_FILE_FREE(gp, obj);
    return 1;
}

char czl_sys_fclose(czl_gp *gp, czl_fun *fun)
{
    czl_var *var = CZL_GRV(fun->vars);

    if (0 == CZL_ORCD1(var->val.Obj))
        fun->ret.val.inum = czl_file_delete(gp, var->val.Obj);
    else
        fun->ret.val.inum = 1;

    var->type = CZL_INT;
    var->val.inum = 0;

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
    else if (num >= -2147483647 && num <= 2147483647)
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


    strcpy(buf, ins->pclass->name);
    buf += (strlen(ins->pclass->name)+1);

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
    *buf++ = obj->type;

    switch (obj->type)
    {
    case CZL_INT:
        return czl_get_int_buf(obj->val.inum, buf);
    case CZL_FLOAT:
        *((czl_float*)buf) = obj->val.fnum;
        return buf+sizeof(czl_float);
    case CZL_STRING:
        return czl_get_str_buf(CZL_STR(obj->val.str.Obj), buf);
    case CZL_INSTANCE:
        return czl_get_ins_buf(gp, CZL_INS(obj->val.Obj), buf);
    case CZL_TABLE:
        return czl_get_tab_buf(gp, CZL_TAB(obj->val.Obj), buf);
    case CZL_ARRAY:
        return czl_get_arr_buf(gp, CZL_ARR(obj->val.Obj), buf);
    case CZL_STACK: case CZL_QUEUE:
        return czl_get_sq_buf(gp, CZL_SQ(obj->val.Obj), buf);
    case CZL_FILE:
        *(buf-1) = CZL_INT; //文件只保存大小信息
        return czl_get_int_buf(czl_get_file_size(CZL_FIL(obj->val.Obj)->fp), buf);
    case CZL_FUN_REF:
        *((unsigned long*)buf) = (unsigned long)obj->val.fun;
        return buf+4;
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
    if (!ftell(fp))
    {
        if (!fwrite(&czl_check_sum, 4, 1, fp))
            return 0;
    }

    while (p)
    {
        czl_var *res;
        unsigned long obj_size = 4;
        char buf[CZL_FILE_BUF_SIZE];
        char *tmp = buf;
        if (!(res=czl_exp_cac(gp, p->para)) || !czl_sizeof_obj(gp, 1, res, &obj_size) ||
            (obj_size > CZL_FILE_BUF_SIZE && !(tmp=(char*)CZL_TMP_MALLOC(gp, obj_size))))
            return 0;
        *((unsigned long*)tmp) = obj_size-4;
        czl_get_obj_buf(gp, res, tmp+4);
        if (!fwrite(tmp, obj_size, 1, fp))
        {
            if (obj_size > CZL_FILE_BUF_SIZE)
                CZL_TMP_FREE(gp, buf, obj_size);
            return 0;
        }
        if (obj_size > CZL_FILE_BUF_SIZE)
            CZL_TMP_FREE(gp, buf, obj_size);
        p = p->next;
    }

    return 1;
}

char czl_line_write(czl_gp *gp, FILE *fp, char sign, czl_para *p)
{
    if (fseek(fp, 0, SEEK_END))
        return 0;

    for (; p; p = p->next)
    {
        char *item;
        char tmp[32];
        char flag;
        czl_var *ret = czl_exp_cac(gp, p->para);
        if (!ret)
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
        flag = (p->next ? sign : '\n');
        if (flag && EOF == fputc(flag, fp))
            return 0;
    }

    return 1;
}

char czl_byte_write(czl_gp *gp, FILE *fp, czl_para *p)
{
    for (; p; p = p->next)
    {
        char *item;
        char tmp[32];
        unsigned long i = 0, len;
        czl_var *ret = czl_exp_cac(gp, p->para);
        if (!ret)
            return 0;
        switch (ret->type)
        {
        case CZL_STRING:
            item = CZL_STR(ret->val.str.Obj)->str;
            len = CZL_STR(ret->val.str.Obj)->len;
            break;
        case CZL_INT:
            item = tmp;
            len = czl_itoa(ret->val.inum, tmp);
            break;
        case CZL_FLOAT:
            item = tmp;
            len = czl_ftoa(ret->val.fnum, tmp, 6);
            break;
        default:
            continue;
        }
        while (i < len && fputc(*item++, fp) != EOF)
            ++i;
    }

    return 1;
}

char czl_sys_fwrite(czl_gp *gp, czl_fun *fun)
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

    if (1 == f->mode)
        fun->ret.val.inum = czl_byte_write(gp, f->fp, paras->next);
    else
    {
        if (fseek(f->fp, 0, SEEK_END))
            fun->ret.val.inum = 0;
        else
            fun->ret.val.inum = (2 == f->mode ?
                                 czl_line_write(gp, f->fp, f->sign, paras->next) :
                                 czl_struct_write(gp, f->fp, paras->next));
    }

    return 1;
}

char* czl_analysis_int_buf(char *buf, czl_long *num)
{
    switch (*buf)
    {
    case 2: *num = *((char*)(buf+1)); break;
    case 3: *num = *((short*)(buf+1)); break;
    case 5: *num = *((long*)(buf+1)); break;
    default: *num = *((czl_long*)(buf+1)); break;
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
    czl_class *pclass = (czl_class*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                      buf, &gp->class_hash);

    if (!pclass || !(obj->val.Obj=czl_instance_fork(gp, pclass, 0)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

    return czl_analysis_ins(gp, buf+strlen(buf)+1, CZL_INS(obj->val.Obj));
}

char* czl_analysis_tab_buf(czl_gp *gp, char *buf, czl_var *obj)
{
	char kt;
    czl_long num;
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

    if (!(obj->val.Obj=czl_table_create(gp, len, key, attack_cnt)))
    {
        obj->type = CZL_INT;
        return NULL;
    }

    tab = CZL_TAB(obj->val.Obj);

    while ((kt=*buf++) != CZL_NIL)
    {
        if (CZL_STRING == kt)
        {
            hash = *((unsigned long*)buf);
            buf = czl_analysis_int_buf(buf+4, &num);
            if (!(ele=czl_create_str_tabkv(gp, tab, hash, num, buf)))
                return NULL;
            buf += num;
        }
        else
        {
            buf = czl_analysis_int_buf(buf, &num);
            if (!(ele=czl_create_num_tabkv(gp, tab, num)))
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

char* czl_analysis_str_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_long len;
    buf = czl_analysis_int_buf(buf, &len);
    if (!czl_str_create(gp, &obj->val.str, len+1, len, buf))
    {
        obj->type = CZL_INT;
        return NULL;
    }
    return buf+len;
}

char* czl_analysis_ele_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    obj->type = *buf++;

    switch (obj->type)
    {
    case CZL_INT: //CZL_FILE类型只保存文件大小
        return czl_analysis_int_buf(buf, &obj->val.inum);
    case CZL_FLOAT:
        obj->val.fnum = *((czl_float*)buf);
        return buf+sizeof(czl_float);
    case CZL_STRING:
        return czl_analysis_str_buf(gp, buf, obj);
    case CZL_FUN_REF:
        obj->val.fun = (czl_fun*)(*((unsigned long*)buf));
        return buf+4;
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

void** czl_analysis_obj_buf(czl_gp *gp, char *buf)
{
    void **obj;
    czl_array *arr;

    if (!(obj=czl_array_create(gp, 2, 0)))
        return NULL;

    arr = CZL_ARR(obj);

    while (*buf != CZL_NIL)
    {
        if (!(buf=czl_analysis_ele_buf(gp, buf+4, arr->vars+arr->cnt++)) ||
            (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt)))
        {
            czl_array_delete(gp, obj);
            return NULL;
        }
    }

    return obj;
}

char czl_obj_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
    unsigned long size;
    char buf[CZL_FILE_BUF_SIZE];
    char *tmp = buf;

    if (!ftell(fp))
    {
        unsigned long check_sum;
        if (!fread(&check_sum, 4, 1, fp) || check_sum != czl_check_sum)
            return 0;
    }

    if (!fread(&size, 4, 1, fp))
        return 0;

    if (size > CZL_FILE_BUF_SIZE && !(tmp=(char*)CZL_TMP_MALLOC(gp, size)))
        return 0;

    if (!fread(tmp, size, 1, fp) || !czl_analysis_ele_buf(gp, tmp, ret))
    {
        if (size > CZL_FILE_BUF_SIZE)
            CZL_TMP_FREE(gp, buf, size);
        return 0;
    }

    if (size > CZL_FILE_BUF_SIZE)
        CZL_TMP_FREE(gp, buf, size);

    return 1;
}

char czl_objs_read(czl_gp *gp, FILE *fp, czl_var *ret, unsigned long sum)
{
    czl_array *arr;

    if (!ftell(fp))
    {
        unsigned long check_sum;
        if (!fread(&check_sum, 4, 1, fp) || check_sum != czl_check_sum)
            return 0;
    }

    if (!(ret->val.Obj=czl_array_create(gp, 2, 0)))
        return 0;

    arr = CZL_ARR(ret->val.Obj);

    do {
        unsigned long size;
        char buf[CZL_FILE_BUF_SIZE];
        char *tmp = buf;
        if (!fread(&size, 4, 1, fp))
            break;
        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            goto CZL_OOM;
        if (size > CZL_FILE_BUF_SIZE && !(tmp=(char*)CZL_TMP_MALLOC(gp, size)))
            goto CZL_OOM;
        if (!fread(tmp, size, 1, fp))
            break;
        if (!czl_analysis_ele_buf(gp, tmp, arr->vars+arr->cnt))
        {
            if (size > CZL_FILE_BUF_SIZE)
                CZL_TMP_FREE(gp, buf, size);
            goto CZL_OOM;
        }
        if (size > CZL_FILE_BUF_SIZE)
            CZL_TMP_FREE(gp, buf, size);
    } while (++arr->cnt < sum);

    ret->type = CZL_ARRAY;
    return 1;

CZL_OOM:
    czl_array_delete(gp, ret->val.Obj);
    return 0;
}

char czl_struct_read(czl_gp *gp, FILE *fp, czl_var *ret, long sum)
{
    unsigned long len;
    unsigned long file_size;
    char buf[CZL_FILE_BUF_SIZE];
    char *tmp = buf;

    if (1 == sum)
        return czl_obj_read(gp, fp, ret);
    else if (sum > 0)
        return czl_objs_read(gp, fp, ret, sum);

    if (!ftell(fp))
    {
        unsigned long check_sum;
        if (!fread(&check_sum, 4, 1, fp) || check_sum != czl_check_sum)
            return 0;
    }

    file_size = czl_get_file_size(fp) - ftell(fp);
    if (file_size+2 > CZL_FILE_BUF_SIZE && !(tmp=(char*)CZL_TMP_MALLOC(gp, file_size+2)))
        return 0;

    if ((len=fread(tmp, 1, file_size, fp)) <= 4)
    {
        if (file_size+2 > CZL_FILE_BUF_SIZE)
            CZL_TMP_FREE(gp, buf, file_size+2);
        return 0;
    }
    tmp[len] = tmp[len+1] = CZL_NIL;

    if (!(ret->val.Obj=czl_analysis_obj_buf(gp, tmp)))
    {
        if (file_size+2 > CZL_FILE_BUF_SIZE)
            CZL_TMP_FREE(gp, buf, file_size+2);
        return 0;
    }

    if (file_size+2 > CZL_FILE_BUF_SIZE)
        CZL_TMP_FREE(gp, buf, file_size+2);
    ret->type = CZL_ARRAY;
    return 1;
}

char czl_line_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
    czl_str str = {NULL, 0};
    unsigned char flag = 1;

    do {
        unsigned long len;
        char buf[CZL_FILE_BUF_SIZE];
        if (!fgets(buf, CZL_FILE_BUF_SIZE, fp))
        {
            if (str.Obj)
                CZL_SF(gp, str);
            return 0;
        }
        len = strlen(buf);
        if ('\n' == buf[len-1])
        {
            flag = 0;
            buf[--len] = '\0';
        }
        if (str.Obj)
        {
            czl_string *s;
            void **obj = CZL_SR(gp, str, len+str.size);
            if (!obj)
            {
                CZL_SF(gp, str);
                return 0;
            }
            str.Obj = obj;
            s = CZL_STR(obj);
            memcpy(s->str + s->len, buf, len);
            s->len += len;
            str.size += len;
        }
        else if (!czl_str_create(gp, &str, len+1, len, buf))
            return 0;
    } while (flag);

    ret->type = CZL_STRING;
    ret->val.str = str;
    return 1;
}

char czl_lines_read(czl_gp *gp, FILE *fp, czl_var *ret, long sum)
{
    czl_array *arr;

    if (1 == sum)
        return czl_line_read(gp, fp, ret);

    if (!(ret->val.Obj=czl_array_create(gp, 2, 0)))
        return 0;

    arr = CZL_ARR(ret->val.Obj);

    do {
        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            goto CZL_OOM;
        if (!czl_line_read(gp, fp, arr->vars+arr->cnt))
            break;
    } while (++arr->cnt < (unsigned long)sum || sum <= 0);

    ret->type = CZL_ARRAY;
    return 1;

CZL_OOM:
    czl_array_delete(gp, ret->val.Obj);
    return 0;
}

czl_long czl_get_file_size(FILE *fp)
{
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    struct stat buf;
    fstat(fileno(fp), &buf);
    return buf.st_size;
#else
    long size;
    long cur = ftell(fp);
    if (EOF == cur)
        return 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    return size;
#endif
}

char czl_bytes_read
(
    czl_gp *gp,
    FILE *fp,
#ifdef CZL_SYSTEM_WINDOWS
    unsigned char txt,
#endif
    czl_var *ret,
    long sum
)
{
    czl_str str;
    czl_string *s;
    long i = 0;

    if (1 == sum)
        return (EOF == (ret->val.inum=fgetc(fp)) ? 0 : 1);

    if (sum <= 0)
    {
        long cur = ftell(fp);
        if (EOF == cur)
            return 0;
        sum = czl_get_file_size(fp) - cur;
        if (0 == sum)
            return 0;
    }

    if (!czl_str_create(gp, &str, sum+1, 0, NULL))
        return 0;

    s = CZL_STR(str.Obj);

    while (i < sum)
    {
        s->str[i] = fgetc(fp);
    #ifdef CZL_SYSTEM_WINDOWS
        //windows下文本方式读系统会把文件里的换行符\r\n合并为\n读出，这样节省内存
        if ('\n' == s->str[i] && txt)
            --sum;
    #endif
        ++i;
    }

    s->len = i;
    s->str[i] = '\0';

    ret->type = CZL_STRING;
    ret->val.str = str;
    return 1;
}

char czl_sys_fread(czl_gp *gp, czl_fun *fun)
{
    czl_file *f = CZL_FIL(fun->vars->val.Obj);
    long cnt = fun->vars[1].val.inum;

    if (1 == f->mode)
    {
        if (!czl_bytes_read(gp, f->fp,
                            #ifdef CZL_SYSTEM_WINDOWS
                            f->txt,
                            #endif
                            &fun->ret, cnt))
            fun->ret.val.inum = 0;
    }
    else
    {
        if (fseek(f->fp, f->addr, SEEK_SET))
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        if (!(2 == f->mode ?
              czl_lines_read(gp, f->fp, &fun->ret, cnt) :
              czl_struct_read(gp, f->fp, &fun->ret, cnt)))
        {
            fun->ret.val.inum = 0;
        }
        f->addr = ftell(f->fp);
    }

    return 1;
}

char czl_sys_fseek(czl_gp *gp, czl_fun *fun)
{
    czl_file *f;
    czl_var *file = CZL_GCRV(fun->vars);
    long end, addr = fun->vars[1].val.inum;

    if (!file) return 0;

    f = CZL_FIL(file->val.Obj);
    if (f->rc > 1 && !(f=czl_file_fork(gp, file)))
        return 0;

    if (1 == f->mode)
        fun->ret.val.inum = !fseek(f->fp, addr, SEEK_SET);
    else
    {
        fseek(f->fp, 0, SEEK_END);
        end = ftell(f->fp);
        f->addr = (addr > end || addr < 0 ? end : addr);
        fun->ret.val.inum = 1;
    }

    return 1;
}

char czl_sys_ftell(czl_gp *gp, czl_fun *fun)
{
    czl_file *f = CZL_FIL(fun->vars->val.Obj);
    fun->ret.val.inum = (1 == f->mode ? ftell(f->fp) : f->addr);
    return 1;
}

char czl_sys_fprint(czl_gp *gp, czl_fun *fun)
{
    char ret;
    FILE *fout;
    czl_var *res;
    czl_para *p = (czl_para*)fun->vars;

    if (!(res=czl_exp_cac(gp, p->para)) || res->type != CZL_STRING)
        return 0;

    if (!(fout=fopen(CZL_STR(res->val.str.Obj)->str, "a")))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
    fun->ret.val.inum = 1;

    ret = czl_modify_print(gp, p->next, fout);

    fclose(fout);

    return ret;
}

char czl_sys_fclean(czl_gp *gp, czl_fun *fun)
{
    fclose(fopen(CZL_STR(fun->vars->val.str.Obj)->str, "w"));
    return 1;
}

char czl_sys_read(czl_gp *gp, czl_fun *fun)
{
    char *path = CZL_STR(fun->vars->val.str.Obj)->str;
    long size = (fun->vars[1].val.inum < 0 ? 40*1024 : fun->vars[1].val.inum);
    czl_buf_file *f = czl_sys_hash_find(CZL_STRING, CZL_NIL, path, &gp->file_hash);
    FILE *fp = (f ? f->fp : fopen(path, "rb"));
    czl_string *s;
    struct stat buf;

    if (!fp)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    fstat(fileno(fp), &buf);

    if (size < buf.st_size)
    {
        if (f && f->date == buf.st_mtime)
        {
            fun->ret.type = CZL_STRING;
            fun->ret.val.str.size = CZL_STR(f->buf)->len + 1;
            fun->ret.val.str.Obj = f->buf;
            f->buf = NULL;
        }
        else
        {
            if (f)
                rewind(fp);
            if (!czl_str_create(gp, &fun->ret.val.str, buf.st_size+1, buf.st_size, NULL))
                return 0;
            if (fread(CZL_STR(fun->ret.val.str.Obj)->str, 1, buf.st_size, fp))
                fun->ret.type = CZL_STRING;
            else
            {
                CZL_SF(gp, fun->ret.val.str);
                fun->ret.val.inum = 0;
            }
        }
        if (f)
            czl_buf_file_delete(gp, f);
        else
            fclose(fp);
        return 1;
    }

    if (f)
    {
        if (f->date == buf.st_mtime)
        {
            fun->ret.type = CZL_STRING;
            fun->ret.val.str.Obj = f->buf;
            ++CZL_STR(f->buf)->rc;
            return 1;
        }
        rewind(fp);
        if (0 == --CZL_STR(f->buf)->rc)
            CZL_STR_FREE(gp, f->buf, CZL_SL(CZL_STR(f->buf)->len+1));
    }
    else
    {
        if (!(f=(czl_buf_file*)CZL_BUF_FILE_MALLOC(gp)) ||
            !(f->path=(char*)CZL_TMP_MALLOC(gp, strlen(path)+1)) ||
            !czl_sys_hash_insert(gp, CZL_STRING, f, &gp->file_hash))
        {
            if (f)
            {
                CZL_TMP_FREE(gp, f->path, strlen(path)+1);
                CZL_BUF_FILE_FREE(gp, f);
            }
            fclose(fp);
            return 0;
        }
        strcpy(f->path, path);
        f->fp = fp;
        f->last = NULL;
        f->next = gp->file_head;
        if (gp->file_head)
            gp->file_head->last = f;
        gp->file_head = f;
    }

    f->date = buf.st_mtime;

    if (!(f->buf=(void**)CZL_STR_MALLOC(gp, CZL_SL(buf.st_size+1))))
    {
        czl_buf_file_delete(gp, f);
        return 0;
    }
    s = CZL_STR(f->buf);
    s->rc = 1;
    s->len = buf.st_size;
    s->str[s->len] = '\0';

    if (!fread(s->str, 1, buf.st_size, fp))
    {
        czl_buf_file_delete(gp, f);
        fun->ret.val.inum = 0;
    }
    else
    {
        fun->ret.type = CZL_STRING;
        fun->ret.val.str.Obj = f->buf;
        ++s->rc;
    }

    return 1;
}

char czl_sys_close(czl_gp *gp, czl_fun *fun)
{
    char *path = CZL_STR(fun->vars->val.str.Obj)->str;
    czl_buf_file *f = czl_sys_hash_find(CZL_STRING, CZL_NIL, path, &gp->file_hash);

    if (f)
    {
        czl_buf_file_delete(gp, f);
        fun->ret.val.inum = 1;
    }
    else
        fun->ret.val.inum = 0;

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

char czl_sys_rands(czl_gp *gp, czl_fun *fun)
{
    const char *s = "abcdefghijklm0123456789nopqrstuvwxyz0123456789ABCDEFGHIJKLM0123456789NOPQRSTUVWXYZ0123456789";
    static int inx = 0;
    unsigned long r = rand();
    unsigned long i, j = r%11 + 10, m = inx, n = 92/j;
    char res[20];

    r *= r;
    for (i = 0; i < j; ++i)
    {
        if (m >= 91) m = 0;
        res[i] = (r&0x01 ? s[m+1] : s[m]);
        r >>= 1;
        m += n;
    }
    inx = m;

    return czl_set_ret_str(gp, &fun->ret, res, j);
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

char czl_sys_num(czl_gp *gp, czl_fun *fun)
{
    switch (fun->vars->type)
    {
    case CZL_STRING:
        if ('\0' == *CZL_STR(fun->vars->val.str.Obj)->str ||
            !czl_get_number_from_str(CZL_STR(fun->vars->val.str.Obj)->str, &fun->ret))
            fun->ret.val.fnum = 0;
        break;
    case CZL_INT:
        fun->ret.val.inum = fun->vars->val.inum;
        break;
    case CZL_FLOAT:
        fun->ret.type = CZL_FLOAT;
        fun->ret.val.fnum = fun->vars->val.fnum;
        break;
    default:
        fun->ret.val.inum = 0;
        break;
    }

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

unsigned long czl_itoa32(long num, char *dst)
{
    char res[64];
    long tmp;
    unsigned long i = 0, j = 0;

    if (num < 0)
    {
        dst[0] = '-';
        num = -num;
        j = 1;
    }

    do {
        tmp = num;
        res[i++] = (char)(tmp - (num/=10)*10) + '0';
    } while (num);

    do {
        dst[j++] = res[--i];
    } while (i > 0);

    dst[j] = '\0';

    return j;
}

unsigned long czl_itoa64(czl_long num, char *dst)
{
    char res[64];
    czl_long tmp;
    unsigned long i = 0, j = 0;

    if (num < 0)
    {
        dst[0] = '-';
        num = -num;
        j = 1;
    }

    do {
        tmp = num;
        res[i++] = (char)(tmp - (num/=10)*10) + '0';
    } while (num);

    do {
        dst[j++] = res[--i];
    } while (i > 0);

    dst[j] = '\0';

    return j;
}

unsigned long czl_itoa(czl_long num, char *dst)
{
    if (num >= -2147483647 && num <= 2147483647)
        return czl_itoa32(num, dst);
    else
        return czl_itoa64(num, dst);
}

int czl_0f_cnt(double num)
{
    if (num >= 0.1) return 1;
    else if (num >= 0.01) return 2;
    else if (num >= 0.001) return 3;
    else if (num >= 0.0001) return 4;
    else if (num >= 0.00001) return 5;
    else if (num >= 0.000001) return 6;
    else if (num >= 0.0000001) return 7;
    else if (num >= 0.00000001) return 8;
    else if (num >= 0.000000001) return 9;
    else if (num >= 0.0000000001) return 10;
    else if (num >= 0.00000000001) return 11;
    else if (num >= 0.000000000001) return 12;
    else if (num >= 0.0000000000001) return 13;
    else if (num >= 0.00000000000001) return 14;
    else if (num >= 0.000000000000001) return 15;
    else if (num >= 0.0000000000000001) return 16;
    else if (num >= 0.00000000000000001) return 17;
    else return 18;
}

unsigned long czl_ftoa(double num, char *dst, int _NumOfDec)
{
    unsigned long i = 0;
    int j, dec, sig;
    char *s;

    if (_NumOfDec > 20)
        _NumOfDec = 20;

    s = fcvt(num, _NumOfDec, &dec, &sig);

    if (sig)
        dst[i++] = '-';

    if (num < 0)
        num = -num;
    if (num >= 1)
    {
        for (j = 0; j < dec; ++j)
            dst[i++] = s[j];
        dst[i++] = '.';
    }
    else
    {
        int k = czl_0f_cnt(num);
        dst[i++] = '0';
        dst[i++] = '.';
        for (j = 1; j < k; ++j)
            dst[i++] = '0';
        j = 0;
    }

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
    char value[32];
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
    char value[32];
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
    char value[160];
    char *tmp = value;

    for (;;)
    {
        if (*s >= '0' && *s <= '1')
            *(tmp++) = *(s++);
        else
            break;
        if (tmp - value == 159)
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

char* czl_numstr_ignore_sign_filt(char *code)
{
    while (' ' == *code || '\t' == *code || '\n' == *code || '\r' == *code)
        ++code;

    return code;
}

char* czl_get_number_from_str(char *s, czl_var *res)
{
    char mark = 1;
    char radix = 0;

    s = czl_numstr_ignore_sign_filt(s);

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
    char tmp[64];
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

    s = CZL_STR(str->val.str.Obj)->str;
    sl = CZL_STR(str->val.str.Obj)->len;
    f = CZL_STR(sign->val.str.Obj)->str;
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
            if (++arr->cnt > arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            {
                czl_array_delete(gp, obj);
                return 0;
            }
            if (!czl_str_create(gp, &arr->vars[arr->cnt-1].val.str, len+1, len, s+k-len))
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

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;
}

char czl_str_toul(czl_gp *gp, czl_fun *fun, char flag)
{
	char d;
    char *ps;
    czl_var *str = CZL_GCRV(fun->vars);
    long i = fun->vars[1].val.inum;
    long j = fun->vars[2].val.inum;
    czl_string *s;

    if (!str) return 0;

    s = CZL_STR(str->val.str.Obj);

    if (i < 0 || (unsigned long)i >= s->len || i > (j = (j >= 0 ? j : (long)s->len-1)))
        return 1;

    if (s->rc > 1 && !(s=czl_string_fork(gp, str)))
        return 0;

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

char czl_sys_memset(czl_gp *gp, czl_fun *fun)
{
    czl_var *str = CZL_GCRV(fun->vars);
    long sign = fun->vars[1].val.inum;
    long i = fun->vars[2].val.inum;
    long j = fun->vars[3].val.inum;
    czl_string *s;

    if (!str) return 0;

    s = CZL_STR(str->val.str.Obj);

    if (i < 0 || (unsigned long)i >= s->len || i > (j = (j >= 0 ? j : (long)s->len-1)))
        return 1;

    if (s->rc > 1 && !(s=czl_string_fork(gp, str)))
        return 0;

    memset(s->str+i, sign, j-i+1);

    return 1;
}

char czl_sys_memget(czl_gp *gp, czl_fun *fun)
{
    czl_var *str = fun->vars;
    long i = fun->vars[1].val.inum;
    long j = fun->vars[2].val.inum;
    czl_string *s = CZL_STR(str->val.str.Obj);

    if (i < 0 || (unsigned long)i >= s->len || i > (j = (j >= 0 ? j : (long)s->len-1)))
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    return czl_set_ret_str(gp, &fun->ret, s->str+i, j-i+1);
}

char czl_sys_memcmp(czl_gp *gp, czl_fun *fun)
{
    czl_string *a = CZL_STR(fun->vars[0].val.str.Obj);
    czl_string *b = CZL_STR(fun->vars[1].val.str.Obj);
    long i = fun->vars[2].val.inum;
    long size = fun->vars[3].val.inum;

    if (i < 0)
    {
        fun->ret.val.inum = memcmp(a->str, b->str, (a->len < b->len ? a->len : b->len));
        if (!fun->ret.val.inum && a->len != b->len)
            fun->ret.val.inum = (a->len < b->len ? -1 : 1);
    }
    else if (size <= 0)
    {
        fun->ret.val.inum = memcmp(a->str+i, b->str+i,
                                   (a->len < b->len ? a->len : b->len));
        if (!fun->ret.val.inum && a->len != b->len)
            fun->ret.val.inum = (a->len < b->len ? -1 : 1);
    }
    else
    {
        fun->ret.val.inum = memcmp(a->str+i, b->str+i, size);
    }

    return 1;
}

long czl_memspn
(
    const char *a,
    const char *b,
    const unsigned long al,
    const unsigned long bl
)
{
    unsigned long i = 0, j, k;

    while (i < al)
    {
        k = i;
        for (j = 0; j < bl && i < al && a[i] == b[j]; ++j)
            ++i;
        if (j == bl)
            return k;
        i = k+1;
    }

    return -1;
}

char czl_sys_memspn(czl_gp *gp, czl_fun *fun)
{
    czl_string *a = CZL_STR(fun->vars[0].val.str.Obj);
    czl_string *b = CZL_STR(fun->vars[1].val.str.Obj);
    long i = fun->vars[2].val.inum;
    long j = fun->vars[3].val.inum;

    if (i < 0 || (unsigned long)i >= a->len || i > (j = (j >= 0 ? j : (long)a->len-1)) ||
        0 == b->len)
    {
        fun->ret.val.inum = -1;
        return 1;
    }

    j = czl_memspn(a->str+i, b->str, j-i+1, b->len);
    fun->ret.val.inum = (j >= 0 ? i+j : -1);
    return 1;
}

char czl_sys_memrep(czl_gp *gp, czl_fun *fun)
{
    czl_var *str = CZL_GCRV(fun->vars);
    czl_string *a = CZL_STR(fun->vars[1].val.str.Obj);
    czl_string *b = CZL_STR(fun->vars[2].val.str.Obj);
    long i = fun->vars[3].val.inum;
    long j = fun->vars[4].val.inum;
    long k, end;
    unsigned long len;
    czl_string *s, *res;
    czl_str tmp;

    if (!str) return 0;

    s = CZL_STR(str->val.str.Obj);
    if (i < 0 || (unsigned long)i >= s->len || i > (j = (j >= 0 ? j : (long)s->len-1)) ||
        0 == a->len)
    {
        fun->ret.val.inum = -1;
        return 1;
    }

    end = j-i+1;
    k = czl_memspn(s->str+i, a->str, end, a->len);
    if (k < 0)
    {
        fun->ret.val.inum = -1;
        return 1;
    }

    k += i;
    fun->ret.val.inum = k;

    if (a->len >= b->len)
        len = s->len - a->len + b->len;
    else
        len = (b->len/a->len + (b->len%a->len ? 1:0)) * s->len;

    if (!czl_str_create(gp, &tmp, len+1, 0, NULL))
        return 0;
    res = CZL_STR(tmp.Obj);

    for (i = j = 0; k >= 0 && end > i; k = czl_memspn(s->str+i, a->str, end-i, a->len))
    {
        memcpy(res->str+j, s->str+i, k);
        memcpy(res->str+j+k, b->str, b->len);
        i += k+a->len;
        j += k+b->len;
    }
    memcpy(res->str+j, s->str+i, s->len-i);
    res->len = j + s->len - i;
    res->str[res->len] = '\0';

    CZL_SRCD1(gp, str->val.str);
    str->val.str = tmp;
    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_abort(czl_gp *gp, czl_fun *fun)
{
    if (CZL_EIT(fun->vars))
        exit(0);

    return 0;
}

char czl_sys_assert(czl_gp *gp, czl_fun *fun)
{
    if (CZL_EIT(fun->vars))
        return 1;

    gp->exit_code = CZL_EXIT_ASSERT;
    return 0;
}

char czl_sys_errLine(czl_gp *gp, czl_fun *fun)
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

char czl_sys_setFun(czl_gp *gp, czl_fun *fun)
{
    czl_fun *callbackFun = fun->vars[0].val.fun;
    long exceptionCode = fun->vars[1].val.inum;

    if (callbackFun->enter_vars_cnt)
        return 1;

    if (0 == exceptionCode)
    {
        unsigned long i;
        for (i = 0; i < CZL_EXCEPTION_CODE_NUM; ++i)
            gp->exceptionFuns[i] = callbackFun;
    }
    else if (exceptionCode >= 1 && exceptionCode <= CZL_EXCEPTION_CODE_NUM)
        gp->exceptionFuns[exceptionCode-1] = callbackFun;
#ifdef CZL_MULT_THREAD
    else if (-1 == exceptionCode)
        gp->killFun = callbackFun;
#endif //#ifdef CZL_MULT_THREAD

    return 1;
}

#ifdef CZL_SYSTEM_LINUX
long czl_linux_clock(void)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec*1000 + time.tv_usec/1000;
}
#endif //#ifdef CZL_SYSTEM_LINUX

#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_sleep(czl_gp *gp, czl_fun* fun)
{
    CZL_SLEEP(fun->vars->val.inum);
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

#ifdef CZL_TIMER
#ifdef CZL_SYSTEM_WINDOWS
void WINAPI czl_timer_event(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dwl, DWORD dw2)
#else //CZL_SYSTEM_LINUX
void czl_timer_event(union sigval sv)
#endif
{
    czl_timer *t;
#ifdef CZL_SYSTEM_WINDOWS
    czl_gp *gp = ((czl_gp*)dwUser);
#else //CZL_SYSTEM_LINUX
    czl_timer *uTimerID = sv.sival_ptr;
    czl_gp *gp = uTimerID->gp;
#endif

    czl_timer_lock(gp);

    if ((t=czl_sys_hash_find(
                         #ifdef CZL_SYSTEM_WINDOWS
                             CZL_NIL,
                         #else //CZL_SYSTEM_LINUX
                             CZL_INT,
                         #endif
                             CZL_NIL, (void*)uTimerID, &gp->timers_hash)) && 0 == t->state)
    {
        //将有事件的定时器放到链表头
        if (t->last)
        {
            t->last->next = t->next;
            if (t->next)
                t->next->last = t->last;
            t->last = NULL;
            t->next = gp->timers_head;
            gp->timers_head = gp->timers_head->last = t;
        }
        t->state = 1;
        ++gp->timerEventCnt;
    }

    czl_timer_unlock(gp);
}

char czl_sys_timer(czl_gp *gp, czl_fun *fun)
{
    long period = fun->vars->val.inum;
    czl_fun *cb_fun = fun->vars[1].val.fun;
    czl_timer *p;

#ifdef CZL_SYSTEM_WINDOWS
    unsigned long timerId;
#else //CZL_SYSTEM_LINUX
    timer_t timerId;
#endif

    fun->ret.val.inum = 0;

    if (period <= 0 || cb_fun->enter_vars_cnt > 1)
        return 1;

    if (!(p=CZL_TIMER_MALLOC(gp)))
        return 0;

#ifdef CZL_SYSTEM_WINDOWS
    timerId = timeSetEvent(period, 1, czl_timer_event, gp, TIME_PERIODIC);
#else //CZL_SYSTEM_LINUX
    struct sigevent se;
    memset(&se, 0, sizeof(struct sigevent));
    se.sigev_value.sival_ptr = p;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = czl_timer_event;
    if (timer_create(CLOCK_REALTIME, &se, &timerId) == -1)
    {
        CZL_TIMER_FREE(gp, p);
        return 1;
    }
    struct itimerspec it; //统一转为毫秒
    unsigned long sec = period/1000;
    unsigned long nsec = (period - sec*1000) * 1000*1000;
    it.it_interval.tv_sec = sec;
    it.it_interval.tv_nsec = nsec;
    it.it_value.tv_sec = sec; //延迟启动定时器时间
    it.it_value.tv_nsec = nsec;
    if (timer_settime(timerId, 0, &it, NULL) == -1)
    {
        CZL_TIMER_FREE(gp, p);
        timer_delete(timerId);
        return 1;
    }
#endif

    if (!(fun->ret.val.inum=czl_timer_create(gp, p, timerId,
                                         #ifdef CZL_SYSTEM_WINDOWS
                                             timeKillEvent,
                                         #else //CZL_SYSTEM_LINUX
                                             timer_delete,
                                         #endif
                                             cb_fun)))
        return 0;

    return 1;
}
#endif //#ifdef CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)

char czl_sys_date(czl_gp *gp, czl_fun *fun)
{
    char *modify = CZL_STR(fun->vars->val.str.Obj)->str;
    time_t now;
    struct tm *tm_now;
    char date[128];
    unsigned long len = 0;
    unsigned short year, tmp;

    time(&now);
    tm_now = localtime(&now);

    //Y-M-D h:m:s
    while (*modify && len < 128)
    {
        switch (*modify)
        {
        case 'Y':
            year = tm_now->tm_year+1900-2000;
            date[len++] = '2';
            date[len++] = year/100 + '0';
            date[len++] = (tmp=year/10) + '0';
            date[len++] = year-tmp*10 + '0';
            break;
        case 'M':
            if (tm_now->tm_mon+1 < 10) {
                date[len++] = '0';
                date[len++] = tm_now->tm_mon+1 + '0';
            } else {
                date[len++] = '1';
                date[len++] = tm_now->tm_mon+1 - 10 + '0';
            }
            break;
        case 'D':
            if (tm_now->tm_mday < 10) {
                date[len++] = '0';
                date[len++] = tm_now->tm_mday + '0';
            } else if (tm_now->tm_mday < 20) {
                date[len++] = '1';
                date[len++] = tm_now->tm_mday-10 + '0';
            } else if (tm_now->tm_mday < 30) {
                date[len++] = '2';
                date[len++] = tm_now->tm_mday-20 + '0';
            } else {
                date[len++] = '3';
                date[len++] = tm_now->tm_mday-30 + '0';
            }
            break;
        case 'h':
            if (tm_now->tm_hour < 10) {
                date[len++] = '0';
                date[len++] = tm_now->tm_hour + '0';
            } else if (tm_now->tm_hour < 20) {
                date[len++] = '1';
                date[len++] = tm_now->tm_hour-10 + '0';
            } else {
                date[len++] = '2';
                date[len++] = tm_now->tm_hour-20 + '0';
            }
            break;
        case 'm':
            if (tm_now->tm_min < 10) {
                date[len++] = '0';
                date[len++] = tm_now->tm_min + '0';
            } else if (tm_now->tm_min < 20) {
                date[len++] = '1';
                date[len++] = tm_now->tm_min-10 + '0';
            } else if (tm_now->tm_min < 30) {
                date[len++] = '2';
                date[len++] = tm_now->tm_min-20 + '0';
            } else if (tm_now->tm_min < 40) {
                date[len++] = '3';
                date[len++] = tm_now->tm_min-30 + '0';
            } else if (tm_now->tm_min < 50) {
                date[len++] = '4';
                date[len++] = tm_now->tm_min-40 + '0';
            } else {
                date[len++] = '5';
                date[len++] = tm_now->tm_min-50 + '0';
            }
            break;
        case 's':
            if (tm_now->tm_sec < 10) {
                date[len++] = '0';
                date[len++] = tm_now->tm_sec + '0';
            } else if (tm_now->tm_sec < 20) {
                date[len++] = '1';
                date[len++] = tm_now->tm_sec-10 + '0';
            } else if (tm_now->tm_sec < 30) {
                date[len++] = '2';
                date[len++] = tm_now->tm_sec-20 + '0';
            } else if (tm_now->tm_sec < 40) {
                date[len++] = '3';
                date[len++] = tm_now->tm_sec-30 + '0';
            } else if (tm_now->tm_sec < 50) {
                date[len++] = '4';
                date[len++] = tm_now->tm_sec-40 + '0';
            } else {
                date[len++] = '5';
                date[len++] = tm_now->tm_sec-50 + '0';
            }
            break;
        default:
            if ('\\' == *modify &&
                ('Y' == *(modify+1) || 'M' == *(modify+1) || 'D' == *(modify+1) ||
                 'h' == *(modify+1) || 'm' == *(modify+1) || 's' == *(modify+1)))
                ++modify;
            date[len++] = *modify;
            break;
        }
        ++modify;
    }

    date[len] = '\0';

    return czl_set_ret_str(gp, &fun->ret, date, len);
}

char czl_sys_useMem(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->mm_cnt;
    return 1;
}

char czl_sys_maxMem(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->mm_max;
    return 1;
}

char czl_sys_setMem(czl_gp *gp, czl_fun *fun)
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
char czl_sys_setCache(czl_gp *gp, czl_fun *fun)
{
    gp->mm_cache_size = fun->vars->val.inum;
    return 1;
}
#endif //#ifdef CZL_MM_CACHE

char czl_sys_setRank(czl_gp *gp, czl_fun *fun)
{
    if (fun->vars->val.inum >= 0 && gp->mmp_rank <= CZL_MM_SP_HEAP_NUM_MAX)
        gp->mmp_rank = fun->vars->val.inum;
    return 1;
}

char czl_sys_setSa(czl_gp *gp, czl_fun *fun)
{
    gp->mmp_selfAdapt = fun->vars->val.inum;
    return 1;
}

char czl_sys_setGc(czl_gp *gp, czl_fun *fun)
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
    a->mmp_ref = b->mmp_ref;
    a->mmp_file = b->mmp_file;
    #ifdef CZL_MM_CACHE
        a->mm_cache_size = b->mm_cache_size;
        a->mm_cache = b->mm_cache;
    #endif //#ifdef CZL_MM_CACHE
    a->mmp_sh_head = b->mmp_sh_head;
    a->mmp_gc_size = b->mmp_gc_size;
    a->mmp_rank = b->mmp_rank;
    a->mmp_selfAdapt = b->mmp_selfAdapt;
#endif //#ifdef CZL_MM_MODULE
    a->mm_limit_backup = b->mm_limit_backup;
    a->mm_limit = b->mm_limit;
    a->mm_cnt = b->mm_cnt;
    a->mm_max = b->mm_max;
#ifdef CZL_TIMER
    #ifdef CZL_SYSTEM_WINDOWS
        a->timer_cs = b->timer_cs;
    #elif defined CZL_SYSTEM_LINUX
        a->timer_mutex = b->timer_mutex;
    #endif
#endif //#ifdef CZL_TIMER
#ifndef CZL_CONSOLE
    a->table = b->table;
    a->log_path = b->log_path;
    a->pclass = b->pclass;
#endif //#ifndef CZL_CONSOLE
    a->ch_head = b->ch_head;
    a->fun_deep = b->fun_deep;
#if defined CZL_SYSTEM_LINUX && (defined CZL_LIB_COM || \
                                 defined CZL_LIB_TCP || defined CZL_LIB_UDP)
    a->kdpfd = b->kdpfd;
#endif
}

char czl_sys_runShell(czl_gp *gp, czl_fun *fun)
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
char czl_key_cmp(const czl_tabkv *a, const czl_tabkv *b)
{
    if (CZL_INT == a->type)
    {
        if (CZL_INT == b->type)
        {
            if (a->key.inum == b->key.inum) return 0;
            else if (a->key.inum < b->key.inum) return -1;
            else return 1;
        }
        else
            return -1;
    }
    else
    {
        if (CZL_STRING == b->type)
            return strcmp(CZL_STR(a->key.str.Obj)->str, CZL_STR(b->key.str.Obj)->str);
        else
            return 1;
    }
}

char czl_val_cmp(czl_gp *gp, czl_var *a, czl_var *b)
{
    if (gp->cur_fun)
        return czl_sort_cmp_fun_ret(gp, a, b);

    switch (a->type)
    {
    case CZL_INT:
        switch (b->type)
        {
        case CZL_INT:
            if (a->val.inum == b->val.inum) return 0;
            else if (a->val.inum < b->val.inum) return -1;
            else return 1;
        case CZL_FLOAT:
            if (a->val.inum == b->val.fnum) return 0;
            else if (a->val.inum < b->val.fnum) return -1;
            else return 1;
        default:
            return -1;
        }
    case CZL_FLOAT:
        switch (b->type)
        {
        case CZL_FLOAT:
            if (a->val.fnum == b->val.fnum) return 0;
            else if (a->val.fnum < b->val.fnum) return -1;
            else return 1;
        case CZL_INT:
            if (a->val.fnum == b->val.inum) return 0;
            else if (a->val.fnum < b->val.inum) return -1;
            else return 1;
        default:
            return -1;
        }
    case CZL_STRING:
        switch (b->type)
        {
        case CZL_STRING:
            return strcmp(CZL_STR(a->val.str.Obj)->str, CZL_STR(b->val.str.Obj)->str);
        case CZL_INT: case CZL_FLOAT:
            return 1;
        default:
            return -1;
        }
    default:
        return 1;
    }
}

unsigned long czl_partition_0(czl_gp *gp, czl_var *a, unsigned long rh)
{
    unsigned long lh = 1;
    czl_value tmp;

    for (;;)
    {
        while (!gp->error_flag && lh < rh && czl_val_cmp(gp, a+rh, a) >= 0) --rh;
        while (!gp->error_flag && lh < rh && czl_val_cmp(gp, a+lh, a) < 0) ++lh;
        if (gp->error_flag)
            return 0;
        if (lh == rh)
            break;
        CZL_VAL_SWAP(a[lh].val, a[rh].val, tmp);
        if (a[lh].type != a[rh].type)
            CZL_INT_SWAP(a[lh].type, a[rh].type);
    }

    if (czl_val_cmp(gp, a+lh, a) >= 0)
        return 0;
    if (gp->error_flag)
        return 0;

    CZL_VAL_SWAP(a->val, a[lh].val, tmp);
    if (a->type != a[lh].type)
        CZL_INT_SWAP(a->type, a[lh].type);

    return lh;
}

unsigned long czl_partition_1(czl_gp *gp, czl_var *a, unsigned long rh)
{
    unsigned long lh = 1;
    czl_value tmp;

    for (;;)
    {
        while (!gp->error_flag && lh < rh && czl_val_cmp(gp, a+rh, a) < 0) --rh;
        while (!gp->error_flag && lh < rh && czl_val_cmp(gp, a+lh, a) >= 0) ++lh;
        if (gp->error_flag)
            return 0;
        if (lh == rh)
            break;
        CZL_VAL_SWAP(a[lh].val, a[rh].val, tmp);
        if (a[lh].type != a[rh].type)
            CZL_INT_SWAP(a[lh].type, a[rh].type);
    }

    if (czl_val_cmp(gp, a+lh, a) < 0)
        return 0;
    if (gp->error_flag)
        return 0;

    CZL_VAL_SWAP(a->val, a[lh].val, tmp);
    if (a->type != a[lh].type)
        CZL_INT_SWAP(a->type, a[lh].type);

    return lh;
}

void czl_quick_sort_0(czl_gp *gp, czl_var *a, unsigned long n)
{
    long b;
    if (n < 2)
        return;
    b = czl_partition_0(gp, a, n-1);
    if (!gp->error_flag)
    {
        czl_quick_sort_0(gp, a, b);
        if (!gp->error_flag)
           czl_quick_sort_0(gp, a+b+1, n-b-1);
    }
}

void czl_quick_sort_1(czl_gp *gp, czl_var *a, unsigned long n)
{
    long b;
    if (n < 2)
        return;
    b = czl_partition_1(gp, a, n-1);
    if (!gp->error_flag)
    {
        czl_quick_sort_1(gp, a, b);
        if (!gp->error_flag)
           czl_quick_sort_1(gp, a+b+1, n-b-1);
    }
}

czl_tabkv* czl_merge(czl_gp *gp, czl_tabkv *first, czl_tabkv *second, unsigned char mode)
{
    czl_tabkv combined;
    czl_tabkv *last_sorted = &combined;

    while (first && second)
    {
        char ret;
        switch (mode)
        {
        case 0:
            ret = czl_val_cmp(gp, (czl_var*)first, (czl_var*)second);
            if (gp->error_flag)
                mode = 4;
            break;
        case 1:
            ret = (czl_val_cmp(gp, (czl_var*)first, (czl_var*)second) < 0 ? 1 : -1);
            if (gp->error_flag)
                mode = 4;
            break;
        case 2:
            ret = czl_key_cmp(first, second);
            break;
        case 3:
            ret = (czl_key_cmp(first, second) < 0 ? 1 : -1);
            break;
        default:
            ret = -1;
            break;
        }
        if (ret < 0)
        {
            last_sorted->next = first;
            last_sorted = first;
            first = first->next;
        }
        else
        {
            last_sorted->next = second;
            last_sorted = second;
            second = second->next;
        }
    }

    if (!first)
        last_sorted->next = second;
    else
        last_sorted->next = first;

    return combined.next;
}

czl_tabkv* czl_divide_from(czl_tabkv *sub_list)
{
    czl_tabkv *position, *midpoint, *second_half;

    midpoint = sub_list;
    position = midpoint->next;
    while (position)
    {
        position = position->next;
        if (position)
        {
            midpoint = midpoint->next;
            position = position->next;
        }
    }
    second_half = midpoint->next;
    midpoint->next = NULL;

    return second_half;
}

void czl_recursive_merge_sort(czl_gp *gp, czl_tabkv **sub_list, unsigned char mode)
{
    if (*sub_list && (*sub_list)->next)
    {
        czl_tabkv *second_half = czl_divide_from(*sub_list);
        czl_recursive_merge_sort(gp, sub_list, mode);
        if (!gp->error_flag)
            czl_recursive_merge_sort(gp, &second_half, mode);
        *sub_list = czl_merge(gp, *sub_list, second_half, (gp->error_flag ? 4 : mode));
    }
}

void czl_fix_tabkv_ptr(czl_table *tab)
{
    czl_tabkv *p, *q = NULL;
    for (p = tab->eles_head; p; p = p->next)
    {
        p->last = q;
        q = p;
    }
    tab->eles_tail = q;
}

char czl_strcmp(const char *a, const char *b)
{
    while ((*a == *b || *a == *b+32 || *b == *a+32) && *a)
    {
        ++a; ++b;
    }
    return (*a || *b ? 0 : 1);
}

char czl_sys_sort(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = CZL_GRV(fun->vars);
    char *flag = CZL_STR(fun->vars[1].val.str.Obj)->str;
    unsigned char mode = 0;
    unsigned char quality = obj->quality;

    if (CZL_FUN_REF == fun->vars[2].type)
    {
        czl_fun *cmp_fun = fun->vars[2].val.fun;
        if (cmp_fun->enter_vars_cnt != 2)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        gp->ef2.fun = cmp_fun;
        gp->cur_fun = (czl_fun*)&gp->ef2;
        CZL_LOCK_OBJ(obj); //防止在排序函数删除中删除了obj
    }
    else
        gp->cur_fun = NULL;

    gp->error_flag = 0;

    if (czl_strcmp("asc", flag))
        mode = 0;
    else if (czl_strcmp("desc", flag))
        mode = 1;
    else if (czl_strcmp("kasc", flag))
        mode = 2;
    else if (czl_strcmp("kdesc", flag))
        mode = 3;

    if (CZL_ARRAY == obj->type)
    {
        czl_array *arr = CZL_ARR(obj->val.Obj);
        if (arr->rc > 1 && !(arr=czl_array_fork(gp, obj)))
            return 0;
        if (mode)
            czl_quick_sort_1(gp, arr->vars, arr->cnt);
        else
            czl_quick_sort_0(gp, arr->vars, arr->cnt);
    }
    else if (CZL_TABLE == obj->type)
    {
        czl_table *tab = CZL_TAB(obj->val.Obj);
        if (tab->rc > 1 && !(tab=czl_table_fork(gp, obj)))
            return 0;
        czl_recursive_merge_sort(gp, &tab->eles_head, mode);
        czl_fix_tabkv_ptr(tab);
    }

    CZL_UNLOCK_OBJ(obj, quality);
    fun->ret.val.inum = 1;

    return !gp->error_flag;
}
///////////////////////////////////////////////////////////////
char czl_sys_toBin(czl_gp *gp, czl_fun *fun)
{
    unsigned long obj_size = 4;

    if (!czl_sizeof_obj(gp, 1, fun->vars, &obj_size) ||
        !czl_set_ret_str(gp, &fun->ret, NULL, obj_size))
        return 0;

    memcpy(CZL_STR(fun->ret.val.str.Obj)->str, &czl_check_sum, 4);
    czl_get_obj_buf(gp, fun->vars, CZL_STR(fun->ret.val.str.Obj)->str+4);

    return 1;
}

char czl_sys_toObj(czl_gp *gp, czl_fun *fun)
{
    czl_string *bin = CZL_STR(fun->vars->val.str.Obj);

    if (bin->len <= 4 || memcmp(&czl_check_sum, bin->str, 4) ||
        !czl_analysis_ele_buf(gp, bin->str+4, &fun->ret))
        fun->ret.val.inum = 0;

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

    if (!ref) return 0;

    stack = CZL_SQ(ref->val.Obj);
    if (stack->rc > 1 && !(stack=czl_sq_fork(gp, ref)))
        return 0;

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
    if (sq->rc > 1 && !(sq=czl_sq_fork(gp, ref)))
        return 0;

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

    if (!ref) return 0;

    queue = CZL_SQ(ref->val.Obj);
    if (queue->rc > 1 && !(queue=czl_sq_fork(gp, ref)))
        return 0;

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

    if (!ref) return 0;

    tab = CZL_TAB(ref->val.Obj);
    if (tab->rc > 1 && !(tab=czl_table_fork(gp, ref)))
        return 0;

    p = czl_find_tabkv(tab, fun->vars+1);
    if (!(q=czl_create_tabkv(gp, tab, fun->vars+2, p, 0)))
        return 0;

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

    if (!fun || CZL_SYS_FUN == fun->type || !fun->yeild_flag)
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
        czl_sys_hash_delete(gp, CZL_INT, obj, &gp->coroutines_hash);
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
    fun->ret.val.inum = (unsigned long)czl_coroutine_create(gp, fun->vars->val.fun,
                                                            fun->vars[1].val.inum);
    return 1;
}

char czl_sys_reset(czl_gp *gp, czl_fun *fun)
{
    if (CZL_FUN_REF == fun->vars->type)
    {
        czl_fun *f = fun->vars->val.fun;
        if (CZL_SYS_FUN == f->type || !f->yeild_flag || CZL_IN_BUSY == f->state)
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
        void **obj = czl_sys_hash_find(CZL_INT, CZL_NIL, (char*)id, &gp->coroutines_hash);
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

char czl_sys_corsta(czl_gp *gp, czl_fun *fun)
{
    unsigned long id = (unsigned long)fun->vars->val.inum;
    if (czl_sys_hash_find(CZL_INT, CZL_NIL, (char*)id, &gp->coroutines_hash))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
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
    {
    #ifdef CZL_MULT_THREAD
        czl_thread *t = (czl_thread*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                                       (char*)id, &gp->threads_hash);
        if (t)
        {
            czl_event_send(&t->pipe->notify_event);
            fun->ret.val.inum = 1;
        }
    #endif //#ifdef CZL_MULT_THREAD
        return 1;
    }

    c = CZL_COR(obj);

    if ((unsigned long)c->fun->enter_vars_cnt < fun->paras_cnt-1)
        return 1;

    if (!(ret=czl_coroutine_run(gp, p->next, fun->paras_cnt-1, obj)))
        return 0;

    fun->ret.type = ret->type;
    fun->ret.val = ret->val;
    ret->type = CZL_INT;

    return gp->yeild_end ? 1 : 2;
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
#ifdef CZL_TIMER
    else if (czl_timer_delete(gp, id))
    {
        fun->ret.val.inum = 1;
    }
#endif //#ifdef CZL_TIMER
    else
    {
    #ifdef CZL_MULT_THREAD
        czl_thread *t = (czl_thread*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                                       (char*)id, &gp->threads_hash);
        if (!t)
            fun->ret.val.inum = 0;
        else
        {
            extern void czl_thread_delete(czl_gp*, czl_thread*);
            fun->ret.val.inum = 1;
            t->pipe->alive = 0;
            t->pipe->kill = 1;
            czl_event_send(&t->pipe->notify_event);
            t->pipe = NULL;
            czl_thread_delete(gp, t);
        }
    #else
        fun->ret.val.inum = 0;
    #endif //#ifdef CZL_MULT_THREAD
    }

    return 1;
}
///////////////////////////////////////////////////////////////
#if (defined CZL_LIB_TCP || defined CZL_LIB_UDP)
char czl_sys_dns(czl_gp *gp, czl_fun *fun)
{
    char *domain = CZL_STR(fun->vars[0].val.str.Obj)->str;
    struct hostent *host;
    char **list;
    void **obj;
    czl_var *vars;
    unsigned long cnt = 0;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if (!(host=gethostbyname(domain)))
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    list = host->h_addr_list;
    while (*list++) ++cnt;

    if (!(obj=czl_array_create(gp, cnt, cnt)))
        return 0;

    vars = CZL_ARR(obj)->vars;
    for (list = host->h_addr_list; *list; ++list)
    {
        char *ip = inet_ntoa(*(struct in_addr*)list[0]);
        unsigned long len = strlen(ip);
        if (!czl_str_create(gp, &vars->val.str, len+1, len, ip))
        {
            czl_array_delete(gp, obj);
            return 0;
        }
        (vars++)->type = CZL_STRING;
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;
}
#else
char czl_sys_dns(czl_gp *gp, czl_fun *fun)
{
    return 1;
}
#endif
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

    czl_sys_hash_delete(gp, CZL_INT, p, &gp->threads_hash);

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
        goto CZL_OOM;

    if (!czl_event_init(&pipe->notify_event) || !czl_event_init(&pipe->report_event))
    {
        free(pipe->para);
        goto CZL_OOM;
    }

    czl_lock_init(&pipe->report_lock);
    pipe->alive = 1;
    return pipe;

CZL_OOM:
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
        goto CZL_OOM;
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
        goto CZL_OOM;
    }

    czl_exec_shell(gp, shell_path, 0);
    czl_memory_free(gp);
    free(gp);

CZL_OOM:
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

    fun->ret.val.inum = 0;

    if (!(p=czl_thread_create(gp)))
        return 1;

    if (!(p->pipe=czl_thread_pipe_create(gp, fun->vars+1)))
        goto CZL_OOM;

    if (!(argv=(void**)calloc(3, sizeof(void*))))
        goto CZL_OOM;

    argv[0] = p->pipe;

    if (!(shell_path=(char*)malloc(CZL_STR(fun->vars->val.str.Obj)->len+1)))
        goto CZL_OOM;
    memcpy(shell_path,
           CZL_STR(fun->vars->val.str.Obj)->str,
           CZL_STR(fun->vars->val.str.Obj)->len);
    shell_path[CZL_STR(fun->vars->val.str.Obj)->len] = '\0';
    argv[1] = shell_path;

#ifndef CZL_CONSOLE
    if (CZL_STR(fun->vars[2].val.str.Obj)->len)
    {
        if (!(log_path=(char*)malloc(CZL_STR(fun->vars[2].val.str.Obj)->len+1)))
            goto CZL_OOM;
        memcpy(log_path,
               CZL_STR(fun->vars[2].val.str.Obj)->str,
               CZL_STR(fun->vars[2].val.str.Obj)->len);
        log_path[CZL_STR(fun->vars[2].val.str.Obj)->len] = '\0';
    }
    argv[2] = log_path;
#endif //#ifndef CZL_CONSOLE

#ifdef CZL_SYSTEM_WINDOWS
    if (!(p->id=CreateThread(NULL, 0, czl_run_thread, argv, 0, NULL)))
        goto CZL_OOM;
#elif defined CZL_SYSTEM_LINUX
    if (pthread_create(&p->id, NULL, czl_run_thread, argv))
        goto CZL_OOM;
#endif

    fun->ret.val.inum = (unsigned long)p;
    return 1;

CZL_OOM:
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

char czl_sys_notifyAll(czl_gp *gp, czl_fun *fun)
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

char czl_sys_suspend(czl_gp *gp, czl_fun *fun)
{
    czl_thread *t = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL);
    if (!t)
        fun->ret.val.inum = 0;
    else
    {
        t->pipe->suspend = 1;
        fun->ret.val.inum = 1;
    }
    return 1;
}
#endif //#ifdef CZL_MULT_THREAD
///////////////////////////////////////////////////////////////
#ifndef CZL_CONSOLE
char czl_sys_cleanLog(czl_gp *gp, czl_fun *fun)
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

char czl_sys_openLog(czl_gp *gp, czl_fun *fun)
{
    free(gp->log_path);
    if (!(gp->log_path=malloc(CZL_STR(fun->vars->val.str.Obj)->len+1)))
        return 0;
    strcpy(gp->log_path, CZL_STR(fun->vars->val.str.Obj)->str);
    return 1;
}

char czl_sys_closeLog(czl_gp *gp, czl_fun *fun)
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
        goto CZL_OOM;

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
                goto CZL_OOM;
            tmp += 4;
            break;
        default:
            break;
        }
    }

    return 1;

CZL_OOM:
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

    if (!gp->table || !(table=czl_table_create(gp, 1, 0, 0)))
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
        goto CZL_OOM;
    strcpy(gp->shell_path, shell_path);

    if (log_path)
    {
       if (!(gp->log_path=malloc(strlen(log_path)+1)))
           goto CZL_OOM;
       strcpy(gp->log_path, log_path);
    }
    else
        gp->log_path = NULL;

    if (class_name)
    {
       if (!(gp->class_name=malloc(strlen(class_name)+1)))
           goto CZL_OOM;
       strcpy(gp->class_name, class_name);
    }
    else
        gp->class_name = NULL;

    if (!czl_sys_init(gp))
    {
        czl_init_free(gp, 1);
        goto CZL_OOM;
    }
    else if (!czl_exec_prepare(gp))
    {
        czl_memory_free(gp);
        goto CZL_OOM;
    }

    return gp;

CZL_OOM:
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

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
char czl_sys_setsign(czl_gp*, czl_fun*);
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
char czl_sys_ston(czl_gp*, czl_fun*);
char czl_sys_ntos(czl_gp*, czl_fun*);
char czl_sys_split(czl_gp*, czl_fun*);
char czl_sys_upper(czl_gp*, czl_fun*);
char czl_sys_lower(czl_gp*, czl_fun*);
//
char czl_sys_abort(czl_gp*, czl_fun*);
char czl_sys_assert(czl_gp*, czl_fun*);
char czl_sys_errline(czl_gp*, czl_fun*);
char czl_sys_errfile(czl_gp*, czl_fun*);
char czl_sys_errcode(czl_gp*, czl_fun*);
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_sleep(czl_gp*, czl_fun*);
char czl_sys_clock(czl_gp*, czl_fun*);
#ifdef CZL_TIMER
char czl_sys_settimer(czl_gp*, czl_fun*);
char czl_sys_gettimer(czl_gp*, czl_fun*);
char czl_sys_stoptimer(czl_gp*, czl_fun*);
#endif //defined CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
char czl_sys_ltime(czl_gp*, czl_fun*);
char czl_sys_gc(czl_gp*, czl_fun*);
char czl_sys_usemem(czl_gp*, czl_fun*);
char czl_sys_maxmem(czl_gp*, czl_fun*);
char czl_sys_setmem(czl_gp*, czl_fun*);
char czl_sys_runshl(czl_gp*, czl_fun*);
//
char czl_sys_hcac(czl_gp*, czl_fun*);
char czl_sys_hdod(czl_gp*, czl_fun*);
//
char czl_sys_tp(czl_gp*, czl_fun*);
char czl_sys_ot(czl_gp*, czl_fun*);
char czl_sys_sz(czl_gp*, czl_fun*);
char czl_sys_ct(czl_gp*, czl_fun*);
char czl_sys_rc(czl_gp*, czl_fun*);
//
char czl_sys_push(czl_gp*, czl_fun*);
char czl_sys_pop(czl_gp*, czl_fun*);
char czl_sys_in(czl_gp*, czl_fun*);
char czl_sys_out(czl_gp*, czl_fun*);
char czl_sys_at(czl_gp*, czl_fun*);
char czl_sys_ins(czl_gp*, czl_fun*);
char czl_sys_del(czl_gp*, czl_fun*);
//
char czl_sys_newcor(czl_gp*, czl_fun*);
char czl_sys_resume(czl_gp*, czl_fun*);
char czl_sys_corsta(czl_gp*, czl_fun*);
char czl_sys_reset(czl_gp*, czl_fun*);
char czl_sys_kill(czl_gp*, czl_fun*);
//
#ifdef CZL_MULT_THREAD
char czl_sys_newshl(czl_gp*, czl_fun*);
char czl_sys_wait(czl_gp*, czl_fun*);
char czl_sys_listen(czl_gp*, czl_fun*);
char czl_sys_waitshl(czl_gp*, czl_fun*);
char czl_sys_report(czl_gp*, czl_fun*);
char czl_sys_send(czl_gp*, czl_fun*);
char czl_sys_notify(czl_gp*, czl_fun*);
char czl_sys_notifyall(czl_gp*, czl_fun*);
char czl_sys_shlsta(czl_gp*, czl_fun*);
#endif //#ifdef CZL_MULT_THREAD
//
#ifndef CZL_CONSOLE
char czl_sys_cleanlog(czl_gp*, czl_fun*);
char czl_sys_openlog(czl_gp*, czl_fun*);
char czl_sys_closelog(czl_gp*, czl_fun*);
char czl_sys_pushi(czl_gp*, czl_fun*);
char czl_sys_pushf(czl_gp*, czl_fun*);
char czl_sys_pushs(czl_gp*, czl_fun*);
char czl_sys_pop_data(czl_gp*, czl_fun*);
char czl_sys_size(czl_gp*, czl_fun*);
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
    {"open",      czl_sys_open,       4,  "&v1,str_v2,str_v3=\"rb+\",int_v4=1"},
    {"close",     czl_sys_close,      1,  "&fil_v1"},
    {"write",     czl_sys_write,      -2, NULL},
    {"read",      czl_sys_read,       2,  "&fil_v1,&v2"},
    {"setsign",   czl_sys_setsign,    2,  "&fil_v1,int_v2"},
    //随机数函数
    {"srand",     czl_sys_srand,      1,  "int_v1"},
    {"rand",      czl_sys_rand,       0,  NULL},
    {"rrand",     czl_sys_rrand,      2,  "int_v1,int_v2"},
    //数值处理函数
    {"Int",       czl_sys_int,        1,  "int_v1"},
    {"Float",     czl_sys_float,      1,  "float_v1"},
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
    {"ston",      czl_sys_ston,       1,  "str_v1"},
    {"ntos",      czl_sys_ntos,       2,  "num_v1,int_v2=6"},
    {"split",     czl_sys_split,      2,  "&str_v1,str_v2=\" \""},
    {"upper",     czl_sys_upper,      3,  "&str_v1,int_v2=0,int_v3=-1"},
    {"lower",     czl_sys_lower,      3,  "&str_v1,int_v2=0,int_v3=-1"},
    /*
     * 格式化(format)，
     * 子串查找(find)，子串提取(sub)，子串替换(swap)，子串插入(ins)
    */
    //系统函数
    {"abort",     czl_sys_abort,      0,  NULL},
    {"assert",    czl_sys_assert,     1,  NULL},
    {"errline",   czl_sys_errline,    0,  NULL},
    {"errfile",   czl_sys_errfile,    0,  NULL},
    {"errcode",   czl_sys_errcode,    0,  NULL},
#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"sleep",     czl_sys_sleep,      1,  "int_v1"},
    {"clock",     czl_sys_clock,      0,  NULL},
    #ifdef CZL_TIMER
    {"settimer",  czl_sys_settimer,   4,  "int_v1,fun_v2,int_v3=0,int_v4=0"},
    {"gettimer",  czl_sys_gettimer,   0,  NULL},
    {"stoptimer", czl_sys_stoptimer,  0,  NULL},
    #endif //CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    {"ltime",     czl_sys_ltime,      1,  "str_v1=\"Y-M-D h:m:s\""},
#ifdef CZL_MM_MODULE
    {"gc",        czl_sys_gc,         0,  NULL},
#endif //#ifdef CZL_MM_MODULE
    {"usemem",    czl_sys_usemem,     0,  NULL},
    {"maxmem",    czl_sys_maxmem,     0,  NULL},
    {"setmem",    czl_sys_setmem,     1,  "int_v1"},
    {"runshl",    czl_sys_runshl,     3,  "str_v1,&v2,v3=0"},
    //哈希表内建函数
    {"hcac",      czl_sys_hcac,       1,  "&tab_v1"},
    {"hdod",      czl_sys_hdod,       3,  "&tab_v1,int_v2,int_v3"},
    //变量内建函数
    {"tp",        czl_sys_tp,         1,  NULL},
    {"ot",        czl_sys_ot,         1,  "&v1"},
    {"sz",        czl_sys_sz,         1,  NULL},
    {"ct",        czl_sys_ct,         1,  NULL},
    {"rc",        czl_sys_rc,         1,  "&v1"},
    //复合类型变量内建函数
    {"push",      czl_sys_push,       2,  "&sta_v1"},
    {"pop",       czl_sys_pop,        1,  "&sta_v1"},
    {"in",        czl_sys_in,         2,  "&que_v1"},
    {"out",       czl_sys_out,        1,  "&que_v1"},
    {"at",        czl_sys_at,         2,  "&lis_v1,int_v2"},
    {"ins",       czl_sys_ins,        3,  "&lis_v1,int_v2=-1"},
    {"del",       czl_sys_del,        2,  "&lis_v1,int_v2=-1"},
    //协成框架接口函数
    {"newcor",    czl_sys_newcor,     1,  "fun_v1"},
    {"resume",    czl_sys_resume,     -1, NULL},
    {"corsta",    czl_sys_corsta,     1,  "int_v1"},
    {"reset",     czl_sys_reset,      1,  NULL},
    {"kill",      czl_sys_kill,       1,  "int_v1"},
#ifdef CZL_MULT_THREAD
    //多线程框架接口函数
    #ifdef CZL_CONSOLE
    {"newshl",    czl_sys_newshl,     2,  "str_v1,v2=0"},
    #else
    {"newshl",    czl_sys_newshl,     3,  "str_v1,v2=0,str_v3=\"\""},
    #endif
    {"wait",      czl_sys_wait,       1,  "int_v1=0"},
    {"listen",    czl_sys_listen,     1,  "int_v1"},
    {"waitshl",   czl_sys_waitshl,    1,  "int_v1"},
    {"report",    czl_sys_report,     1,  NULL},
    {"send",      czl_sys_send,       1,  NULL},
    {"notify",    czl_sys_notify,     2,  "int_v1,v2=0"},
    {"notifyall", czl_sys_notifyall,  1,  "v1=0"},
    {"shlsta",    czl_sys_shlsta,     1,  "int_v1"},
#endif //#ifdef CZL_MULT_THREAD
#ifndef CZL_CONSOLE
    //与C/C++栈交互接口函数
    {"cleanlog",  czl_sys_cleanlog,   0,  NULL},
    {"openlog",   czl_sys_openlog,    1,  "str_v1"},
    {"closelog",  czl_sys_closelog,   0,  NULL},
    {"czl_pushi", czl_sys_pushi,      1,  "int_v1"},
    {"czl_pushf", czl_sys_pushf,      1,  "float_v1"},
    {"czl_pushs", czl_sys_pushs,      1,  "str_v1"},
    {"czl_pop",   czl_sys_pop_data,   1,  "&v1"},
    {"czl_size",  czl_sys_size,       0,  NULL},
    {"czl_clean", czl_sys_clean,      0,  NULL},
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
    czl_fun *fun,
    const char *str,
    unsigned long len
)
{
    if (!czl_str_create(gp, &fun->ret.val.str, len+1, len, str))
        return 0;
    fun->ret.type = CZL_STRING;

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
    case 'x': case 'b': case 'X': case 'B':
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
    czl_ins **parents = ins->parents;
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
        czl_print_ins(gp, *parents++, fout);
    }
}

void czl_print_tab(czl_gp *gp, const czl_table *tab, FILE *fout)
{
	czl_tabkv *p;

    fprintf(fout, "{");

    for (p = tab->eles_head; p; p = p->next)
    {
        if (CZL_STRING == p->kt)
            fprintf(fout, "\"%s\":", p->key.str.s->str);
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

void czl_print_sql(czl_gp *gp, const czl_sql *sql, FILE *fout)
{
    czl_glo_var *ele;

    fprintf(fout, "<");
    for (ele = sql->eles_head; ele; ele = ele->next)
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

char czl_print_tab_list(czl_gp *gp, const czl_table_list *ele, FILE *fout)
{
    czl_var *ret, tmp;

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
        czl_print_obj(gp, &tmp, fout);
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
        fprintf(fout, "\"%s\"", obj->val.str.s->str);
        break;
    case CZL_INSTANCE:
        czl_print_ins(gp, (czl_ins*)obj->val.obj, fout);
        break;
    case CZL_TABLE:
        czl_print_tab(gp, (czl_table*)obj->val.obj, fout);
        break;
    case CZL_ARRAY:
        czl_print_arr(gp, (czl_array*)obj->val.obj, fout);
        break;
    case CZL_STACK: case CZL_QUEUE: case CZL_LIST:
        czl_print_sql(gp, (czl_sql*)obj->val.obj, fout);
        break;
    case CZL_FUN_REF:
        fprintf(fout, "%d", (int)obj->val.obj);
        break;
    case CZL_FILE:
        fprintf(fout, "%d[%d]", (int)obj->val.file.fp, obj->val.file.mode);
        break;
    case CZL_ARRAY_LIST:
        return czl_print_arr_list(gp, (czl_array_list*)obj->val.obj, fout);
    case CZL_TABLE_LIST:
        return czl_print_tab_list(gp, (czl_table_list*)obj->val.obj, fout);
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
        czl_print_str_hex(res->val.str.s, modify_type, fout);
        return 1;
    case 'b': case 'B':
        czl_print_str_bin(res->val.str.s, modify_type, fout);
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
        //多线程控制台工作方式下print递归会导致死锁
        czl_print_lock();
    #endif
    fout = stdout;
    fun->ret.val.inum = 1;
#else
    if (!gp->log_path[0])
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
        if (!(res=czl_exp_cac(gp, p->para)) ||
            !czl_print_obj(gp, res, fout))
        {
            ret = 0;
            break;
        }
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
    if (!gp->log_path[0])
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
            for (s = str.s->str; *s; s++)
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

    return czl_set_ret_str(gp, fun, tmp, len);
}
#endif //CZL_CONSOLE
///////////////////////////////////////////////////////////////
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
	czl_class_parent *p;
	czl_ins **parents;

    if (!ins)
        return;

	var = CZL_GIV(ins);
	for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        czl_sizeof_obj(gp, flag, var++, sum);

    parents = ins->parents;
    for (p = ins->pclass->parents; p; p = p->next)
        czl_sizeof_ins(gp, flag, *parents++, sum);
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
            *sum += (flag ? p->key.str.s->len + 9 : p->key.str.s->len);
        else if (flag)
            *sum += sizeof(czl_long) + 1;
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

void czl_sizeof_sql
(
    czl_gp *gp,
    char flag,
    const czl_sql *sql,
    unsigned long *sum
)
{
    czl_glo_var *ele;

    for (ele = sql->eles_head; ele; ele = ele->next)
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
    const czl_table_list *ele,
    unsigned long *sum
)
{
    czl_var *ret;

    while (ele)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return 0;
        switch (ret->type)
        {
        case CZL_STRING:
            *sum += (flag ? ret->val.str.s->len + 9 :
                            ret->val.str.s->len);
            break;
        case CZL_INT:
            *sum += sizeof(czl_long) + 1;
            break;
        case CZL_FLOAT:
            *sum += sizeof(czl_long) + 1;
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
        *sum += flag ? sizeof(czl_long)+1 : sizeof(czl_long);
        return 1;
    case CZL_FLOAT:
        *sum += flag ? sizeof(double)+1 : sizeof(double);
        return 1;
    case CZL_STRING:
        *sum += flag ? obj->val.str.s->len+5 : obj->val.str.s->len;
        return 1;
    case CZL_FUN_REF:
        *sum += flag ? 5 : 4;
        return  1;
    case CZL_FILE:
        *sum += flag ? sizeof(czl_file)+1 : sizeof(czl_file);
        return 1;
    case CZL_INSTANCE:
        if (flag)
            *sum += (2+strlen(((czl_ins*)obj->val.obj)->pclass->name));
        czl_sizeof_ins(gp, flag, (czl_ins*)obj->val.obj, sum);
        return 1;
    case CZL_TABLE:
        czl_sizeof_tab(gp, flag, (czl_table*)obj->val.obj, sum);
        if (flag)
            *sum += 14;
        return 1;
    case CZL_ARRAY:
        czl_sizeof_arr(gp, flag, (czl_array*)obj->val.obj, sum);
        if (flag)
            *sum += 6;
        return 1;
    case CZL_STACK: case CZL_QUEUE: case CZL_LIST:
        czl_sizeof_sql(gp, flag, (czl_sql*)obj->val.obj, sum);
        if (flag)
            *sum += 2;
        return 1;
    case CZL_ARRAY_LIST:
        if (!czl_sizeof_arr_list(gp, flag, (czl_array_list*)obj->val.obj, sum))
            return 0;
        if (flag)
            *sum += 6;
        return 1;
    case CZL_TABLE_LIST:
        if (!czl_sizeof_tab_list(gp, flag, (czl_table_list*)obj->val.obj, sum))
            return 0;
        if (flag)
            *sum += 14;
        return 1;
    default:
        return 1;
    }
}
///////////////////////////////////////////////////////////////
long czl_get_file_size(FILE *fp)
{
	long size;
    long cur = ftell(fp);
    if (EOF == cur)
        return EOF;
    if (fseek(fp, 0, SEEK_END))
        return EOF;

    size = ftell(fp);
    if (EOF == size)
        return EOF;
    if (fseek(fp, cur, SEEK_SET))
        return EOF;

    return size;
}

char czl_sys_open(czl_gp *gp, czl_fun *fun)
{
    czl_var *file = CZL_GRV(fun->vars);
	czl_var *path = fun->vars + 1;
    czl_var *mode1 = fun->vars + 2;
    czl_var *mode2 = fun->vars + 3;

    if (file->type != CZL_FILE)
    {
        if (!czl_ret_clean(gp, file))
            return 0;
        file->type = CZL_FILE;
    }
    file->val.file.sign = ' ';

    fun->ret.val.inum = 0;

    if (mode2->val.inum != 1 && mode2->val.inum != 2)
        return 1;

    //wb+ 覆盖， rb+ 追加
    if (1 == mode2->val.inum &&
        strcmp("wb+", mode1->val.str.s->str) &&
        strcmp("rb+", mode1->val.str.s->str))
        return 1;
    if (!(file->val.file.fp = fopen(path->val.str.s->str,
                                    mode1->val.str.s->str)))
        return 1;

    file->val.file.mode = (unsigned char)mode2->val.inum;

    fun->ret.val.inum = 1;
    return 1;
}

char czl_sys_close(czl_gp *gp, czl_fun *fun)
{
    czl_var *file = CZL_GCRV(fun->vars);
    fun->ret.val.inum = (fclose(file->val.file.fp) == EOF ? 0 : 1);
    return 1;
}

char* czl_get_ins_buf(czl_gp *gp, czl_ins *ins, char *buf)
{
	czl_var *var;
	unsigned long i, j;
	czl_ins **parents;
    czl_class_parent *p;

    if (!ins)
        return buf;

    var = CZL_GIV(ins);
	for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        buf = czl_get_obj_buf(gp, var++, buf);

	parents = ins->parents;
    for (p = ins->pclass->parents; p; p = p->next)
        buf = czl_get_ins_buf(gp, *parents++, buf);

    return buf;
}

char* czl_get_tab_buf(czl_gp *gp, czl_table *tab, char *buf)
{
	czl_tabkv *p;
	unsigned long len;
    
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
            buf += 4;
            len = p->key.str.s->len;
            *((unsigned long*)buf) = len;
            memcpy(buf+4, p->key.str.s->str, len);
            buf += len+4;
        }
        else
        {
            *((czl_long*)buf) = p->key.inum;
            buf += sizeof(czl_long);
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

char* czl_get_sql_buf(czl_gp *gp, czl_sql *sql, char *buf)
{
    czl_glo_var *ele;

    if (0 == sql->count)
        return buf-1;

    for (ele = sql->eles_head; ele; ele = ele->next)
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

char* czl_get_tab_list_buf(czl_gp *gp, czl_table_list *head, char *buf)
{
	unsigned long key;
    unsigned long cnt = 0;
	unsigned long len;
	czl_var *ret;
    czl_table_list *ele;

    for (ele = head; ele; ele = ele->next)
        cnt++;
    *((unsigned long*)buf) = cnt;
    buf += 4;

    key = czl_hash_key_create((unsigned long)gp->mm_cnt, 0);
    *((unsigned long*)buf) = key;
    buf += 4;

    *((unsigned long*)buf) = 0;
    buf += 4;

    for (ele = head; ele; ele = ele->next)
    {
        if (!(ret=czl_exp_cac(gp, ele->key)))
            return NULL;
        *((char*)buf++) = ret->type;
        switch (ret->type)
        {
        case CZL_STRING:
            len = ret->val.str.s->len;
            *((unsigned long*)buf) =
                    czl_str_hash(ret->val.str.s->str, len, key, 0);
            buf += 4;
            *((unsigned long*)buf) = len;
            memcpy(buf+4, ret->val.str.s->str, len);
            buf += len+4;
            break;
        case CZL_INT:
            *((czl_long*)buf) = ret->val.inum;
            buf += sizeof(czl_long);
            break;
        case CZL_FLOAT:
            *((czl_long*)buf) = (czl_long)ret->val.fnum;
            buf += sizeof(czl_long);
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
    unsigned char len;

    *((char*)buf++) = obj->type;

    switch (obj->type)
    {
    case CZL_INT:
        *((czl_long*)buf) = obj->val.inum;
        return buf+sizeof(czl_long);
    case CZL_FLOAT:
        *((double*)buf) = obj->val.fnum;
        return buf+sizeof(double);
    case CZL_STRING:
        *((unsigned long*)buf) = obj->val.str.s->len;
        memcpy(buf+4, obj->val.str.s->str, obj->val.str.s->len);
        return buf+4+obj->val.str.s->len;
    case CZL_INSTANCE:
        len = strlen(((czl_ins*)obj->val.obj)->pclass->name);
        *((unsigned char*)buf++) = len;
        memcpy(buf, ((czl_ins*)obj->val.obj)->pclass->name, len);
        return czl_get_ins_buf(gp, (czl_ins*)obj->val.obj, buf+len);
    case CZL_TABLE:
        return czl_get_tab_buf(gp, (czl_table*)obj->val.obj, buf);
    case CZL_ARRAY:
        return czl_get_arr_buf(gp, (czl_array*)obj->val.obj, buf);
    case CZL_STACK: case CZL_QUEUE: case CZL_LIST:
        return czl_get_sql_buf(gp, (czl_sql*)obj->val.obj, buf);
    case CZL_FUN_REF:
        memcpy(buf, obj->val.obj, 4);
        return buf+4;
    case CZL_FILE:
        memcpy(buf, obj->val.file.fp, 4);
        *((int*)buf+4) = (int)obj->val.file.mode;
        return buf+sizeof(czl_file);
    case CZL_ARRAY_LIST:
        *(buf-1) = CZL_ARRAY;
        return czl_get_arr_list_buf(gp, (czl_array_list*)obj->val.obj, buf);
    case CZL_TABLE_LIST:
        *(buf-1) = CZL_TABLE;
        return czl_get_tab_list_buf(gp, (czl_table_list*)obj->val.obj, buf);
    default:
        return NULL;
    }
}

long czl_get_struct_file_size(FILE *fp)
{
    //结构化文件结尾信息： size:1234
	char buf[16];
	long tmp, file_size;

    if (fseek(fp, 0, SEEK_END))
        return EOF;
    tmp = ftell(fp);
    if (EOF == tmp)
        return EOF;
    if (0 == tmp)
        return 0;
    else if (tmp <= 9)
        return EOF;
    if (fseek(fp, -9, SEEK_END))
        return EOF;
    if (!fread(buf, 9, 1, fp))
        return EOF;
    if (memcmp("size:", buf, 5))
        return EOF;
    file_size = *((long*)(buf+5));
    if (file_size <= 0)
        return EOF;

    return file_size;
}

char czl_struct_write(czl_gp *gp, FILE *fp, czl_para *p)
{
	czl_var *res;
    unsigned long sum;
	char *buf;
	char ret = 1;
	char tmp[16] = "size:";

    long file_size = czl_get_struct_file_size(fp);
    if (EOF == file_size)
        return 1;
    if (file_size > 0 && fseek(fp, -9, SEEK_END))
        return 1;

    while (p)
    {
        if (!(res=czl_exp_cac(gp, p->para)))
        {
            ret = 0;
            break;
        }
        sum = 0;
        if (!czl_sizeof_obj(gp, 1, res, &sum) ||
            sum + file_size > CZL_STRUCT_FILE_MAX_SZIE ||
            !(buf=(char*)CZL_TMP_MALLOC(gp, sum)))
        {
            ret = 0;
            break;
        }
        czl_get_obj_buf(gp, res, buf);
        if (!fwrite(buf, sum, 1, fp))
        {
            CZL_TMP_FREE(gp, buf, sum);
            ret = 0;
            break;
        }
        CZL_TMP_FREE(gp, buf, sum);
        file_size += sum;
        p = p->next;
    }

    *((long*)(tmp+5)) = file_size;
    if (!fwrite(tmp, 9, 1, fp))
        return 0;

    return ret;
}

char czl_line_write(czl_gp *gp, FILE *fp, char sign, czl_para *p)
{
    char flag[2];
    char tmp[128];
    char *item;
    czl_var *ret;

    flag[1] = '\0';

    while (p)
    {
        if (!(ret=czl_exp_cac(gp, p->para)))
            return 0;
        switch (ret->type)
        {
        case CZL_STRING:
            item = ret->val.str.s->str;
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
            return 0;
        }
        if (EOF == fputs(item, fp))
            return 0;
        flag[0] = p->next ? sign : '\n';
        if (EOF == fputs(flag, fp))
            return 0;
        p = p->next;
    }

    return 1;
}

char czl_sys_write(czl_gp *gp, czl_fun *fun)
{
    czl_var *ret;
	czl_var file;
	czl_para* paras = (czl_para*)fun->vars;

    if (!(ret=czl_exp_cac(gp, paras->para)))
        return 0;
    if (CZL_OBJ_REF == ret->type)
    {
        czl_var *ref = CZL_GRV(ret);
        file.type = ref->type;
        file.val = ref->val;
    }
    else
    {
        file.type = ret->type;
        file.val = ret->val;
    }

    if (file.type != CZL_FILE)
        return 0;

    fun->ret.val.inum = (1 == file.val.file.mode ?
                         czl_struct_write(gp, file.val.file.fp,
                                          paras->next) :
                         czl_line_write(gp, file.val.file.fp,
                                        file.val.file.sign,
                                        paras->next));

    return 1;
}

char* czl_analysis_ins(czl_gp *gp, char *buf, czl_ins *ins)
{
	czl_var *var;
	unsigned long i, j;
	czl_ins **parents;
	czl_class_parent *p;

    if (!ins)
        return buf;

    var = CZL_GIV(ins);
    for (i = 0, j = ins->pclass->vars_count; i < j; i++)
        if (!(buf=czl_analysis_ele_buf(gp, buf, var++)))
            return NULL;

    parents = ins->parents;
    for (p = ins->pclass->parents; p; p = p->next)
        if (!(buf=czl_analysis_ins(gp, buf, *parents++)))
            return NULL;

    return buf;
}

char* czl_analysis_ins_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_class *pclass;
    czl_ins *ins = NULL;
    char name[CZL_NAME_MAX_SIZE];
    unsigned long len = *((unsigned char*)buf++);

    memcpy(name, buf, len);
    name[len] = '\0';

    if (!(pclass=czl_class_find(gp, name)))
        return NULL;

    if (!czl_instance_fork(gp, &ins, pclass, 0))
        return NULL;

    obj->type = CZL_INSTANCE;
    obj->val.obj = ins;

    return czl_analysis_ins(gp, buf+len, ins);
}

char* czl_analysis_tab_buf(czl_gp *gp, char *buf, czl_var *obj)
{
	char kt;
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

    if (!(tab=czl_table_create(gp, key, attack_cnt)))
        return NULL;

    if (!czl_hash_init(gp, tab, len))
    {
        czl_table_delete(gp, tab);
        return NULL;
    }

    obj->type = CZL_TABLE;
    obj->val.obj = tab;

    while ((kt=*buf++) != CZL_NIL)
    {
        if (CZL_STRING == kt)
        {
            hash = *((unsigned long*)buf);
            buf += 4;
            len = *((unsigned long*)buf);
            if (!(ele=czl_create_str_tabkv(gp, tab, hash, len, buf+4)))
                return NULL;
            buf += len+4;
        }
        else
        {
            if (!(ele=czl_create_num_tabkv(gp, tab, *((czl_long*)buf))))
                return NULL;
            buf += sizeof(czl_long);
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

    arr = czl_array_create(gp, *((unsigned long*)buf), 0);
    if (!arr)
        return NULL;

    buf += 4;
    obj->type = CZL_ARRAY;
    obj->val.obj = arr;

    while (*buf != CZL_NIL)
    {
        var = arr->vars + arr->cnt++;
        buf = czl_analysis_ele_buf(gp, buf, var);
        if (!buf)
            return NULL;
    }

    return buf+1;
}

char* czl_analysis_sql_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    czl_sql *sql = czl_sql_create(gp, 0);
    if (!sql)
    {
        obj->type = CZL_INT;
        return NULL;
    }

    obj->val.obj = sql;

    while (*buf != CZL_NIL)
    {
        czl_glo_var *ele = czl_sqlele_node_create(gp
                                                  #ifdef CZL_MM_MODULE
                                                  , &sql->pool
                                                  #endif
                                                  );
        if (!ele)
            return NULL;
        if (sql->eles_head)
            sql->eles_tail->next = ele;
        else
            sql->eles_head = ele;
        sql->eles_tail = ele;
        sql->count++;
        if (!(buf=czl_analysis_ele_buf(gp, buf, (czl_var*)ele)))
            return NULL;
    }

    return buf+1;
}

char* czl_analysis_ele_buf(czl_gp *gp, char *buf, czl_var *obj)
{
    unsigned long len;

    obj->type = *buf++;

    switch (obj->type)
    {
    case CZL_INT:
        obj->val.inum = *((czl_long*)buf);
        return buf+sizeof(czl_long);
    case CZL_FLOAT:
        obj->val.fnum = *((double*)buf);
        return buf+sizeof(double);
    case CZL_STRING:
        obj->type = CZL_INT;
        len = *((unsigned long*)buf);
        if (!czl_str_create(gp, &obj->val.str, len+1, len, buf+4))
            return 0;
        obj->type = CZL_STRING;
        memcpy(obj->val.str.s->str, buf+4, obj->val.str.s->len);
        return buf+4+len;
    case CZL_FUN_REF:
        memcpy(obj->val.obj, buf, 4);
        return buf+4;
    case CZL_FILE:
        memcpy(obj->val.file.fp, buf, 4);
        obj->val.file.mode = *((int*)buf+4);
        return buf+sizeof(czl_file);
    case CZL_INSTANCE:
        obj->type = CZL_INT;
        return czl_analysis_ins_buf(gp, buf, obj);
    case CZL_TABLE:
        obj->type = CZL_INT;
        return czl_analysis_tab_buf(gp, buf, obj);
    case CZL_ARRAY:
        obj->type = CZL_INT;
        return czl_analysis_arr_buf(gp, buf, obj);
    default: //CZL_STACK/CZL_QUEUE/CZL_LIST
        return czl_analysis_sql_buf(gp, buf, obj);
    }
}

czl_array* czl_analysis_obj_buf(czl_gp *gp, char *buf)
{
    czl_array *arr;
    const char end = (char)0xff;

    if (!(arr=czl_array_create(gp, 1, 0)))
        return NULL;

    while (*buf != end)
    {
        if (arr->cnt == arr->sum &&
            !czl_array_resize(gp, arr, arr->cnt))
        {
            czl_array_delete(gp, arr);
            return NULL;
        }
        if (!(buf=czl_analysis_ele_buf(gp, buf, arr->vars+arr->cnt++)))
        {
            czl_array_delete(gp, arr);
            return NULL;
        }
    }

    arr->vars = CZL_TMP_REALLOC(gp,
                                arr->vars,
                                arr->cnt*sizeof(czl_var),
                                arr->sum*sizeof(czl_var));
    arr->sum = arr->cnt;

    return arr;
}

char czl_struct_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
	long sum;
	char *buf;
	char tmp[16];
	czl_array *arr;

    if (fseek(fp, -9, SEEK_END))
    {
        if (fseek(fp, 0, SEEK_END) || ftell(fp))
            return 0;
        return 1;
    }

    if (!fread(tmp, 9, 1, fp))
        return 0;
    if (memcmp("size:", tmp, 5))
        return 0;
    sum = *((long*)(tmp+5));

    if (!(buf=(char*)CZL_TMP_MALLOC(gp, sum+1)))
        return 1;
    buf[sum] = (char)0xff; //注意不能等于czl_opr_type_enum中任何一个

    rewind(fp);
    if (!fread(buf, sum, 1, fp) || !(arr=czl_analysis_obj_buf(gp, buf)))
    {
        CZL_TMP_FREE(gp, buf, sum+1);
        return 0;
    }

    CZL_TMP_FREE(gp, buf, sum+1);
    ret->type = CZL_ARRAY;
    ret->val.obj = arr;

    return 1;
}

char czl_line_read(czl_gp *gp, FILE *fp, czl_var *ret)
{
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
        if (!(s=CZL_SR(gp, str, len+str.size)))
        {
            CZL_SF(gp, str);
            return 0;
        }
        strcpy(s->str + s->len, tmp);
        s->len += len;
        str.size += len;
        str.s = s;
    } while (flag);

    ret->type = CZL_STRING;
    ret->val.str = str;
    return 1;
}

char czl_sys_read(czl_gp *gp, czl_fun *fun)
{
	czl_var *ret;
    czl_var *file = CZL_GCRV(fun->vars);
    if (!file)
        return 0;

    ret = CZL_GRV(fun->vars+1);
    if (!czl_ret_clean(gp, ret))
        return 0;

    fun->ret.val.inum = (1 == file->val.file.mode ?
                         czl_struct_read(gp, file->val.file.fp, ret) :
                         czl_line_read(gp, file->val.file.fp, ret));

    return 1;
}

char czl_sys_setsign(czl_gp *gp, czl_fun *fun)
{
    czl_var *file = CZL_GCRV(fun->vars);
	czl_var *sign = fun->vars + 1;

    if (!file)
        return 0;

    file->val.file.sign = (unsigned char)sign->val.inum;

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
    fun->ret.val = fun->vars->val;
    return 1;
}

char czl_sys_float(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val = fun->vars->val;
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

char czl_sys_ston(czl_gp *gp, czl_fun *fun)
{
    if ('\0' == *fun->vars->val.str.s->str ||
        !czl_get_number_from_str(fun->vars->val.str.s->str, &fun->ret))
    {
        fun->ret.type = CZL_INT;
        fun->ret.val.inum = 0;
    }
    return 1;
}

char czl_sys_ntos(czl_gp *gp, czl_fun *fun)
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

    return czl_set_ret_str(gp, fun, tmp, len);
}

char czl_sys_split(czl_gp *gp, czl_fun *fun)
{
	char flag;
	char *s, *f;
	unsigned long sl, fl;
	unsigned long i = 0, j, k = 0, len = 0;
    czl_array *arr = NULL;
    czl_var *str = CZL_GCRV(fun->vars);
	czl_var *sign = fun->vars + 1;

    if (!str)
        return 0;
    
    s = str->val.str.s->str;
	f = sign->val.str.s->str;
    sl = str->val.str.s->len;
	fl = sign->val.str.s->len;

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
            if (!arr && !(arr=czl_array_create(gp, 1, 0)))
                return 0;
            if (++arr->cnt > arr->sum &&
                !czl_array_resize(gp, arr, arr->cnt))
            {
                czl_array_delete(gp, arr);
                return 0;
            }
            if (!czl_str_create(gp, &arr->vars[arr->cnt-1].val.str,
                                len+1, len, s+k-len))
            {
                czl_array_delete(gp, arr);
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

    if (!arr)
        return 1;

    arr->vars = CZL_TMP_REALLOC(gp, arr->vars,
                                arr->cnt*sizeof(czl_var),
                                arr->sum*sizeof(czl_var));
    arr->sum = arr->cnt;

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.obj = arr;
    return 1;
}

char czl_str_toul(czl_fun *fun, char flag)
{
	char d;
	char *s;
	unsigned long i, j;
    czl_var *str = CZL_GCRV(fun->vars);
	czl_var *begin = fun->vars + 1;
    czl_var *end = fun->vars + 2;

    if (!str)
        return 0;
    
    fun->ret.val.inum = 0;

    if (begin->val.inum < 0 ||
        (unsigned long)begin->val.inum >= str->val.str.s->len)
        return 1;

    i = (unsigned long)begin->val.inum;
    j = end->val.inum < 0 ? str->val.str.s->len-1 :
						    (unsigned long)end->val.inum;
    if (i > j) CZL_INT_SWAP(i, j);
    d = 'a' - 'A';
    s = str->val.str.s->str + i;
    while (i++ <= j)
    {
        if (flag)
        {
            if (*s >= 'a' && *s <= 'z')
                *s++ -= d;
        }
        else
        {
            if (*s >= 'A' && *s <= 'Z')
                *s++ += d;
        }
    }

    fun->ret.val.inum = 1;
    return 1;
}

char czl_sys_upper(czl_gp *gp, czl_fun *fun)
{
    return czl_str_toul(fun, 1);
}

char czl_sys_lower(czl_gp *gp, czl_fun *fun)
{
    return czl_str_toul(fun, 0);
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
            czl_set_ret_str(gp, fun, gp->error_file,
                            strlen(gp->error_file)) : 1);
}

char czl_sys_errcode(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->exceptionCode;
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
    return 1;
}

#ifdef CZL_TIMER
char czl_sys_settimer(czl_gp *gp, czl_fun *fun)
{
	czl_var *time;
	czl_var *cb_fun;
	czl_var *mode;
	czl_var *limit;

    fun->ret.val.inum = 0;

    if (gp->timer_flag)
        return 1;

    time = fun->vars;
    cb_fun = fun->vars + 1;
    mode = fun->vars + 2;
    limit = fun->vars + 3;

    if (limit->val.inum < 0 || !cb_fun->val.obj ||
        ((czl_fun*)cb_fun->val.obj)->enter_vars_count)
        return 1;

    gp->timeout = time->val.inum;
    gp->timer_limit = limit->val.inum;
    gp->timer_flag = 1;
    gp->timer_mode = mode->val.inum;
    gp->timer_count = 0;
    gp->begin_time = CZL_CLOCK;
    gp->cb_fun = (czl_fun*)cb_fun->val.obj;

    fun->ret.val.inum = 1;

    return 1;
}

char czl_sys_gettimer(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = (gp->timer_flag ? gp->timer_count : -1);
    return 1;
}

char czl_sys_stoptimer(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = gp->timer_flag;
    gp->timer_flag = 0;
    return 1;
}
#endif //defined CZL_TIMER
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)

char czl_sys_ltime(czl_gp *gp, czl_fun *fun)
{
    char *modify = fun->vars->val.str.s->str;
    time_t now;
    struct tm *tm_now;
    char time_str[128];
    unsigned long len = 0;

    time(&now);
    tm_now = localtime(&now);

    //Y-M-D h:m:s
    while (*modify)
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
            time_str[len++] = *modify;
            break;
        }
        modify++;
    }

    time_str[len] = '\0';

    return czl_set_ret_str(gp, fun, time_str, len);
}

#ifdef CZL_MM_MODULE
char czl_sys_gc(czl_gp *gp, czl_fun *fun)
{
    czl_mm_sp_pool_gc(gp);
    fun->ret.val.inum = gp->mm_cnt;
    return 1;
}
#endif //#ifdef CZL_MM_MODULE

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
    if (fun->vars->val.inum < 0)
    {
        fun->ret.val.inum = 0;
        return 0;
    }
    gp->mm_limit = fun->vars->val.inum;
    fun->ret.val.inum = 1;
    return 1;
}

//被调脚本与主脚本共享内存空间
void czl_gp_copy(czl_gp *a, czl_gp *b)
{
#ifdef CZL_MM_MODULE
	unsigned long i;
#ifdef CZL_MM_SLAB
    a->mm_cache = b->mm_cache;
    a->slab_pool = b->slab_pool;
#endif //#ifdef CZL_MM_SLAB
    for (i = 0; i < CZL_MM_SP_RANGE; ++i)
    {
        a->mmp_tmp[i] = b->mmp_tmp[i];
        a->mmp_rt[i] = b->mmp_rt[i];
        a->mmp_stack[i] = b->mmp_stack[i];
        a->mmp_str[i] = b->mmp_str[i];
    }
    a->mmp_tab = b->mmp_tab;
    a->mmp_arr = b->mmp_arr;
    a->mmp_sql = b->mmp_sql;
    a->mmp_ref = b->mmp_ref;
#endif //#ifdef CZL_MM_MODULE
    a->mm_limit = b->mm_limit;
#ifndef CZL_CONSOLE
    a->stack = b->stack;
    strcpy(a->log_path, b->log_path);
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

    if (czl_exec_shell(&new_gp, path->val.str.s->str))
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
char* czl_get_obj_type(const czl_var *obj, char *type)
{
    switch (obj->type)
    {
    case CZL_INT: strcpy(type, "int"); break;
    case CZL_FLOAT: strcpy(type, "float"); break;
    case CZL_STRING: strcpy(type, "string"); break;
    case CZL_FILE: strcpy(type, "file"); break;
    case CZL_FUN_REF: strcpy(type, "fun"); break;
    case CZL_TABLE: strcpy(type, "table"); break;
    case CZL_ARRAY: strcpy(type, "array"); break;
    case CZL_STACK: strcpy(type, "stack"); break;
    case CZL_QUEUE: strcpy(type, "queue"); break;
    case CZL_LIST: strcpy(type, "list"); break;
    default: //CZL_INSTANCE
        strcpy(type, ((czl_ins*)obj->val.obj)->pclass->name);
        break;
    }
    return type;
}

char czl_sys_hcac(czl_gp *gp, czl_fun *fun)
{
    czl_table *tab = (czl_table*)CZL_GCRV(fun->vars)->val.obj;
    fun->ret.val.inum = tab->attack_cnt;
    return 1;
}

char czl_sys_hdod(czl_gp *gp, czl_fun *fun)
{
	czl_tabkv *p;
	unsigned long i, j, sum;
	unsigned long s = (unsigned long)fun->vars[1].val.inum;
    unsigned long e = (unsigned long)fun->vars[2].val.inum;
    czl_table *tab = (czl_table*)CZL_GCRV(fun->vars)->val.obj;
	
    if (!tab)
        return 0;
    
	sum = 0;
    j = tab->size;
    for (i = 0; i < j; i++)
    {
        unsigned long cnt = 0;
        for (p = tab->buckets[i]; p; p = p->Next)
            ++cnt;
        if (cnt >= s && cnt <= e)
            sum += cnt;
    }

    fun->ret.type = CZL_FLOAT;
    fun->ret.val.fnum = (double)sum/tab->count;

    return 1;
}

char czl_sys_tp(czl_gp *gp, czl_fun *fun)
{
    char tmp[8];
    char *type = czl_get_obj_type(fun->vars, tmp);
    return czl_set_ret_str(gp, fun, type, strlen(type));
}

char czl_sys_ot(czl_gp *gp, czl_fun *fun)
{
    char tmp[8];
    char *ot = czl_get_obj_type(CZL_GRV(fun->vars), tmp);
    return czl_set_ret_str(gp, fun, ot, strlen(ot));
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

char czl_sys_ct(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = fun->vars;

    switch (obj->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FUN_REF: case CZL_INSTANCE:
        fun->ret.val.inum = 1;
        break;
    case CZL_STRING:
        fun->ret.val.inum = obj->val.str.size;
        break;
    case CZL_FILE:
        fun->ret.val.inum = czl_get_file_size(obj->val.file.fp);
        break;
    case CZL_TABLE:
        fun->ret.val.inum = ((czl_table*)obj->val.obj)->count;
        break;
    case CZL_ARRAY:
        fun->ret.val.inum = ((czl_array*)obj->val.obj)->cnt;
        break;
    default: //CZL_STACK/CZL_QUEUE/CZL_LIST
        fun->ret.val.inum = ((czl_sql*)obj->val.obj)->count;
        break;
    }

    return 1;
}

char czl_sys_rc(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = CZL_GRV(fun->vars);

    switch (obj->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FILE: case CZL_FUN_REF:
        fun->ret.val.inum = 1;
        break;
    case CZL_STRING:
        fun->ret.val.inum = obj->val.str.s->rc;
        break;
    case CZL_INSTANCE:
        fun->ret.val.inum = ((czl_ins*)obj->val.obj)->rc;
        break;
    case CZL_TABLE:
        fun->ret.val.inum = ((czl_table*)obj->val.obj)->rc;
        break;
    case CZL_ARRAY:
        fun->ret.val.inum = ((czl_array*)obj->val.obj)->rc;
        break;
    default: //CZL_STACK/CZL_QUEUE/CZL_LIST
        fun->ret.val.inum = ((czl_sql*)obj->val.obj)->rc;
        break;
    }

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_sys_push(czl_gp *gp, czl_fun *fun)
{
	czl_var *data;
	czl_sql *stack;
	czl_glo_var *ele;
    czl_var *ref = CZL_GCRV(fun->vars);

    if (!ref)
        return 0;

    stack = (czl_sql*)ref->val.obj;
    if (stack->rc > 1)
    {
        if (!czl_sql_fork(gp, stack, &ref->val, 1))
            return 0;
        stack = (czl_sql*)ref->val.obj;
    }

    if (!(ele=czl_sqlele_node_create(gp
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
    czl_sql *sq = (czl_sql*)ref->val.obj;
    if (sq->rc > 1)
    {
        if (!czl_sql_fork(gp, sq, &ref->val, 1))
            return 0;
        sq = (czl_sql*)ref->val.obj;
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
	czl_sql *queue;
	czl_glo_var *ele;
    czl_var *ref = CZL_GCRV(fun->vars);

    if (!ref)
        return 0;

    queue = (czl_sql*)ref->val.obj;
    if (queue->rc > 1)
    {
        if (!czl_sql_fork(gp, queue, &ref->val, 1))
            return 0;
        queue = (czl_sql*)ref->val.obj;
    }

    if (!(ele=czl_sqlele_node_create(gp
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

char czl_list_node_delete(czl_gp *gp, czl_sql *list, unsigned long inx)
{
	unsigned long i;
	czl_glo_var *p = list->eles_head, *q = NULL;

    if (!p)
        return 1;

    for (i = 1; p && i < inx; i++)
    {
        q = p;
        p = p->next;
    }

    if (CZL_VAR_EXIST_REF(p))
        czl_ref_obj_delete(gp, (czl_var*)p);
    if (!czl_val_del(gp, (czl_var*)p))
        return 0;
    if (p == list->foreach_inx)
        list->foreach_inx = p->next;

    if (p == list->eles_head)
        list->eles_head = p->next;
    else
        q->next = p->next;

    if (p == list->eles_tail)
        list->eles_tail = q;

    czl_free(gp, p, sizeof(czl_glo_var)
             #ifdef CZL_MM_MODULE
             , &list->pool
             #endif
             );

    --list->count;
    return 1;
}

czl_glo_var* czl_list_node_find(czl_sql *list, unsigned long inx)
{
	unsigned long i;
    czl_glo_var *p = list->eles_head;

    if (inx <= 0 || inx > list->count)
        return NULL;

    for (i = 1; p && i < inx; i++)
        p = p->next;

    return p;
}

char czl_sys_at(czl_gp *gp, czl_fun *fun)
{
	czl_glo_var *ele;
    czl_var *obj = CZL_GRV(fun->vars);
    czl_sql *list = (czl_sql*)obj->val.obj;
    if (list->rc > 1)
    {
        if (!czl_sql_fork(gp, list, &obj->val, 1))
            return 0;
        list = (czl_sql*)obj->val.obj;
    }

    if (!(ele=czl_list_node_find(list, (unsigned long)fun->vars[1].val.inum)))
    {
        fun->ret.type = CZL_INT;
        fun->ret.val.inum = 0;
        return 1;
    }

    if (!ele->name && !czl_ref_obj_create(gp, obj, (czl_var*)ele, &obj, 1))
        return -1;

    fun->ret.type = CZL_OBJ_REF;
    fun->ret.val.ref.inx = -1;
    fun->ret.val.ref.var = (czl_var*)ele;
    return 1;
}

char czl_sys_ins(czl_gp *gp, czl_fun *fun)
{
	unsigned long i = 0;
    czl_var *obj = CZL_GRV(fun->vars);
    czl_var *inx = fun->vars + 1;
    czl_var *data = fun->vars + 2;
    czl_sql *list = (czl_sql*)obj->val.obj;
	czl_glo_var *p = list->eles_head, *q = NULL;

    if (list->rc > 1)
    {
        if (!czl_sql_fork(gp, list, &obj->val, 1))
            return 0;
        list = (czl_sql*)obj->val.obj;
    }
    
    for (i = 0; p && i < inx->val.inum; i++)
    {
        q = p;
        p = p->next;
    }

    if (!(p=czl_sqlele_node_create(gp
                                   #ifdef CZL_MM_MODULE
                                   , &list->pool
                                   #endif
                                   )))
        return 0;

    if (!q)
    {
        p->next = list->eles_head;
        list->eles_head = p;
    }
    else
    {
        p->next = q->next;
        q->next = p;
    }

    if (q == list->eles_tail)
        list->eles_tail = p;

    list->count++;

    p->type = data->type;
    p->val = data->val;
    data->type = CZL_INT;

    return 1;
}

char czl_sys_del(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = CZL_GRV(fun->vars);

    czl_sql *list = (czl_sql*)obj->val.obj;
    if (list->rc > 1)
    {
        if (!czl_sql_fork(gp, list, &obj->val, 1))
            return 0;
        list = (czl_sql*)obj->val.obj;
    }

    return czl_list_node_delete(gp, list, (unsigned long)fun->vars[1].val.inum);
}
///////////////////////////////////////////////////////////////
czl_coroutine* czl_coroutine_create(czl_gp *gp, czl_fun *fun)
{
	czl_coroutine *p;
	unsigned long cnt;

    if (!fun || !fun->yeild_flag)
        return 0;

    if (!(p=CZL_COR_MALLOC(gp)))
        return NULL;

    if (!czl_sys_hash_insert(gp, CZL_INT, p, &gp->coroutines_hash))
    {
        CZL_COR_FREE(gp, p);
        return NULL;
    }

	cnt = fun->dynamic_vars_cnt + fun->foreach_cnt;
    if (0 == cnt)
        p->vars = NULL;
    else if (!(p->vars=(czl_var*)CZL_STACK_CALLOC(gp, cnt, sizeof(czl_var))))
    {
        CZL_COR_FREE(gp, p);
        czl_sys_hash_delete(gp, (unsigned long)p, &gp->coroutines_hash);
        return NULL;
    }
    czl_vars_init(p->vars, fun->vars, fun->dynamic_vars_cnt);

    p->fun = fun;
    p->pc = NULL;

    p->last = NULL;
    p->next = gp->coroutines_head;
    if (gp->coroutines_head)
        gp->coroutines_head->last = p;
    gp->coroutines_head = p;

    return p;
}

char czl_sys_newcor(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum =
            (unsigned long)czl_coroutine_create(gp, (czl_fun*)fun->vars->val.obj);
    return 1;
}

char czl_sys_resume(czl_gp *gp, czl_fun *fun)
{
	unsigned long id;
	czl_coroutine *c;
    czl_para *p = (czl_para*)fun->vars;
    czl_var *ret = czl_exp_cac(gp, p->para);
    if (!ret)
        return 0;
    if (ret->type != CZL_INT)
        return 1;
    id = (unsigned long)ret->val.inum;
    if (!(c=(czl_coroutine*)czl_sys_hash_find(CZL_INT, CZL_NIL,
								(char*)id, &gp->coroutines_hash)))
        return 1;
    if ((unsigned long)c->fun->enter_vars_count < fun->paras_cnt-1)
        return 1;

    if ((ret=czl_coroutine_run(gp, p->next, fun->paras_cnt-1, c)))
    {
        fun->ret.type = ret->type;
        fun->ret.val = ret->val;
        ret->type = CZL_INT;
    }
    return 1;
}

char czl_sys_corsta(czl_gp *gp, czl_fun *fun)
{
    unsigned long id = (unsigned long)fun->vars->val.inum;
    if (czl_sys_hash_find(CZL_INT, CZL_NIL,
                          (char*)id, &gp->coroutines_hash))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    return 1;
}

char czl_sys_reset(czl_gp *gp, czl_fun *fun)
{
    if (CZL_FUN_REF == fun->vars->type)
    {
        czl_fun *f = (czl_fun*)fun->vars->val.obj;
        if (!f || !f->yeild_flag || CZL_IN_BUSY == f->state)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        f->pc = NULL;
        czl_coroutine_paras_reset(gp, (czl_var*)f->backup_vars, f);
    }
    else if (CZL_INT == fun->vars->type)
    {
        unsigned long id = (unsigned long)fun->vars->val.inum;
        czl_coroutine *c = (czl_coroutine*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                             (char*)id, &gp->coroutines_hash);
        if (!c || CZL_IN_BUSY == c->fun->state)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        c->pc = NULL;
        czl_coroutine_paras_reset(gp, c->vars, c->fun);
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
    czl_coroutine *c = (czl_coroutine*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                         (char*)id, &gp->coroutines_hash);
    if (!c || CZL_IN_BUSY == c->fun->state)
        fun->ret.val.inum = 0;
    else
    {
        fun->ret.val.inum = 1;
        czl_coroutine_delete(gp, (czl_var*)c->vars, c);
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

void czl_pipe_buf_delete(czl_var *vars, unsigned long cnt, char flag)
{
    unsigned long i;
    for (i = 0; i < cnt; ++i)
        if (CZL_STRING == vars[i].type)
            free(vars[i].val.str.s);

    if (flag)
        free(vars);
}

char czl_pipe_buf_create
(
    czl_var *vars,
    const czl_array *arr
)
{
    czl_str *a, *b;
    unsigned long i;
    for (i = 0; i < arr->cnt; ++i)
    {
        switch (arr->vars[i].type)
        {
        case CZL_INT: case CZL_FLOAT:
            vars[i].val = arr->vars[i].val;
            break;
        case CZL_STRING:
            a = &vars[i].val.str;
            b = &arr->vars[i].val.str;
            if (!(a->s=(czl_string*)malloc(b->s->len+1)))
                return 0;
            memcpy(a->s, b->s->str, b->s->len);
            ((char*)a->s)[b->s->len] = '\0';
            a->size = b->s->len;
            break;
        default:
            break;
        }
        vars[i].type = arr->vars[i].type;
    }

    return 1;
}

char czl_report_buf_create
(
    czl_thread_pipe *pipe,
    const czl_var *var
)
{
	czl_var *buf;
    unsigned long cnt;

    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        cnt = 1;
        break;
    case CZL_ARRAY:
        cnt = ((czl_array*)var->val.obj)->cnt;
        break;
    default:
        return 1;
    }
    
    if (pipe->rb_cnt + cnt > pipe->rb_size)
    {
        if (!(buf=(czl_var*)realloc(pipe->report_buf,
                          2*(pipe->rb_cnt+cnt)*sizeof(czl_var))))
            return 0;
        pipe->report_buf = buf;
        pipe->rb_size = 2*(pipe->rb_cnt+cnt);
        memset(buf+pipe->rb_cnt, 0, cnt*sizeof(czl_var));
    }
    buf = pipe->report_buf + pipe->rb_cnt;

    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT:
        *buf = *var;
        break;
    case CZL_STRING:
        buf->val.str.s = (czl_string*)malloc(var->val.str.s->len+1);
        if (!buf->val.str.s)
            return 0;
        memcpy(buf->val.str.s, var->val.str.s->str, var->val.str.s->len);
        ((char*)buf->val.str.s)[var->val.str.s->len] = '\0';
        buf->val.str.size = var->val.str.s->len;
        buf->type = CZL_STRING;
        break;
    default: //CZL_ARRAY
        if (!czl_pipe_buf_create(buf, (czl_array*)var->val.obj))
        {
            czl_pipe_buf_delete(buf, cnt, 0);
            return 0;
        }
        break;
    }

    pipe->rb_cnt += cnt;
    return 1;
}

char czl_notify_buf_create
(
    czl_thread_pipe *pipe,
    const czl_var *var
)
{
	czl_var *buf;
	char ret = 0;
    unsigned long cnt;
    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        cnt = 1;
        break;
    case CZL_ARRAY:
        cnt = ((czl_array*)var->val.obj)->cnt;
        break;
    default:
        return 1;
    }

    czl_thread_lock(&pipe->notify_lock); //lock

    if (pipe->nb_cnt + cnt > pipe->nb_size)
    {
        if (!(buf=(czl_var*)realloc(pipe->notify_buf,
                          2*(pipe->nb_cnt+cnt)*sizeof(czl_var))))
            goto CZL_END;
        pipe->notify_buf = buf;
        pipe->nb_size = 2*(pipe->nb_cnt+cnt);
        memset(buf+pipe->nb_cnt, 0, cnt*sizeof(czl_var));
    }
    buf = pipe->notify_buf + pipe->nb_cnt;

    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT:
        *buf = *var;
        break;
    case CZL_STRING:
        buf->val.str.s = (czl_string*)malloc(var->val.str.s->len+1);
        if (!buf->val.str.s)
            goto CZL_END;
        memcpy(buf->val.str.s, var->val.str.s->str, var->val.str.s->len);
        ((char*)buf->val.str.s)[var->val.str.s->len] = '\0';
        buf->val.str.size = var->val.str.s->len;
        buf->type = CZL_STRING;
        break;
    default: //CZL_ARRAY
        if (!czl_pipe_buf_create(buf, (czl_array*)var->val.obj))
        {
            czl_pipe_buf_delete(buf, cnt, 0);
            goto CZL_END;
        }
        break;
    }

    pipe->nb_cnt += cnt;
    ret = 1;

CZL_END:
    czl_thread_unlock(&pipe->notify_lock); //unlock
    return ret;
}

czl_array* czl_pipe_paras_create
(
    czl_gp *gp,
    czl_var *vars,
    unsigned long cnt
)
{
	unsigned long i;
    czl_array *arr = czl_array_create(gp, cnt, cnt);
    if (!arr)
        return NULL;

    for (i = 0; i < cnt; ++i)
    {
        switch (vars[i].type)
        {
        case CZL_INT: case CZL_FLOAT:
            arr->vars[i].val = vars[i].val;
            break;
        case CZL_STRING:
            if (!czl_str_create(gp, &arr->vars[i].val.str,
                                vars[i].val.str.size+1,
                                vars[i].val.str.size,
                                vars[i].val.str.s))
            {
                czl_array_delete(gp, arr);
                return NULL;
            }
            break;
        default:
            break;
        }
        arr->vars[i].type = vars[i].type;
    }

    return arr;
}

char czl_report_paras_create
(
    czl_gp *gp,
    czl_var *var,
    czl_thread_pipe *pipe
)
{
    czl_array *arr = czl_pipe_paras_create(gp,
                                            pipe->report_buf,
                                            pipe->rb_cnt);

    czl_pipe_buf_delete(pipe->report_buf, pipe->rb_cnt, 1);
    pipe->report_buf = NULL;
    pipe->rb_cnt = pipe->rb_size = 0;

    if (!arr)
    {
        var->val.inum = 1;
        return 0;
    }

    var->type = CZL_ARRAY;
    var->val.obj = arr;
    return 1;
}

char czl_notify_paras_create
(
    czl_gp *gp,
    czl_var *var,
    czl_thread_pipe *pipe
)
{
    czl_array *arr = czl_pipe_paras_create(gp,
                                            pipe->notify_buf,
                                            pipe->nb_cnt);

    czl_pipe_buf_delete(pipe->notify_buf, pipe->nb_cnt, 1);
    pipe->notify_buf = NULL;
    pipe->nb_cnt = pipe->nb_size = 0;

    if (!arr)
    {
        var->val.inum = 1;
        return 0;
    }

    var->type = CZL_ARRAY;
    var->val.obj = arr;
    return 1;
}

void czl_thread_pipe_delete(czl_thread_pipe *pipe)
{
    czl_pipe_buf_delete(pipe->report_buf, pipe->rb_cnt, 1);
    pipe->report_buf = NULL;
    pipe->rb_cnt = pipe->rb_size = 0;
    czl_pipe_buf_delete(pipe->notify_buf, pipe->nb_cnt, 1);
    pipe->notify_buf = NULL;
    pipe->nb_cnt = pipe->nb_size = 0;
    czl_lock_destroy(&pipe->notify_lock);
    czl_lock_destroy(&pipe->report_lock);
    free(pipe);
}

void czl_thread_delete(czl_gp *gp, czl_thread *p)
{
#ifdef CZL_SYSTEM_WINDOWS
    CloseHandle(p->id);
#endif
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

czl_thread_pipe* czl_thread_pipe_create(czl_var *var)
{
    czl_thread_pipe *pipe = (czl_thread_pipe*)malloc(sizeof(czl_thread_pipe));
    if (!pipe)
        return NULL;

    czl_lock_init(&pipe->notify_lock);
    czl_lock_init(&pipe->report_lock);

    if (!czl_event_init(&pipe->notify_event))
    {
        free(pipe);
        return NULL;
    }
    if (!czl_event_init(&pipe->report_event))
    {
        czl_event_destroy(&pipe->notify_event);
        free(pipe);
        return NULL;
    }
    if (!czl_event_init(&pipe->pipe_event))
    {
        czl_event_destroy(&pipe->notify_event);
        czl_event_destroy(&pipe->report_event);
        free(pipe);
        return NULL;
    }

    pipe->alive = 1;
    pipe->notify_buf = NULL;
    pipe->nb_cnt = pipe->nb_size = 0;
    pipe->report_buf = NULL;
    pipe->rb_cnt = pipe->rb_size = 0;

    if (!czl_notify_buf_create(pipe, var))
    {
        czl_event_destroy(&pipe->notify_event);
        czl_event_destroy(&pipe->report_event);
        czl_event_destroy(&pipe->pipe_event);
        free(pipe);
        return NULL;
    }

    return pipe;
}

#ifdef CZL_SYSTEM_WINDOWS
DWORD WINAPI
#elif defined CZL_SYSTEM_LINUX
void*
#endif
czl_sys_thread(void *argv)
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
        strcpy(gp->log_path, log_path);
        free(log_path);
        log_path = NULL;
    }
#endif
    if (!czl_sys_init(gp, 0))
    {
        czl_event_send(&gp->thread_pipe->pipe_event);
        czl_val_del(gp, &gp->enter_var);
        czl_init_free(gp, 1);
        free(gp);
        goto CZL_END;
    }

    czl_exec_shell(gp, shell_path);
    czl_memory_free(gp);
    free(gp);

CZL_END:
    free(shell_path);
#ifndef CZL_CONSOLE
    free(log_path);
#endif
    czl_event_send(&pipe->report_event); //防止主线程阻塞等待已经退出的子线程
    czl_event_destroy(&pipe->notify_event);
    czl_event_destroy(&pipe->report_event);
    czl_event_destroy(&pipe->pipe_event);
    if (pipe->alive)
        pipe->alive = 0;
    else
        czl_thread_pipe_delete(pipe);
    return 0;
}

char czl_sys_newshl(czl_gp *gp, czl_fun *fun)
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

    if (!(p->pipe=czl_thread_pipe_create(fun->vars+1)))
        goto CZL_END;

    if (!(argv=(void**)calloc(3, sizeof(void*))))
        goto CZL_END;

    argv[0] = p->pipe;

    if (!(shell_path=(char*)malloc(fun->vars->val.str.s->len)))
        goto CZL_END;
    memcpy(shell_path,
           fun->vars->val.str.s->str,
           fun->vars->val.str.s->len);
    shell_path[fun->vars->val.str.s->len] = '\0';
    argv[1] = shell_path;

#ifndef CZL_CONSOLE
    if (fun->vars[2].val.str.s->len)
    {
        if (!(log_path=(char*)malloc(fun->vars[2].val.str.s->len)))
            goto CZL_END;
        memcpy(log_path,
               fun->vars[2].val.str.s->str,
               fun->vars[2].val.str.s->len);
        log_path[fun->vars[2].val.str.s->len] = '\0';
    }
    argv[2] = log_path;
#endif //#ifndef CZL_CONSOLE

#ifdef CZL_SYSTEM_WINDOWS
    if (!(p->id=CreateThread(NULL, 0, czl_sys_thread, argv, 0, NULL)))
        goto CZL_END;
#elif defined CZL_SYSTEM_LINUX
    if (pthread_create(&p->id, NULL, czl_sys_thread, argv))
        goto CZL_END;
#endif

    //防止线程参数被notify参数覆盖，所以设置事件等待
    czl_event_wait(&p->pipe->pipe_event);
    fun->ret.val.inum = (unsigned long)p;
    return 1;

CZL_END:
    free(argv);
    free(shell_path);
#ifndef CZL_CONSOLE
    free(log_path);
#endif
    if (p->pipe)
    {
        czl_event_destroy(&p->pipe->notify_event);
        czl_event_destroy(&p->pipe->report_event);
        czl_event_destroy(&p->pipe->pipe_event);
    }
    czl_thread_delete(gp, p);

    return 1;
}

czl_thread* czl_thread_find(czl_gp *gp, unsigned long id, czl_fun *fun)
{
#ifdef CZL_SYSTEM_WINDOWS
	DWORD exit_code;
#endif
    czl_thread *p = (czl_thread*)czl_sys_hash_find(CZL_INT, CZL_NIL,
                                      (char*)id, &gp->threads_hash);
    if (!p)
        return NULL;

#ifdef CZL_SYSTEM_WINDOWS
    if (GetExitCodeThread(p->id, &exit_code) &&
        exit_code != STILL_ACTIVE)
#elif defined CZL_SYSTEM_LINUX
    if (pthread_kill(p->id, 0) == ESRCH)
#endif
    {
        if (fun && p->pipe->report_buf)
            czl_report_paras_create(gp, &fun->ret, p->pipe);
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
        if (gp->thread_pipe->notify_buf)
            czl_notify_paras_create(gp, &fun->ret, gp->thread_pipe);
        else
        {
            czl_thread_unlock(&gp->thread_pipe->notify_lock); //unlock
            czl_event_wait(&gp->thread_pipe->notify_event);
            czl_thread_lock(&gp->thread_pipe->notify_lock); //lock
            if (gp->thread_pipe->notify_buf)
                czl_notify_paras_create(gp, &fun->ret, gp->thread_pipe);
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
        if (p->pipe->report_buf)
        {
            czl_report_paras_create(gp, &fun->ret, p->pipe);
            czl_event_send(&p->pipe->pipe_event);
        }
        else
        {
            czl_thread_unlock(&p->pipe->report_lock); //unlock
            czl_event_wait(&p->pipe->report_event);
            czl_thread_lock(&p->pipe->report_lock); //lock
            if (p->pipe->report_buf)
            {
                czl_report_paras_create(gp, &fun->ret, p->pipe);
                czl_event_send(&p->pipe->pipe_event);
            }
            else
                fun->ret.val.inum = 0;
        }
        czl_thread_unlock(&p->pipe->report_lock); //unlock
    }

    return 1;
}

char czl_sys_listen(czl_gp *gp, czl_fun *fun)
{
    czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, fun);
    if (!p)
    {
        if (CZL_INT == fun->ret.type)
            fun->ret.val.inum = 0;
        return 1;
    }
    czl_thread_lock(&p->pipe->report_lock); //lock
    if (p->pipe->report_buf)
    {
        czl_report_paras_create(gp, &fun->ret, p->pipe);
        czl_event_send(&p->pipe->pipe_event);
    }
    else
        fun->ret.val.inum = 0;
    czl_thread_unlock(&p->pipe->report_lock); //unlock

    return 1;
}

char czl_sys_waitshl(czl_gp *gp, czl_fun *fun)
{
    czl_thread *p = czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL);
    if (p)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        WaitForSingleObject(p->id, INFINITE);
    #elif defined CZL_SYSTEM_LINUX
        pthread_join(p->id, NULL);
    #endif
        if (p->pipe->report_buf)
            czl_report_paras_create(gp, &fun->ret, p->pipe);
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

    czl_thread_lock(&gp->thread_pipe->report_lock); //lock
    if (czl_report_buf_create(gp->thread_pipe, fun->vars))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    czl_thread_unlock(&gp->thread_pipe->report_lock); //unlock
    czl_event_send(&gp->thread_pipe->report_event);
    czl_event_wait(&gp->thread_pipe->pipe_event);

    return 1;
}

char czl_sys_send(czl_gp *gp, czl_fun *fun)
{
    if (!gp->thread_pipe)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    czl_thread_lock(&gp->thread_pipe->report_lock); //lock
    if (czl_report_buf_create(gp->thread_pipe, fun->vars))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;
    czl_thread_unlock(&gp->thread_pipe->report_lock); //unlock
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
    if (czl_notify_buf_create(p->pipe, fun->vars + 1))
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
    #ifdef CZL_SYSTEM_WINDOWS
        DWORD exit_code;
        if (GetExitCodeThread(p->id, &exit_code) &&
            exit_code != STILL_ACTIVE)
    #elif defined CZL_SYSTEM_LINUX
        if (pthread_kill(p->id, 0) == ESRCH)
    #endif
        {
            czl_thread *q = p->next;
            czl_thread_delete(gp, p);
            p = q;
            continue;
        }
        if (!czl_notify_buf_create(p->pipe, fun->vars))
            fun->ret.val.inum = 0;
        czl_event_send(&p->pipe->notify_event);
        p = p->next;
    }

    return 1;
}

char czl_sys_shlsta(czl_gp *gp, czl_fun *fun)
{
    if (czl_thread_find(gp, (unsigned long)fun->vars->val.inum, NULL))
        fun->ret.val.inum = 1;
    else
        fun->ret.val.inum = 0;

    return 1;
}
#endif //#ifdef CZL_MULT_THREAD
///////////////////////////////////////////////////////////////
#ifndef CZL_CONSOLE
char czl_sys_cleanlog(czl_gp *gp, czl_fun *fun)
{
	FILE *fout;

    if (!gp->log_path[0])
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
    strcpy(gp->log_path, fun->vars->val.str.s->str);
    return 1;
}

char czl_sys_closelog(czl_gp *gp, czl_fun *fun)
{
    gp->log_path[0] = '\0';
    return 1;
}

czl_glo_var* czl_stack_ele_create(czl_gp*);

char czl_sys_pushi(czl_gp *gp, czl_fun *fun)
{
    return czl_pushi(gp, fun->vars->val.inum);
}

char czl_sys_pushf(czl_gp *gp, czl_fun *fun)
{
    return czl_pushf(gp, fun->vars->val.fnum);
}

char czl_sys_pushs(czl_gp *gp, czl_fun *fun)
{
    czl_glo_var *ele = czl_stack_ele_create(gp);
    if (!ele)
        return 0;

    ele->type = CZL_STRING;
    ele->val = fun->vars->val;
    fun->vars->type = CZL_INT;
    return 1;
}

char czl_sys_pop_data(czl_gp *gp, czl_fun *fun)
{
    czl_var *obj = CZL_GRV(fun->vars);
    if (!czl_ret_clean(gp, obj))
        return 0;

    fun->ret.val.inum = czl_pop(gp, obj);
    return 1;
}

char czl_sys_size(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_size(gp);
    return 1;
}

char czl_sys_clean(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_clean(gp);
    return 1;
}

czl_glo_var* czl_stack_ele_create(czl_gp *gp)
{
    czl_glo_var *ele = (czl_glo_var*)czl_malloc(gp, sizeof(czl_glo_var)
                                  #ifdef CZL_MM_MODULE
                                  , &gp->stack->pool
                                  #endif
                                  );
    if (!ele)
        return NULL;

    ele->name = NULL;

    ele->next = gp->stack->eles_head;
    gp->stack->eles_head = ele;
    gp->stack->count++;
    return ele;
}

char czl_pushi(czl_gp *gp, czl_long inum)
{
	czl_glo_var *ele;

    if (!gp->stack) return 0;

    if (!(ele=czl_stack_ele_create(gp)))
        return 0;

    ele->type = CZL_INT;
    ele->val.inum = inum;
    return 1;
}

char czl_pushf(czl_gp *gp, double fnum)
{
	czl_glo_var *ele;

    if (!gp->stack) return 0;

    if (!(ele=czl_stack_ele_create(gp)))
        return 0;

    ele->type = CZL_FLOAT;
    ele->val.fnum = fnum;
    return 1;
}

char czl_pushs(czl_gp *gp, char *str)
{
	czl_str s;
	unsigned long len;
	czl_glo_var *ele;

    if (!gp->stack) return 0;

    len = strlen(str);
    if (!czl_str_create(gp, &s, len+1, len, str))
        return 0;

    if (!(ele=czl_stack_ele_create(gp)))
    {
        CZL_SF(gp, s);
        return 0;
    }

    ele->type = CZL_STRING;
    ele->val.str = s;
    return 1;
}

char czl_pop(czl_gp *gp, czl_var *ret)
{
	czl_glo_var *ele;

    if (!gp->stack) return 0;

    if (0 == gp->stack->count)
        return 0;

    gp->stack->count--;
    ele = gp->stack->eles_head;
    gp->stack->eles_head = ele->next;

    ret->type = ele->type;
    ret->val = ele->val;

    czl_free(gp, ele, sizeof(czl_glo_var)
             #ifdef CZL_MM_MODULE
             , &gp->stack->pool
             #endif
             );

    return 1;
}

void czl_dels(czl_gp *gp, czl_var str)
{
    CZL_SRCD1(gp, str.val.str);
}

unsigned long czl_size(czl_gp *gp)
{
    if (!gp->stack) return 0;

    return gp->stack->count;
}

#ifdef CZL_MM_MODULE
char czl_clean(czl_gp *gp)
{
    czl_glo_var *p;

    if (!gp->stack) return 0;

    for (p = gp->stack->eles_head; p; p = p->next)
    {
        czl_val_del(gp, (czl_var*)p);
    }

    gp->stack->eles_head = NULL;
    gp->stack->count = 0;

    czl_mm_sp_destroy(gp, &gp->stack->pool);
    czl_mm_pool_init(&gp->stack->pool, sizeof(czl_glo_var), 0);

    return 1;
}
#else
char czl_clean(czl_gp *gp)
{
    czl_glo_var *p, *q;

    if (!gp->stack) return 0;

    for (p = gp->stack->eles_head; p; p = q)
    {
        q = p->next;
        czl_val_del(gp, (czl_var*)p);
        czl_free(gp, p, sizeof(czl_glo_var));
    }

    gp->stack->eles_head = NULL;
    gp->stack->count = 0;

    return 1;
}
#endif

char czl_exec(czl_gp *gp)
{
    char ret;

    if (gp->stack)
        ret = czl_resume_shell(gp, gp->cur_fun);
    else
    {
        if (!czl_sys_init(gp, 1))
        {
            czl_init_free(gp, 1);
            return 0;
        }
        ret = czl_exec_shell(gp, gp->shell_path);
    }

    if (ret != 2)
        czl_memory_free(gp);

    return ret;
}

char czl_exit(czl_gp *gp)
{
    if (!gp->stack) return 0;

    czl_memory_free(gp);
    return 1;
}

czl_gp* czl_open(char *shell_path, char *log_path)
{
    czl_gp *gp;

    if (!shell_path)
        return NULL;

    if (!(gp=(czl_gp*)malloc(sizeof(czl_gp))))
        return NULL;

    gp->stack = NULL;
    strcpy(gp->shell_path, shell_path);
    if (log_path)
        strcpy(gp->log_path, log_path);

    return gp;
}

void czl_close(czl_gp *gp)
{
    if (gp->stack)
        czl_memory_free(gp);
    free(gp);
}
#endif //#ifndef CZL_CONSOLE

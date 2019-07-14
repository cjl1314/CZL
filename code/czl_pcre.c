#include "czl_pcre.h"

#ifdef CZL_LIB_REG

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_reg_mode(czl_gp *gp, czl_fun *fun);    //编译正则模式
char czl_reg_free(czl_gp *gp, czl_fun *fun);    //释放正则资源
char czl_reg_match(czl_gp *gp, czl_fun *fun);   //正则匹配
char czl_reg_collect(czl_gp *gp, czl_fun *fun); //正则提取
char czl_reg_replace(czl_gp *gp, czl_fun *fun); //正则替换

//库函数表定义
const czl_sys_fun czl_lib_reg[] =
{
    //函数名,    函数指针,          参数个数,  参数声明
    {"mode",    czl_reg_mode,     1,        "str_v1"},
    {"free",    czl_reg_free,     1,        "int_v1"},
    {"match",   czl_reg_match,    2,        "int_v1,str_v2"},
    {"collect", czl_reg_collect,  2,        "int_v1,str_v2"},
    {"replace", czl_reg_replace,  3,        "int_v1,str_v2,str_v3"},
};

#define CZL_REG_BUF_SIZE 3*100 //必须为3*n, n为结果个数，小于3*n得不到结果

char czl_reg_mode(czl_gp *gp, czl_fun *fun)
{
    char *pattern = CZL_STR(fun->vars->val.str.Obj)->str;
    const char *error;
    int erroffset;
    pcre16 *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);

    if (!re)
        fun->ret.val.inum = 0;
    else if (!(fun->ret.val.inum=czl_extsrc_create(gp, re, pcre_free)))
        return 0;

    return 1;
}

char czl_reg_free(czl_gp *gp, czl_fun *fun)
{
    fun->ret.val.inum = czl_extsrc_free(gp, fun->vars->val.inum);
    return 1;
}

char czl_reg_match(czl_gp *gp, czl_fun *fun)
{
    pcre16 *re = czl_extsrc_get(gp, fun->vars->val.inum);
    czl_string *text = CZL_STR(fun->vars[1].val.str.Obj);
    int ovector[CZL_REG_BUF_SIZE];

    if (!re ||
        pcre_exec(re, NULL, text->str, text->len, 0, 0, ovector, CZL_REG_BUF_SIZE) <= 0 ||
        !(0 == ovector[0] && text->len == ovector[1]))
        fun->ret.val.inum = 0;
    else
        fun->ret.val.inum = 1;

    return 1;
}

char czl_reg_collect(czl_gp *gp, czl_fun *fun)
{
    pcre16 *re = czl_extsrc_get(gp, fun->vars->val.inum);
    czl_string *text = CZL_STR(fun->vars[1].val.str.Obj);
    int offset = 0;
    czl_array *arr = NULL;

    if (!re)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    for (;;)
    {
        czl_var *vars;
        int i, j, cnt;
        int ovector[CZL_REG_BUF_SIZE];
        cnt = pcre_exec(re, NULL, text->str, text->len, offset, 0, ovector, CZL_REG_BUF_SIZE);
        if (cnt <= 0)
            return 1;
        if (CZL_INT == fun->ret.type)
        {
            if (!(fun->ret.val.Obj=czl_array_create(gp, 1, 0)))
                return 0;
            fun->ret.type = CZL_ARRAY;
            arr = CZL_ARR(fun->ret.val.Obj);
        }
        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            return 0;
        if (!(arr->vars[arr->cnt].val.Obj=czl_array_create(gp, cnt, cnt)))
            return 0;
        arr->vars[arr->cnt].type = CZL_ARRAY;
        vars = CZL_ARR(arr->vars[arr->cnt++].val.Obj)->vars;
        for (i = j = 0; i < cnt; ++i, j += 2)
        {
            unsigned long len = ovector[j+1] - ovector[j];
            if (!czl_str_create(gp, &vars[i].val.str, len+1, len, text->str+ovector[j]))
                return 0;
            vars[i].type = CZL_STRING;
        }
        offset = ovector[j-1];
    }
}

char czl_reg_replace(czl_gp *gp, czl_fun *fun)
{
    pcre16 *re = czl_extsrc_get(gp, fun->vars->val.inum);
    czl_string *text = CZL_STR(fun->vars[1].val.str.Obj);
    czl_string *str = CZL_STR(fun->vars[2].val.str.Obj);
    int inx = 0;
    int offset = 0;
    czl_string *s = NULL;

    if (!re)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    for (;;)
    {
        int cnt;
        unsigned long len, size;
        int ovector[CZL_REG_BUF_SIZE];
        cnt = pcre_exec(re, NULL, text->str, text->len, offset, 0, ovector, CZL_REG_BUF_SIZE);
        if (cnt <= 0)
        {
            if (CZL_STRING == fun->ret.type)
            {
                len = text->len - inx;
                memcpy(s->str+s->len, text->str+inx, len);
                s->len += len;
                s->str[s->len] = '\0';
            }
            return 1;
        }
        if (CZL_INT == fun->ret.type)
        {
            if (!czl_str_create(gp, &fun->ret.val.str, text->len+1, 0, NULL))
                return 0;
            fun->ret.type = CZL_STRING;
            s = CZL_STR(fun->ret.val.str.Obj);
        }
        len = ovector[0] - inx;
        size = 1 + s->len + len + str->len + text->len - ovector[1];
        if (size >= fun->ret.val.str.size)
        {
            void **obj;
            size += s->len;
            if (!(obj=CZL_SR(gp, fun->ret.val.str, size)))
                return 0;
            fun->ret.val.str.Obj = obj;
            fun->ret.val.str.size += size;
            s = CZL_STR(fun->ret.val.str.Obj);
        }
        memcpy(s->str+s->len, text->str+inx, len);
        s->len += len;
        memcpy(s->str+s->len, str->str, str->len);
        s->len += str->len;
        s->str[s->len] = '\0';
        inx = ovector[1];
        offset = ovector[cnt*2-1];
    }
}
#endif //CZL_LIB_REG

#include "czl_ws.h"

#ifdef CZL_LIB_WS

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_ws_doc(czl_gp *gp, czl_fun *fun);    //解包websocket数据
char czl_ws_mask(czl_gp *gp, czl_fun *fun);   //对数据进行掩码计算
char czl_ws_send(czl_gp *gp, czl_fun *fun);   //发送websocket数据
char czl_ws_close(czl_gp *gp, czl_fun *fun);  //发送websocket关闭请求
char czl_ws_ping(czl_gp *gp, czl_fun *fun);   //发送websocket心跳ping
char czl_ws_pong(czl_gp *gp, czl_fun *fun);   //发送websocket心跳pong

//库函数表定义
const czl_sys_fun czl_lib_ws[] =
{
    //函数名,     函数指针,        参数个数,  参数声明
    {"doc",      czl_ws_doc,     1,        "str_v1"},
    {"mask",     czl_ws_mask,    3,        "&str_v1,int_v2,int_v3"},
    {"send",     czl_ws_send,    3,        "int_v1,str_v2,int_v3=0"},
    {"close",    czl_ws_close,   1,        "int_v1"},
    {"ping",     czl_ws_ping,    1,        "int_v1"},
    {"pong",     czl_ws_pong,    1,        "int_v1"},
};

void czl_ws_mask_compute
(
    unsigned char *s,
    unsigned long size,
    unsigned long mask
)
{
    unsigned long cnt, len = size;

    size = size/4*4;
    len -= size;

    for (cnt = 0; cnt < size; cnt += 4)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
        *(s++) ^= ((mask&0xff0000)>>16);
        *(s++) ^= ((mask&0xff000000)>>24);
    }

    if (1 == len)
        *(s++) ^= (mask&0xff);
    else if (2 == len)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
    }
    else if (3 == len)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
        *(s++) ^= ((mask&0xff0000)>>16);
    }
}

char czl_ws_doc(czl_gp *gp, czl_fun *fun)
{
    czl_string *str = CZL_STR(fun->vars->val.str.Obj);
    void **obj = czl_array_create(gp, 4, 0);
    czl_array *arr;
    czl_var *vars;
    unsigned char *s, *e;

    if (!obj)
        return 0;
    arr = CZL_ARR(obj);

    s = str->str;
    e = s + str->len;

    while (s < e)
    {
        unsigned long size, cnt;
        unsigned char maskFlag, opcode = *s&0x0f;

        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            goto CZL_OOM;

        if (!(arr->vars[arr->cnt].val.Obj=czl_array_create(gp, 6, 6)))
            return 0;
        vars = CZL_ARR(arr->vars[arr->cnt].val.Obj)->vars;
        arr->vars[arr->cnt++].type = CZL_ARRAY;

        vars[0].val.inum = (*s&0x80)>>4;
        vars[1].val.inum = opcode;
        if (opcode >= 0x08 && opcode <= 0x0a && s+1 == e)
        {
            ++s;
            break;
        }

        size = s[1]&0x7f;
        if (size < 126)
            cnt = 2;
        else if (126 == size)
        {
            if (e-s < 4)
                break;
            size = s[2]<<8 | s[3];
            cnt = 4;
        }
        else
        {
            if (e-s < 10)
                break;
            size = s[6]<<24 | s[7]<<16 | s[8]<<8 | s[9];
            cnt = 10;
        }
        maskFlag = s[1]&0x80;
        if (maskFlag)
            cnt += 4;
        if (s+cnt > e)
            break;

        vars[2].val.inum = size;
        if (size > e-s-cnt)
            size = e - s - cnt;

        if (maskFlag)
        {
            unsigned long mask = *((unsigned long*)(s+cnt-4));
            vars[3].val.inum = mask;
            if (!czl_set_ret_str(gp, vars+4, s+cnt, size))
                goto CZL_OOM;
            if (vars[2].val.inum == size)
                czl_ws_mask_compute(CZL_STR(vars[4].val.str.Obj)->str, size, mask);
        }
        else if (!czl_set_ret_str(gp, vars+4, s+cnt, size))
            goto CZL_OOM;

        s += (cnt+size);
    }

    if (s < e && !czl_set_ret_str(gp, vars+4, s, e-s))
        goto CZL_OOM;

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;

CZL_OOM:
    czl_array_delete(gp, obj);
    return 0;
}

char czl_ws_mask(czl_gp *gp, czl_fun *fun)
{
    czl_var *data = CZL_GRV(fun->vars);
    czl_string *str = CZL_STR(data->val.str.Obj);
    unsigned long mask = fun->vars[1].val.inum;
    unsigned long offset = fun->vars[2].val.inum;

    if (0 == mask || offset > str->len)
        return 1;

    if (str->rc > 1 && !(str=czl_string_fork(gp, data)))
        return 0;

    czl_ws_mask_compute(str->str+offset, str->len-offset, mask);
    return 1;
}

char czl_ws_send(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_string *str = CZL_STR(fun->vars[1].val.str.Obj);
    unsigned long mask = fun->vars[2].val.inum;
    unsigned char *buf = CZL_TMP_MALLOC(gp, 14+str->len);
    unsigned long len;

    if (!buf)
        return 0;

    buf[0] = 0x81;
    buf[1] = (mask ? 0x80 : 0x00);

    if (str->len < 126)
    {
        buf[1] |= str->len;
        len = 2;
    }
    else if (str->len < 65536)
    {
        buf[1] |= 126;
        buf[2] = (str->len&0xff00) >> 8;
        buf[3] = str->len&0xff;
        len = 4;
    }
    else
    {
        buf[1] |= 127;
        buf[2] = buf[3] = buf[4] = buf[5] = 0;
        buf[6] = (str->len&0xff000000) >> 24;
        buf[7] = (str->len&0xff0000) >> 16;
        buf[8] = (str->len&0xff00) >> 8;
        buf[9] = str->len&0xff;
        len = 10;
    }

    if (mask)
    {
        *((unsigned long*)(buf+len)) = mask;
        len += 4;
        memcpy(buf+len, str->str, str->len);
        czl_ws_mask_compute(buf+len, str->len, mask);
    }
    else
        memcpy(buf+len, str->str, str->len);

    if (czl_net_send(gp, sock, buf, len+str->len) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum = str->len;

    CZL_TMP_FREE(gp, buf, 14+str->len);
    return 1;
}

char czl_ws_close(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x88;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}

char czl_ws_ping(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x89;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}

char czl_ws_pong(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x8a;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}
#endif //CZL_LIB_WS

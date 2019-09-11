#include "czl_http.h"

#ifdef CZL_LIB_HTTP

#define CZL_HTTP_BUF_SIZE 50*1024

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_http_doc(czl_gp *gp, czl_fun *fun);    //解析http请求
char czl_http_connect(czl_gp *gp, czl_fun *fun);//连接http服务器
char czl_http_req(czl_gp *gp, czl_fun *fun);    //发送http请求
char czl_http_res(czl_gp *gp, czl_fun *fun);    //发送http响应
char czl_http_kv(czl_gp *gp, czl_fun *fun);     //解析http头部声明键值对

//库函数表定义
const czl_sys_fun czl_lib_http[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"doc",      czl_http_doc,     1,        NULL},
    {"connect",  czl_http_connect, 2,        "str_v1,int_v2=80"},
    {"req",      czl_http_req,     3,        "int_v1,str_v3=\"\""},
    {"res",      czl_http_res,     3,        "int_v1,str_v3=\"\""},
    {"kv",       czl_http_kv,      1,        "str_v1"},
};

unsigned long czl_strchr(const char *s, const char *e, const char ch)
{
    unsigned long cnt = 0;

    while (s < e)
        if (*s++ == ch)
            ++cnt;

    return cnt;
}

void czl_get_form_data(unsigned char *a, const unsigned char *s, const unsigned char *e)
{
    while (s < e)
    {
        if ('%' == *s)
        {
            unsigned char n;
            ++s;
            if (*s >= 'A' && *s <= 'F')
                n = ((*s-'A')+10) * 16;
            else if (*s >= 'a' && *s <= 'f')
                n = ((*s-'a')+10) * 16;
            else
                n = (*s-'0') * 16;
            //
            ++s;
            if (*s >= 'A' && *s <= 'F')
                *a = n + (*s-'A')+10;
            else if (*s >= 'a' && *s <= 'f')
                *a = n + (*s-'a')+10;
            else
                *a = n + (*s-'0');

            ++a; ++s;
        }
        else if ('+' == *s)
        {
            *a++ = ' ';
            ++s;
        }
        else
            *a++ = *s++;
    }
}

void** czl_form_create(czl_gp *gp, char *s, unsigned long len)
{
    void **obj;
    czl_table *tab;
    char *e = s + len;

    //申请一张哈希表格式化存储表单键值对
    if (!(obj=czl_table_create(gp, 12, 0, 0)))
        return 0;
    tab = CZL_TAB(obj);

    //解析表单键值对
    do {
        //usr=czl&pwd=czl&text=hello%2C%5B3
        char *t;
        czl_var key;
        czl_tabkv *ele;
        unsigned long len;
        unsigned long count = tab->count;
        //解析键
        if (!(t=strchr(s, '=')))
            goto CZL_ERROR;
        key.quality = CZL_TMP_VAR;
        if (!czl_str_create(gp, &key.val.str, t-s+1, t-s, s) ||
            !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)) ||
            tab->count == count)
        {
            CZL_SF(gp, key.val.str);
            goto CZL_ERROR;
        }
        s = t+1;
        //解析值
        if (!(t=strchr(s, '&')))
            t = e;
        len = t-s-2*czl_strchr(s, t, '%');
        if (!czl_str_create(gp, &ele->val.str, len+1, len, NULL))
            goto CZL_ERROR;
        czl_get_form_data(CZL_STR(ele->val.str.Obj)->str, s, t);
        ele->type = CZL_STRING;
        s = t+1;
    } while (s < e);

    return obj;

CZL_ERROR:
    czl_table_delete(gp, obj);
    return NULL;
}

void** czl_http_pop(czl_gp *gp, czl_var *data, unsigned char flag)
{
    czl_string *str = CZL_STR(data->val.str.Obj);
    long content_length = 0;
    czl_table *tab;
    czl_tabkv *ele;
    czl_var key;
    char *s, *e;
    void **obj;

    if (flag && str->rc > 1 && !(str=czl_string_fork(gp, data)))
        return NULL;
    s = str->str;
    e = s + str->len;

    //申请一张哈希表格式化存储一次请求
    if (!(obj=czl_table_create(gp, 12, 0, 0)))
        return NULL;
    tab = CZL_TAB(obj);

    //解析http请求
    do {
        //HTTP/1.1 200 OK
        //GET /shell?k1=v1&k2=v2 HTTP/1.1
        //POST /shell HTTP/1.1
        //Content-Type: text/html
        char ch;
        char *t;
        unsigned long count = tab->count;
        //解析键
        if (tab->count <= 1) ch = ('H' == s[0] ? '/' : ' ');
        else ch = ':';
        while (' ' == *s) ++s; //过滤空格
        if (!(t=strchr(s, ch)))
            goto CZL_ERROR;
        key.quality = CZL_TMP_VAR;
        if (!czl_str_create(gp, &key.val.str, t-s+1, t-s, s) ||
            !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)) ||
            tab->count == count)
        {
            CZL_SF(gp, key.val.str);
            goto CZL_ERROR;
        }
        s = t+1;
        //解析值
        ch = (1 == tab->count ? ' ' : '\r');
        while (' ' == *s) ++s; //过滤空格
        if (!(t=strchr(s, ch)))
            goto CZL_ERROR;
        if (0 == content_length && //获取请求数据长度
            czl_strcmp("Content-Length", CZL_STR(key.val.str.Obj)->str))
        {
            if (!czl_get_number_from_str(s, (czl_var*)ele))
                ele->val.inum = 0;
            content_length = ele->val.inum;
        }
        else
        {
            if (!czl_str_create(gp, &ele->val.str, t-s+1, t-s, s))
                goto CZL_ERROR;
            ele->type = CZL_STRING;
        }
        s = t + (1 == tab->count ? 1 : 2);
    } while (s < e && (*s != '\r' || *(s+1) != '\n'));

    if ((*s != '\r' || *(s+1) != '\n'))
        goto CZL_ERROR;
    else
        s += 2; //\r\n

    if (content_length)
    {
        unsigned long count = tab->count;
        if (e-s < content_length)
            goto CZL_ERROR;
        key.quality = CZL_TMP_VAR;
        if (!czl_str_create(gp, &key.val.str, 5, 4, "form") ||
            !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)) ||
            tab->count == count)
        {
            CZL_SF(gp, key.val.str);
            goto CZL_ERROR;
        }
        if ('H' == str->str[0]) //html
        {
            if (!czl_str_create(gp, &ele->val.str, content_length+1, content_length, s))
                goto CZL_ERROR;
            ele->type = CZL_STRING;
        }
        else //表单
        {
            if (!(ele->val.Obj=czl_form_create(gp, s, content_length)))
                goto CZL_ERROR;
            ele->type = CZL_TABLE;
        }
        s += content_length;
    }

    if (flag)
    {
        if (s == e)
        {
            CZL_SF(gp, data->val.str);
            data->type = CZL_INT;
            data->val.inum = 0;
        }
        else
        {
            //清空s前面的数据，保留不全的请求
            str->len = e - s;
            memcpy(str->str, s, str->len);
            str->str[str->len] = '\0';
        }
    }
    return obj;

CZL_ERROR:
    czl_table_delete(gp, obj);
    return NULL;
}

char czl_http_doc(czl_gp *gp, czl_fun *fun)
{
    if (fun->vars->type != CZL_STRING || !(fun->ret.val.Obj=czl_http_pop(gp, fun->vars, 0)))
        return (CZL_EXCEPTION_OUT_OF_MEMORY == gp->exceptionCode ? 0 : 1);
    fun->ret.type = CZL_TABLE;
    return 1;
}

char czl_http_connect(czl_gp *gp, czl_fun *fun)
{
    return czl_tcp_connect(gp, fun);
}

unsigned long czl_key_num_filt(const czl_string *str)
{
    const char *s = str->str;
    int i = str->len - 1;

    while (i >= 0 && s[i] >= '0' && s[i] <= '9')
        --i;

    return i+1;
}

unsigned long CZL_HTTP_KEY_ENCODE
(
    unsigned char type,
    czl_value val,
    char *buf,
    unsigned long len,
    unsigned char sign
)
{
    char tmp[32];
    char *str = tmp;
    unsigned long cnt;

    if (CZL_STRING == type)
    {
        czl_string *s = CZL_STR(val.str.Obj);
        cnt = czl_key_num_filt(s);
        str = s->str;
    }
    else if (CZL_INT == type)
        cnt = czl_itoa(val.inum, tmp);
    else if (CZL_FLOAT == type)
        cnt = czl_ftoa(val.fnum, tmp, 1);
    else
        cnt = 0;

    memcpy(buf+len, str, cnt);
    len += cnt;
    buf[len++] = sign;
    return len;
}

unsigned long CZL_HTTP_ENCODE
(
    unsigned char type,
    czl_value val,
    char *buf,
    unsigned long len,
    unsigned char sign
)
{
    char tmp[32];
    char *str = tmp;
    unsigned long cnt;

    if (CZL_STRING == type)
    {
        czl_string *s = CZL_STR(val.str.Obj);
        cnt = s->len;
        str = s->str;
    }
    else if (CZL_INT == type)
        cnt = czl_itoa(val.inum, tmp);
    else if (CZL_FLOAT == type)
        cnt = czl_ftoa(val.fnum, tmp, 1);
    else
        cnt = 0;

    memcpy(buf+len, str, cnt);
    len += cnt;
    buf[len++] = sign;
    return len;
}

char czl_http_req(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_var *header = fun->vars+1;
    czl_string *datas = CZL_STR(fun->vars[2].val.str.Obj);
    char *host = NULL;
    unsigned short port;
    char buf[CZL_HTTP_BUF_SIZE];
    long len = 0;

    fun->ret.type = CZL_INT;

    if (CZL_STRING == header->type)
    {
        czl_string *s = CZL_STR(header->val.str.Obj);
        char *http = strstr(s->str, "HTTP");
        if (!http)
            return 1;
        port = ('S' == http[4] ? 443 : 80);
        memcpy(buf, s->str, s->len);
        len = s->len;
    }
    else if (CZL_TABLE == header->type)
    {
        czl_table *tab = CZL_TAB(header->val.Obj);
        czl_tabkv *p = tab->eles_head;
        //http请求必须包含: 请求方式, 协议版本, 目标主机域名
        if (tab->count < 3)
            return 1;
        //封装请求方式
        len = CZL_HTTP_ENCODE(p->kt, p->key, buf, len, ' ');
        len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, ' ');
        p = p->next;
        //封装协议版本
        len = CZL_HTTP_ENCODE(p->kt, p->key, buf, len, '/');
        port = (CZL_STRING == p->kt && czl_strcmp("http", CZL_STR(p->key.str.Obj)->str) ? 80 : 443);
        len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, '\r');
        buf[len++] = '\n';
        p = p->next;
        //封装请求字段集声明
        do {
            len = CZL_HTTP_KEY_ENCODE(p->kt, p->key, buf, len, ':');
            len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, '\r');
            buf[len++] = '\n';
            if (CZL_STRING == p->kt && CZL_STRING == p->type &&
                czl_strcmp("host", CZL_STR(p->key.str.Obj)->str))
                host = CZL_STR(p->val.str.Obj)->str;
            else if (CZL_STRING == p->kt && CZL_INT == p->type &&
                     czl_strcmp("port", CZL_STR(p->key.str.Obj)->str))
                port = p->val.inum;
        } while ((p=p->next));
    }
    else
        return 1;

    if (datas->len)
    {
        //数据长度声明
        strcpy(buf+len, "Content-Length:");
        len += 15;
        len += czl_itoa(datas->len, buf+len);
        buf[len++] = '\r';
        buf[len++] = '\n';
    }

    //结尾换行
    buf[len++] = '\r';
    buf[len++] = '\n';

    //发送响应
    if (len+datas->len <= CZL_HTTP_BUF_SIZE)
    {
        memcpy(buf+len, datas->str, datas->len);
        if ((fun->ret.val.inum=czl_net_send(gp, sock, buf, len+datas->len)) < 0)
             return 1;
    }
    else if ((fun->ret.val.inum=czl_net_send(gp, sock, buf, len)) < 0)
        return 1;
    else if (datas->len && (len=czl_net_send(gp, sock, datas->str, datas->len)) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum += len;

    return 1;
}

char czl_http_res(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_var *header = fun->vars+1;
    czl_string *datas = CZL_STR(fun->vars[2].val.str.Obj);
    char buf[CZL_HTTP_BUF_SIZE];
    long len = 0;

    fun->ret.type = CZL_INT;

    if (CZL_STRING == header->type)
    {
        czl_string *s = CZL_STR(header->val.str.Obj);
        memcpy(buf, s->str, s->len);
        len = s->len;
    }
    else if (CZL_TABLE == header->type)
    {
        czl_table *tab = CZL_TAB(header->val.Obj);
        czl_tabkv *p = tab->eles_head;
        //http响应必须包含: 协议版本, 响应码
        if (tab->count < 2)
            return 1;
        //封装协议版本
        len = CZL_HTTP_ENCODE(p->kt, p->key, buf, len, '/');
        len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, ' ');
        p = p->next;
        //封装响应码
        len = CZL_HTTP_ENCODE(p->kt, p->key, buf, len, ' ');
        len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, '\r');
        buf[len++] = '\n';
        p = p->next;
        //封装响应字段集声明
        while (p)
        {
            len = CZL_HTTP_KEY_ENCODE(p->kt, p->key, buf, len, ':');
            len = CZL_HTTP_ENCODE(p->type, p->val, buf, len, '\r');
            buf[len++] = '\n';
            p = p->next;
        }
    }
    else
        return 1;

    if (datas->len)
    {
        //数据长度声明
        strcpy(buf+len, "Content-Length:");
        len += 15;
        len += czl_itoa(datas->len, buf+len);
        buf[len++] = '\r';
        buf[len++] = '\n';
    }

    //结尾换行
    buf[len++] = '\r';
    buf[len++] = '\n';

    //发送响应
    if (len+datas->len <= CZL_HTTP_BUF_SIZE)
    {
        memcpy(buf+len, datas->str, datas->len);
        if ((fun->ret.val.inum=czl_net_send(gp, sock, buf, len+datas->len)) < 0)
             return 1;
    }
    else if ((fun->ret.val.inum=czl_net_send(gp, sock, buf, len)) < 0)
        return 1;
    else if (datas->len && (len=czl_net_send(gp, sock, datas->str, datas->len)) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum += len;

    return 1;
}

char czl_http_kv(czl_gp *gp, czl_fun *fun)
{
    czl_string *str = CZL_STR(fun->vars->val.Obj);
    char *s = str->str, *e = str->str + str->len;
    void **obj;
    czl_table *tab;

    //申请一张哈希表格式化存储键值对
    if (!(obj=czl_table_create(gp, 12, 0, 0)))
        return 0;
    tab = CZL_TAB(obj);

    //解析键值对
    do {
        // uid=czl; sid=123
        char *t;
        czl_var key;
        czl_tabkv *ele;
        unsigned long count = tab->count;
        //解析键
        while (' ' == *s) ++s; //过滤空格
        if (!(t=strchr(s, '=')))
            goto CZL_END;
        key.quality = CZL_TMP_VAR;
        if (!czl_str_create(gp, &key.val.str, t-s+1, t-s, s) ||
            !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)))
        {
            CZL_SF(gp, key.val.str);
            goto CZL_OOM;
        }
        if (tab->count == count) //字段重复认为不是有效数据丢弃
        {
            CZL_SF(gp, key.val.str);
            goto CZL_END;
        }
        s = t+1;
        //解析值
        while (' ' == *s) ++s; //过滤空格
        if (!(t=strchr(s, ';')))
            t = e;
        if (!czl_str_create(gp, &ele->val.str, t-s+1, t-s, s))
            goto CZL_OOM;
        ele->type = CZL_STRING;
        s = t+1;
    } while (s < e);

    fun->ret.val.Obj = obj;
    fun->ret.type = CZL_TABLE;
    return 1;

CZL_END:
    czl_table_delete(gp, obj);
    return 1;

CZL_OOM:
    czl_table_delete(gp, obj);
    return 0;
}
#endif //CZL_LIB_HTTP

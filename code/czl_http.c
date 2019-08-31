#include "czl_http.h"

#ifdef CZL_LIB_HTTP

#define CZL_HTTP_BUF_SIZE 10*1024

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_http_req(czl_gp *gp, czl_fun *fun);    //发送http请求
char czl_http_res(czl_gp *gp, czl_fun *fun);    //发送http响应
char czl_http_doc(czl_gp *gp, czl_fun *fun);    //解析http请求
char czl_http_form(czl_gp *gp, czl_fun *fun);   //解析http表单数据键值对
char czl_http_kv(czl_gp *gp, czl_fun *fun);     //解析http头部声明键值对

//库函数表定义
const czl_sys_fun czl_lib_http[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"req",      czl_http_req,     3,        "int_v1,map_v2,str_v3=\"\""},
    {"res",      czl_http_res,     3,        "int_v1,map_v2,str_v3=\"\""},
    {"doc",      czl_http_doc,     1,        "&str_v1"},
    {"form",     czl_http_form,    1,        "str_v1"},
    {"kv",       czl_http_kv,      1,        "str_v1"},
};

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
    czl_table *tab = CZL_TAB(fun->vars[1].val.Obj);
    czl_string *datas = CZL_STR(fun->vars[2].val.str.Obj);
    czl_tabkv *p = tab->eles_head;
    char *host = NULL;
    unsigned short port;
    unsigned char sockFlag = 0;
    char buf[CZL_HTTP_BUF_SIZE];
    long len = 0;

    fun->ret.val.inum = 0;

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

    //数据长度声明
    strcpy(buf+len, "Content-Length:");
    len += 15;
    len += czl_itoa(datas->len, buf+len);
    buf[len++] = '\r';
    buf[len++] = '\n';

    //结尾换行
    buf[len++] = '\r';
    buf[len++] = '\n';

    //http1.1后支持keep-alive，这里检测是否有可用的tcp连接
    if (sock <= 0)
    {
        struct sockaddr_in service;
        if (!(host=czl_dns(host)))
            return 1;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = inet_addr(host);
        service.sin_port = htons(port);
        if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0 ||
            SOCKET_ERROR == connect(sock, (struct sockaddr*)&service, sizeof(service)))
        {
        #ifdef CZL_SYSTEM_WINDOWS
            closesocket(sock);
        #else //CZL_SYSTEM_LINUX
            close(sock);
        #endif
            return 1;
        }
    }
    else
        sockFlag = 1;

    //发送请求
    if ((len=czl_net_send(gp, sock, buf, len)) < 0)
        fun->ret.val.inum = -1;
    else if (datas->len && (len=czl_net_send(gp, sock, datas->str, datas->len)) < 0)
        fun->ret.val.inum = -1;
    else if (sockFlag)
        fun->ret.val.inum = len;
    else
        fun->ret.val.inum = sock;

    return 1;
}

char czl_http_res(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_table *tab = CZL_TAB(fun->vars[1].val.Obj);
    czl_string *datas = CZL_STR(fun->vars[2].val.str.Obj);
    czl_tabkv *p = tab->eles_head;
    char buf[CZL_HTTP_BUF_SIZE];
    long len = 0;

    fun->ret.val.inum = 0;

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

    //数据长度声明
    strcpy(buf+len, "Content-Length:");
    len += 15;
    len += czl_itoa(datas->len, buf+len);
    buf[len++] = '\r';
    buf[len++] = '\n';

    //结尾换行
    buf[len++] = '\r';
    buf[len++] = '\n';

    //发送响应
    if ((len=czl_net_send(gp, sock, buf, len)) < 0)
        fun->ret.val.inum = -1;
    else if (datas->len && (len=czl_net_send(gp, sock, datas->str, datas->len)) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum = len;

    return 1;
}

char czl_recv_data(czl_gp *gp, czl_array *arr, char *data, unsigned long len)
{
    if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
        return 0;

    if (arr->cnt >= 1 && CZL_STRING == arr->vars[arr->cnt-1].type)
    {
        czl_string *s;
        czl_var *var = arr->vars + arr->cnt - 1;
        void **obj = CZL_SR(gp, var->val.str, len+var->val.str.size);
        if (!obj)
            return 0;
        var->val.str.Obj = obj;
        var->val.str.size += len;
        s = CZL_STR(obj);
        memcpy(s->str + s->len, data, len);
        s->len += len;
        s->str[s->len] = '\0';
    }
    else
    {
        if (!czl_str_create(gp, &arr->vars[arr->cnt].val.str, len+1, len, data))
            return 0;
        arr->vars[arr->cnt++].type = CZL_STRING;
    }

    return 1;
}

char* czl_req_check(char *s, char *b, char *p)
{
    unsigned long i;
    const unsigned char lens[] = {3, 4, 4, 3, 5, 6, 6, 7};
    const char *methods[] = {"GET", "POST", "HEAD", //http必须包含前三项
                             "PUT", "TRACE", "OPIONS", "DELETE", "CONNECT"};

    if (b-s >= 8 && memcmp("HTTP", b-8, 4) == 0)
        return b-8;

    for (i = 0; i < 8; ++i)
        if (b-s >= lens[i] && memcmp(methods[i], b-lens[i], lens[i]) == 0)
            return b-lens[i];

    return p;
}

char czl_http_doc(czl_gp *gp, czl_fun *fun)
{
    czl_var *data = CZL_GRV(fun->vars);
    czl_string *str = CZL_STR(data->val.str.Obj);
    void **obj = czl_array_create(gp, 4, 0);
    czl_array *arr;
    char *s, *e;
    long content_length = 0;

    if (!obj)
        return 0;
    arr = CZL_ARR(obj);

    if (str->rc > 1 && !(str=czl_string_fork(gp, data)))
        return 0;
    s = str->str;
    e = s + str->len;

    while (s < e)
    {
        char *b = NULL; //第一个空格符
        char *p = s;
        char *q;        //用于接收html
        czl_table *tab;

        //有Content-Length字段声明接收固定长度数据
        if (content_length)
        {
            if (e-s < content_length)
                content_length = e-s;
            if (!czl_recv_data(gp, arr, s, content_length))
                goto CZL_OOM;
            if ((s+=content_length) == e)
                break;
            content_length = 0;
            p = s;
        }

        //考虑TCP缓冲区大小有限制可能导致数据不完整问题，这里需要定位\r\n\r\n和第一个空格符
        while (p < e)
        {
            if ('\r' == *p && '\n' == *(p+1) && '\r' == *(p+2) && '\n' == *(p+3))
                break;
            else if (!b && ' ' == *p)
                b = p;
            ++p;
        }

        //一个不完整的请求或数据
        if (p == e || !b)
        {
            if (b) p = czl_req_check(s, b, p);
            if (p == e && s == str->str)
            {
                //这个包全部是数据
                arr->vars[arr->cnt].val.str = data->val.str;
                arr->vars[arr->cnt++].type = CZL_STRING;
                data->type = CZL_INT;
                data->val.inum = 0;
                fun->ret.type = CZL_ARRAY;
                fun->ret.val.Obj = obj;
                return 1;
            }
            if (p != s)
            {
                if (!czl_recv_data(gp, arr, s, p-s))
                    goto CZL_OOM;
                s = p;
            }
            break;
        }

        //请求前有上次没接收完的数据
        if (b && (q=czl_req_check(s, b, s)) != s)
        {
            if (!czl_recv_data(gp, arr, s, q-s))
                goto CZL_OOM;
            s = q;
        }

        //自动增长数组存储请求队列
        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            goto CZL_OOM;

        //申请一张哈希表格式化存储一次请求
        if (!(arr->vars[arr->cnt].val.Obj=czl_table_create(gp, 12, 0, 0)))
            goto CZL_OOM;
        tab = CZL_TAB(arr->vars[arr->cnt].val.Obj);
        arr->vars[arr->cnt++].type = CZL_TABLE;

        //解析http请求
        do {
            //HTTP/1.1 200 OK
            //GET /shell?k1=v1&k2=v2 HTTP/1.1
            //POST /shell HTTP/1.1
            //Content-Type: text/html
            char ch;
            char *t;
            czl_var key;
            czl_tabkv *ele;
            unsigned long count = tab->count;
            //解析键
            if (tab->count <= 1) ch = ('H' == s[0] ? '/' : ' ');
            else ch = ':';
            while (' ' == *s) ++s; //过滤空格
            if (!(t=strchr(s, ch)))
                goto CZL_HTML;
            key.quality = CZL_TMP_VAR;
            if (!czl_str_create(gp, &key.val.str, t-s+1, t-s, s) ||
                !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)))
            {
                CZL_SF(gp, key.val.str);
                goto CZL_OOM;
            }
            if (tab->count == count) //字段重复认为是html
            {
                CZL_SF(gp, key.val.str);
                goto CZL_HTML;
            }
            s = t+1;
            //解析值
            ch = (1 == tab->count ? ' ' : '\r');
            while (' ' == *s) ++s; //过滤空格
            if (!(t=strchr(s, ch)))
                goto CZL_HTML;
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
                    goto CZL_OOM;
                ele->type = CZL_STRING;
            }
            s = t + (1 == tab->count ? 1 : 2);
        } while (s < e && (*s != '\r' || *(s+1) != '\n'));

        s += 2; //\r\n
        continue;

    CZL_HTML:
        //释放误析请求
        czl_table_delete(gp, arr->vars[--arr->cnt].val.Obj);
        arr->vars[arr->cnt].type = CZL_INT;
        s = p+4;
        //接收html
        if (!czl_recv_data(gp, arr, q, s-q))
            goto CZL_OOM;
    }

    if (s >= e)
    {
        CZL_SF(gp, data->val.str);
        data->type = CZL_INT;
        data->val.inum = 0;
    }
    else if (s != str->str)
    {
        //清空s前面的数据，保留不全的请求
        str->len = e - s;
        str->str[str->len] = '\0';
        memcpy(str->str, s, str->len);
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;

CZL_OOM:
    czl_array_delete(gp, obj);
    return 0;
}

unsigned long czl_strchr(const char *s, const char *e, const char ch)
{
    unsigned long cnt = 0;

    while (s < e)
        if (*s++ == ch)
            ++cnt;

    return cnt;
}

void czl_strcpy(unsigned char *a, const unsigned char *s, const unsigned char *e)
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

char czl_http_form(czl_gp *gp, czl_fun *fun)
{
    czl_string *str = CZL_STR(fun->vars->val.Obj);
    char *s = str->str, *e = str->str + str->len;
    void **obj;
    czl_table *tab;

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
            goto CZL_END;
        key.quality = CZL_TMP_VAR;
        if (!czl_str_create(gp, &key.val.str, t-s+1, t-s, s) ||
            !(ele=czl_create_key_str_tabkv(gp, tab, &key, NULL, 1)))
        {
            CZL_SF(gp, key.val.str);
            goto CZL_OOM;
        }
        if (tab->count == count) //字段重复认为不是表单数据丢弃
        {
            CZL_SF(gp, key.val.str);
            goto CZL_END;
        }
        s = t+1;
        //解析值
        if (!(t=strchr(s, '&')))
            t = e;
        len = t-s-2*czl_strchr(s, t, '%');
        if (!czl_str_create(gp, &ele->val.str, len+1, len, NULL))
            goto CZL_OOM;
        czl_strcpy(CZL_STR(ele->val.str.Obj)->str, s, t);
        ele->type = CZL_STRING;
        s = t+1;
    } while (s < e);

    fun->ret.val.Obj = obj;
    fun->ret.type = CZL_TABLE;
    return 1;

CZL_END:
    fun->ret.val.inum = 0;
    czl_table_delete(gp, obj);
    return 1;

CZL_OOM:
    czl_table_delete(gp, obj);
    return 0;
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
    fun->ret.val.inum = 0;
    czl_table_delete(gp, obj);
    return 1;

CZL_OOM:
    czl_table_delete(gp, obj);
    return 0;
}
#endif //CZL_LIB_HTTP

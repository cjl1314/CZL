#include "czl_http.h"

#ifdef CZL_LIB_HTTP

#define CZL_HTTP_BUF_SIZE 10*1024

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_http_req(czl_gp *gp, czl_fun *fun);    //发送http请求
char czl_http_res(czl_gp *gp, czl_fun *fun);    //发送http响应
char czl_http_doc(czl_gp *gp, czl_fun *fun);    //解析http请求

//库函数表定义
const czl_sys_fun czl_lib_http[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"req",      czl_http_req,     3,        "int_v1,map_v2,str_v3=\"\""},
    {"res",      czl_http_res,     3,        "int_v1,map_v2,str_v3=\"\""},
    {"doc",      czl_http_doc,     1,        "&str_v1"},
};

#define CZL_HTTP_ENCODE(s, obj, buf, len, sign) { \
    s = CZL_STR(obj); \
    memcpy(buf+len, s->str, s->len); \
    len += s->len; \
    buf[len++] = sign; \
}

char* czl_dns(char *domain)
{
    struct hostent *host;

    if (!domain)
        return NULL;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return NULL;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if (!(host=gethostbyname(domain)))
        return NULL;

    return inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
}

long czl_send(czl_gp *gp, SOCKET sock, char *buf, long len)
{
    long cnt = 0;
    long ret = send(sock, buf, len, 0);

    if (ret == len)
        return ret;
    else if (ret > 0)
    {
        cnt += ret;
        len -= ret;
    }

#ifdef CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLOUT;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0)
        return cnt;
#endif

    do {
    #ifdef CZL_SYSTEM_WINDOWS
        fd_set fdWrite;
        FD_ZERO(&fdWrite); FD_SET(sock, &fdWrite);
        if (select(0, NULL, &fdWrite, NULL, NULL) < 0)
            break;
    #else //CZL_SYSTEM_LINUX
        if (epoll_wait(gp->kdpfd, events, 1, -1) < 0)
            break;
    #endif
        ret = send(sock, buf+cnt, len, 0);
        if (SOCKET_ERROR == ret)
        {
        #ifdef CZL_SYSTEM_WINDOWS
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                break;
            else
            {
                closesocket(sock);
                return -1;
            }
        #else //CZL_SYSTEM_LINUX
            extern int errno;
            if (EWOULDBLOCK == errno || EAGAIN == errno)
                break;
            else
            {
                cnt = -1;
                break;
            }
        #endif
        }
        cnt += ret;
        len -= ret;
    } while (len);

#ifdef CZL_SYSTEM_LINUX
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
    if (-1 == cnt)
        close(sock);
#endif

    return cnt;
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
    czl_string *s;

    fun->ret.val.inum = 0;

    //http请求必须包含: 请求方式, 协议版本, 目标主机域名
    if (tab->count < 3 ||
        p->kt != CZL_STRING || p->type != CZL_STRING ||
        p->next->kt != CZL_STRING || p->next->type != CZL_STRING)
        return 1;

    //封装请求方式
    CZL_HTTP_ENCODE(s, p->key.str.Obj, buf, len, ' ');
    CZL_HTTP_ENCODE(s, p->val.str.Obj, buf, len, ' ')
    p = p->next;

    //封装协议版本
    CZL_HTTP_ENCODE(s, p->key.str.Obj, buf, len, '/');
    port = (czl_strcmp("http", s->str) ? 80 : 443);
    CZL_HTTP_ENCODE(s, p->val.str.Obj, buf, len, '\r');
    buf[len++] = '\n';
    p = p->next;

    //封装请求字段集声明
    do {
        czl_string *k, *v;
        if (p->kt != CZL_STRING || p->type != CZL_STRING)
            continue;
        CZL_HTTP_ENCODE(k, p->key.str.Obj, buf, len, ':');
        CZL_HTTP_ENCODE(v, p->val.str.Obj, buf, len, '\r');
        buf[len++] = '\n';
        if (czl_strcmp("host", k->str))
            host = v->str;
    } while ((p=p->next));

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
    if ((len=czl_send(gp, sock, buf, len)) < 0)
        fun->ret.val.inum = -1;
    else if (datas->len && (len=czl_send(gp, sock, datas->str, datas->len)) < 0)
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
    czl_string *s;

    fun->ret.val.inum = 0;

    //http响应必须包含: 协议版本, 响应码
    if (tab->count < 2 ||
        p->kt != CZL_STRING || p->type != CZL_STRING ||
        p->next->kt != CZL_STRING || p->next->type != CZL_STRING)
        return 1;

    //封装协议版本
    CZL_HTTP_ENCODE(s, p->key.str.Obj, buf, len, '/');
    CZL_HTTP_ENCODE(s, p->val.str.Obj, buf, len, ' ');
    p = p->next;

    //封装响应码
    CZL_HTTP_ENCODE(s, p->key.str.Obj, buf, len, ' ');
    CZL_HTTP_ENCODE(s, p->val.str.Obj, buf, len, '\r');
    buf[len++] = '\n';
    p = p->next;

    //封装响应字段集声明
    while (p)
    {
        if (CZL_STRING == p->kt && CZL_STRING == p->type)
        {
            CZL_HTTP_ENCODE(s, p->key.str.Obj, buf, len, ':');
            CZL_HTTP_ENCODE(s, p->val.str.Obj, buf, len, '\r');
            buf[len++] = '\n';
        }
        p = p->next;
    }

    buf[len++] = '\r';
    buf[len++] = '\n';

    //发送响应
    if ((len=czl_send(gp, sock, buf, len)) < 0)
        fun->ret.val.inum = -1;
    else if (datas->len && (len=czl_send(gp, sock, datas->str, datas->len)) < 0)
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
            if (!(t=strchr(s, ch)))
                goto CZL_HTML;
            key.quality = CZL_ARRBUF_VAR;
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
#endif //CZL_LIB_HTTP

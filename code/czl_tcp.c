#include "czl_tcp.h"

#ifdef CZL_LIB_TCP

#ifdef CZL_LIB_HTTP
#include "czl_http.h" //HTTP库
#endif //CZL_LIB_HTTP

#ifdef CZL_LIB_WS
#include "czl_ws.h" //WS库
#endif //CZL_LIB_WS

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_tcp_server(czl_gp *gp, czl_fun *fun);  //创建服务器socket
char czl_tcp_fork(czl_gp *gp, czl_fun *fun);    //fork一台服务器
char czl_tcp_connect(czl_gp *gp, czl_fun *fun); //连接服务器并获得socket
char czl_tcp_sock(czl_gp *gp, czl_fun *fun);    //获取服务器sock
char czl_tcp_listen(czl_gp *gp, czl_fun *fun);  //服务器侦听新客户端并获取socket
char czl_tcp_close(czl_gp *gp, czl_fun *fun);   //获取服务器close
char czl_tcp_recv(czl_gp *gp, czl_fun *fun);    //从socket接收数据
char czl_tcp_send(czl_gp *gp, czl_fun *fun);    //向socket发送数据
char czl_tcp_event(czl_gp *gp, czl_fun *fun);   //注册tcp服务器事件
char czl_tcp_limit(czl_gp *gp, czl_fun *fun);   //设置接收缓冲区大小
char czl_tcp_ip(czl_gp *gp, czl_fun *fun);      //获取socket的ip地址

//库函数表定义
const czl_sys_fun czl_lib_tcp[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"server",   czl_tcp_server,   3,        "str_v1,int_v2,int_v3=-1"},
    {"fork",     czl_tcp_fork,     2,        "int_v1,int_v2=-1"},
    {"connect",  czl_tcp_connect,  2,        "str_v1,int_v2"},
    {"sock",     czl_tcp_sock,     1,        "src_v1"},
    {"listen",   czl_tcp_listen,   2,        "src_v1,int_v2=-1"},
    {"close",    czl_tcp_close,    2,        "src_v1,int_v2"},
    {"recv",     czl_tcp_recv,     2,        "src_v1,int_v2=-1"},
    {"send",     czl_tcp_send,     2,        "src_v1,str_v2"},
    {"event",    czl_tcp_event,    3,        "src_v1,str_v2,fun_v3"},
    {"limit",    czl_tcp_limit,    2,        "src_v1,int_v2"},
    {"ip",       czl_tcp_ip,       1,        NULL},
};

#define CZL_TCP_DEFAULT_FD_NUM 1000000

enum czl_tcp_event_type_enum
{
    CZL_TCP_EVENT_TCP,
    CZL_TCP_EVENT_HTTP,
    CZL_TCP_EVENT_WS
};

void czl_tcp_free(czl_tcp_handler *h)
{
    if (h->type != CZL_TCP_SRV_WORKER)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(h->sock);
    #else //CZL_SYSTEM_LINUX
        close(h->sock);
    #endif
    }

    if (CZL_TCP_SRV_MASTER == h->type || CZL_TCP_SRV_WORKER == h->type)
    {
        czl_tabkv *p;
        for (p = CZL_TAB(h->obj)->eles_head; p; p = p->next)
        {
        #ifdef CZL_SYSTEM_WINDOWS
            closesocket(p->key.inum);
        #else //CZL_SYSTEM_LINUX
            epoll_ctl(h->kdpfd, EPOLL_CTL_DEL, p->key.inum, NULL);
            close(p->key.inum);
        #endif
        }
    #ifdef CZL_SYSTEM_LINUX
        close(h->kdpfd);
    #endif
        if (!h->gp->end_flag)
        {
            czl_table_delete(h->gp, h->obj);
        #ifdef CZL_SYSTEM_LINUX
            CZL_TMP_FREE(h->gp, h->events, h->count*sizeof(struct epoll_event));
        #endif
        }
    }

    if (!h->gp->end_flag)
        CZL_TMP_FREE(h->gp, h, sizeof(czl_tcp_handler));
}

unsigned char czl_tcp_busy = 0; //用于避免accept惊群现象

#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION czl_tcp_cs;
#else //CZL_SYSTEM_LINUX
    pthread_mutex_t czl_tcp_mutex;
#endif

void czl_tcp_lock(void)
{
#ifdef CZL_SYSTEM_WINDOWS
    EnterCriticalSection(&czl_tcp_cs);
#else //CZL_SYSTEM_LINUX
    pthread_mutex_lock(&czl_tcp_mutex);
#endif
}

void czl_tcp_unlock(void)
{
#ifdef CZL_SYSTEM_WINDOWS
    LeaveCriticalSection(&czl_tcp_cs);
#else //CZL_SYSTEM_LINUX
    pthread_mutex_unlock(&czl_tcp_mutex);
#endif
}

char czl_tcp_server_create(czl_gp *gp, czl_tcp_handler *h)
{
    if (!(h->obj=czl_table_create(gp, 0, 0, 0)))
    {
        czl_tcp_free(h);
        return 0;
    }

    if (h->maxFd < 0)
        h->maxFd = CZL_TCP_DEFAULT_FD_NUM;

#ifdef CZL_SYSTEM_WINDOWS
    h->last = NULL;
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = h->sock;
    if ((h->kdpfd=epoll_create(h->maxFd)) < 0 ||
        (CZL_TCP_SRV_MASTER == h->type &&
         epoll_ctl(h->kdpfd, EPOLL_CTL_ADD, h->sock, &ev) < 0))
    {
        czl_tcp_free(h);
        return 0;
    }
#endif

    return 1;
}

//库函数定义
char czl_tcp_server(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    struct sockaddr_in service;
    SOCKET sock;
    unsigned long type = 1; //非阻塞模式
    long maxFd = fun->vars[2].val.inum;
    char *ip = czl_dns(CZL_STR(fun->vars[0].val.str.Obj)->str);
    unsigned long port = fun->vars[1].val.inum;

    if (!ip) return 1;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);

    if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0 ||
    #ifdef CZL_SYSTEM_WINDOWS
        SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &type) ||
    #else //CZL_SYSTEM_LINUX
        SOCKET_ERROR == setsockopt(sock, SOL_SOCKET,
                               #ifdef CZL_TCP_REUSEPORT
                                   SO_REUSEPORT,
                               #else
                                   SO_REUSEADDR,
                               #endif
                                   &type, sizeof(int)) ||
    #endif
        SOCKET_ERROR == bind(sock, (struct sockaddr*)&service, sizeof(service)) ||
        SOCKET_ERROR == listen(sock, 5) ||
        !(h=(czl_tcp_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_tcp_handler))))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    h->gp = gp;
    h->sock = sock;

    if (!maxFd)
        h->type = CZL_TCP_SRV;
    else
    {
        h->type = CZL_TCP_SRV_MASTER;
        h->maxFd = maxFd;
        h->limit = 0;
        h->obj = NULL;
        h->login = h->leave = h->block = h->tcp = NULL;
    #ifdef CZL_LIB_HTTP
        h->http = NULL;
    #endif
    #ifdef CZL_LIB_WS
        h->ws = NULL;
    #endif
    #ifdef CZL_SYSTEM_WINDOWS
        h->last = NULL;
    #else //CZL_SYSTEM_LINUX
        h->kdpfd = -1;
        if (!(h->events=(struct epoll_event*)CZL_TMP_MALLOC(gp, sizeof(struct epoll_event))))
        {
            czl_tcp_free(h);
            return 0;
        }
        h->count = 1;
    #endif
        if (!czl_tcp_server_create(gp, h))
            return 0;
    }

    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_tcp_free,
                                             CZL_LIB_TCP_NAME, czl_lib_tcp)))
    {
        czl_tcp_free(h);
        return 0;
    }

    fun->ret.type = CZL_SOURCE;
    return 1;
}

char czl_tcp_fork(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h = (czl_tcp_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_tcp_handler));

    if (!h)
        return 0;

    h->gp = gp;
    h->sock = fun->vars->val.inum;
    h->type = CZL_TCP_SRV_WORKER;
    h->maxFd = fun->vars[1].val.inum;
    h->limit = 0;
    h->obj = NULL;
    h->login = h->leave = h->block = h->tcp = NULL;
#ifdef CZL_LIB_HTTP
    h->http = NULL;
#endif
#ifdef CZL_LIB_WS
    h->ws = NULL;
#endif
#ifdef CZL_SYSTEM_WINDOWS
    h->last = NULL;
#else //CZL_SYSTEM_LINUX
    h->kdpfd = -1;
    if (!(h->events=(struct epoll_event*)CZL_TMP_MALLOC(gp, sizeof(struct epoll_event))))
    {
        czl_tcp_free(h);
        return 0;
    }
    h->count = 1;
#endif

    if (!czl_tcp_server_create(gp, h))
        return 0;

    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_tcp_free,
                                             CZL_LIB_TCP_NAME, czl_lib_tcp)))
    {
        czl_tcp_free(h);
        return 0;
    }

    fun->ret.type = CZL_SOURCE;
    return 1;
}

char czl_tcp_connect(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    struct sockaddr_in service;
    SOCKET sock;
    char *ip = czl_dns(CZL_STR(fun->vars[0].val.str.Obj)->str);

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if (!ip) return 1;

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(fun->vars[1].val.inum);

    if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0 ||
        SOCKET_ERROR == connect(sock, (struct sockaddr*)&service, sizeof(service)) ||
        !(h=(czl_tcp_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_tcp_handler))))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    h->gp = gp;
    h->sock = sock;
    h->type = CZL_TCP_CLIENT;
    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_tcp_free,
                                             CZL_LIB_TCP_NAME, czl_lib_tcp)))
    {
        czl_tcp_free(h);
        return 0;
    }

    fun->ret.type = CZL_SOURCE;
    return 1;
}

char czl_tcp_sock(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);

    if (!extsrc)
        return 1;

    fun->ret.type = CZL_INT;
    fun->ret.val.inum = ((czl_tcp_handler*)extsrc->src)->sock;
    return 1;
}

char czl_tcp_listen_0(czl_gp *gp, czl_fun *fun, SOCKET sock)
{
    czl_tcp_handler *h;
    long time = fun->vars[1].val.inum;
#ifdef CZL_SYSTEM_WINDOWS
    fd_set fdRead;
    struct timeval timeout, *pTime = NULL;
#endif

#ifdef CZL_SYSTEM_WINDOWS
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(sock+1, &fdRead, NULL, NULL, pTime) <= 0)
        sock = 0;
    else if (FD_ISSET(sock, &fdRead))
        sock = accept(sock, NULL, NULL);
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        sock = 0;
    }
    else
    {
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        sock = accept(sock, NULL, NULL);
    }
#endif

    if (!sock)
        return 1;

    if (!(h=(czl_tcp_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_tcp_handler))))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 0;
    }

    h->gp = gp;
    h->sock = sock;
    h->type = CZL_TCP_CLIENT;
    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_tcp_free,
                                             CZL_LIB_TCP_NAME, czl_lib_tcp)))
    {
        czl_tcp_free(h);
        return 0;
    }

    fun->ret.type = CZL_SOURCE;
    return 1;
}

#ifdef CZL_LIB_HTTP
char czl_http_event_loop
(
    czl_gp *gp,
    czl_fun *cb_fun,
    czl_var *srv,
    czl_tabkv *c,
    czl_table *tab
)
{
    do {
        char ret;
        unsigned char type;
        czl_value val;
        SOCKET sock = c->key.inum;
        void **obj = czl_http_pop(gp, (czl_var*)c, 1);
        if (!obj)
            return (CZL_EXCEPTION_OUT_OF_MEMORY == gp->exceptionCode ? 0 : 1);
        type = c->type;
        val = c->val;
        c->val.Obj = obj;
        c->type = CZL_TABLE;
        ret = czl_tcp_event_handle(gp, cb_fun, srv, sock);
        if ((c=czl_find_num_tabkv(tab, sock)))
        {
            czl_val_del(gp, (czl_var*)c);
            c->type = type;
            c->val = val;
        }
        if (!ret)
            return 0;
    } while (CZL_STRING == c->type);

    return 1;
}
#endif //#ifdef CZL_LIB_HTTP

#ifdef CZL_LIB_WS
char czl_ws_event_loop
(
    czl_gp *gp,
    czl_fun *cb_fun,
    czl_var *srv,
    czl_tabkv *c,
    czl_table *tab
)
{
    do {
        char ret;
        unsigned char type;
        czl_value val;
        SOCKET sock = c->key.inum;
        void **obj = czl_ws_pop(gp, (czl_var*)c, 1);
        if (!obj)
            return (CZL_EXCEPTION_OUT_OF_MEMORY == gp->exceptionCode ? 0 : 1);
        type = c->type;
        val = c->val;
        c->val.str.size = CZL_STR(obj)->len+1;
        c->val.str.Obj = obj;
        c->type = CZL_STRING;
        ret = czl_tcp_event_handle(gp, cb_fun, srv, sock);
        if ((c=czl_find_num_tabkv(tab, sock)))
        {
            czl_val_del(gp, (czl_var*)c);
            c->type = type;
            c->val = val;
        }
        if (!ret)
            return 0;
    } while (CZL_STRING == c->type);

    return 1;
}
#endif //#ifdef CZL_LIB_WS

char czl_tcp_recv_data(czl_gp *gp, czl_tabkv *q, unsigned long limit)
{
    long len;
    char buf[CZL_TCP_BUF_SIZE];
    char flag = 0;

    do {
        len = recv(q->key.inum, buf, CZL_TCP_BUF_SIZE, 0);
        if (len <= 0)
        {
            if (flag)
                break;
        #ifdef CZL_SYSTEM_WINDOWS
            if (len < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
        #else //CZL_SYSTEM_LINUX
            extern int errno;
            if (len < 0 && EWOULDBLOCK == errno)
        #endif
                return 1;
            if (!czl_val_del(gp, (czl_var*)q))
                return 0;
            q->type = CZL_INT;
            q->val.inum = -1;
            break;
        }
        if (CZL_STRING == q->type)
        {
            czl_string *s = CZL_STR(q->val.str.Obj);
            if (limit && s->len+len > limit)
            {
                CZL_SF(gp, q->val.str);
                q->type = CZL_INT;
                return 2;
            }
            if (s->rc > 1)
            {
                czl_str str;
                if (!czl_str_create(gp, &str, s->len+len+1, s->len, s->str))
                    return 0;
                --s->rc;
                q->val.str = str;
                s = CZL_STR(str.Obj);
            }
            else if (s->len+len >= q->val.str.size)
            {
                unsigned long size = s->len*2+len+1;
                void **obj = CZL_SR(gp, q->val.str, size);
                if (!obj)
                    return 0;
                q->val.str.Obj = obj;
                q->val.str.size = size;
                s = CZL_STR(obj);
            }
            memcpy(s->str+s->len, buf, len);
            s->str[s->len+len] = '\0';
            s->len += len;
        }
        else
        {
            if (limit && (unsigned long)len > limit)
                return 2;
            if (q->type != CZL_INT && !czl_val_del(gp, (czl_var*)q))
                return 0;
            if (!czl_str_create(gp, &q->val.str, len+1, len, buf))
            {
                q->type = CZL_INT;
                return 0;
            }
            q->type = CZL_STRING;
        }
        flag = 1;
    } while (CZL_TCP_BUF_SIZE == len);

    return 2;
}

char czl_tcp_get_data(czl_gp *gp, czl_var *var, czl_tabkv *q)
{
    czl_tcp_handler *h = CZL_SRC(var->val.Obj)->src;
    char ret = czl_tcp_recv_data(gp, q, h->limit);

    if (0 == ret)
        return 0;
    else if (1 == ret)
    {
        if (h->block && !czl_tcp_event_handle(gp, h->block, var, q->val.inum))
            return 0;
        return 1;
    }

    if (CZL_INT == q->type)
    {
        czl_tcp_sock_delete(gp, h, q->key.inum);
        return (h->leave ? czl_tcp_event_handle(gp, h->leave, var, q->key.inum) : 1);
    }
    else if (h->tcp)
        return czl_tcp_event_handle(gp, h->tcp, var, q->key.inum);
#ifdef CZL_LIB_HTTP
    else if (h->http && CZL_NIL == q->ot)
        return czl_http_event_loop(gp, h->http, var, q, CZL_TAB(h->obj));
#endif
#ifdef CZL_LIB_WS
    else if (h->ws)
        return czl_ws_event_loop(gp, h->ws, var, q, CZL_TAB(h->obj));
#endif
    else
        return 1;
}

#ifdef CZL_SYSTEM_WINDOWS
char czl_tcp_listen_1(czl_gp *gp, czl_fun *fun, czl_var *var)
{
    czl_tcp_handler *h = CZL_SRC(var->val.Obj)->src;
    long time = fun->vars[1].val.inum;
    czl_table *tab = CZL_TAB(h->obj);
    czl_tabkv *p;
    unsigned long i;
    unsigned long cnt = 0;
    unsigned long tmp = tab->count / FD_SETSIZE;
    unsigned long maxCnt = time / (tmp ? tmp : 1);
    struct timeval timeout = {0, (0 == time ? 0 : 1)};

    do {
        if (tab->count < (unsigned long)h->maxFd)
        {
            czl_tcp_lock();
            if (czl_tcp_busy)
            {
                czl_tcp_unlock();
                if (time && 0 == tab->count)
                    CZL_SLEEP(1); //避免CPU空转
            }
            else
            {
                SOCKET sock;
                fd_set fdRead;
                czl_tcp_busy = 1;
                czl_tcp_unlock();
                FD_ZERO(&fdRead);
                FD_SET(h->sock, &fdRead);
                if (select(0, &fdRead, NULL, NULL, &timeout) > 0 &&
                    (sock=accept(h->sock, NULL, NULL)) > 0)
                {
                    czl_tabkv *ele = czl_create_key_num_tabkv(gp, tab, sock, NULL, 0);
                    if (!ele)
                    {
                        closesocket(sock);
                        czl_tcp_busy = 0;
                        return 0;
                    }
                    ele->val.inum = 0;
                    if (h->login && !czl_tcp_event_handle(gp, h->login, var, sock))
                    {
                        czl_tcp_busy = 0;
                        return 0;
                    }
                }
                czl_tcp_busy = 0;
            }
        }
        //
        p = (h->last ? h->last : tab->eles_head);
        for (i = 0; i < tab->count && p; i += FD_SETSIZE)
        {
            fd_set fdRead;
            unsigned long j;
            unsigned char flag = 0;
            czl_tabkv *q = p;
            FD_ZERO(&fdRead);
            for (j = 0; j < FD_SETSIZE && p; ++j, p = p->next)
                FD_SET(p->key.inum, &fdRead);
            if (select(0, &fdRead, NULL, NULL, &timeout) <= 0)
                continue;
            for (j = 0; j < FD_SETSIZE && q; ++j, q = h->last)
            {
                h->last = q->next; //czl_tcp_get_data函数里可能会修改q->next指针，这里先备份
                if (!FD_ISSET(q->key.inum, &fdRead))
                    continue;
                if (!czl_tcp_get_data(gp, var, q))
                    return 0;
                flag = 1;
            }
            if (flag)
                return 1;
        }
        h->last = NULL;
    } while (time < 0 || ++cnt < maxCnt);

    return 1;
}
#else //CZL_SYSTEM_LINUX
char czl_tcp_listen_1(czl_gp *gp, czl_fun *fun, czl_var *var)
{
    czl_tcp_handler *h = CZL_SRC(var->val.Obj)->src;
    long time = fun->vars[1].val.inum;
    czl_table *tab = CZL_TAB(h->obj);
    int i, cnt;
    unsigned char busyFlag = 0;
    char retFlag = 1;

    if (CZL_TCP_SRV_WORKER == h->type && tab->count < (unsigned long)h->maxFd)
    {
        czl_tcp_lock();
        if (czl_tcp_busy)
        {
            czl_tcp_unlock();
            if (time && 0 == tab->count)
                CZL_SLEEP(1); //避免CPU空转
        }
        else
        {
            czl_tcp_busy = 1;
            czl_tcp_unlock();
            busyFlag = 1;
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = h->sock;
            epoll_ctl(h->kdpfd, EPOLL_CTL_ADD, h->sock, &ev);
        }
    }

    if ((cnt=epoll_wait(h->kdpfd, h->events, h->count, time)) <= 0)
    {
        retFlag = 1;
        goto CZL_END;
    }

    for (i = 0; i < cnt; ++i)
    {
        int sock = h->events[i].data.fd;
        if (h->sock == sock)
        {
            czl_tabkv *ele;
            struct epoll_event ev;
            if ((sock=accept(h->sock, NULL, NULL)) <= 0)
                continue;
            if (!(ele=czl_create_key_num_tabkv(gp, tab, sock, NULL, 0)))
            {
                close(sock);
                retFlag = 0;
                goto CZL_END;
            }
            ele->val.inum = 0;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = sock;
            if (epoll_ctl(h->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0)
            {
                close(sock);
                czl_tcp_sock_delete(gp, h, sock);
            }
            if (h->login && !czl_tcp_event_handle(gp, h->login, var, sock))
            {
                retFlag = 0;
                goto CZL_END;
            }
            if (tab->count >= h->maxFd)
                epoll_ctl(h->kdpfd, EPOLL_CTL_DEL, h->sock, NULL);
            if (tab->count >= h->count)
            {
                void *tmp = CZL_TMP_REALLOC(gp, h->events,
                                            2*h->count*sizeof(struct epoll_event),
                                            h->count*sizeof(struct epoll_event));
                if (!tmp)
                {
                    retFlag = 0;
                    goto CZL_END;
                }
                h->events = tmp;
                h->count *= 2;
            }
        }
        else
        {
            czl_tabkv *q = czl_find_num_tabkv(tab, sock);
            if (!q)
                continue;
            if (!czl_tcp_get_data(gp, var, q))
            {
                retFlag = 0;
                goto CZL_END;
            }
        }
    }

CZL_END:
    if (busyFlag)
    {
        czl_tcp_busy = 0;
        epoll_ctl(h->kdpfd, EPOLL_CTL_DEL, h->sock, NULL);
    }
    return retFlag;
}
#endif

char czl_tcp_listen(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);

    if (!extsrc)
        return 1;

    h = extsrc->src;
    if (CZL_TCP_SRV == h->type || CZL_TCP_CLIENT == h->type)
        return czl_tcp_listen_0(gp, fun, h->sock);
    else
        return czl_tcp_listen_1(gp, fun, fun->vars);
}

char czl_tcp_close(czl_gp *gp, czl_fun *fun)
{
    czl_tabkv *p;
    czl_tcp_handler *h;
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);
    SOCKET sock = fun->vars[1].val.inum;

    fun->ret.type = CZL_INT;
    if (!extsrc)
        return 1;

    h = extsrc->src;
    if (h->type != CZL_TCP_SRV_MASTER && h->type != CZL_TCP_SRV_WORKER)
        return 1;

    if (!(p=czl_tcp_sock_delete(gp, h, sock)))
        return 1;

#ifdef CZL_SYSTEM_WINDOWS
    if (h->last && h->last == p)
        h->last = h->last->next;
#else //CZL_SYSTEM_LINUX
    if (CZL_TCP_SRV_MASTER == h->type && CZL_TAB(h->obj)->count == h->maxFd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = h->sock;
        epoll_ctl(h->kdpfd, EPOLL_CTL_ADD, h->sock, &ev);
    }
#endif

    fun->ret.val.inum = 1;
    return 1;
}

unsigned long czl_tcp_data(czl_gp *gp, SOCKET sock, long time, char *buf)
{
    long len;

#ifdef CZL_SYSTEM_WINDOWS
    fd_set fdRead;
    struct timeval timeout, *pTime = NULL;
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(0, &fdRead, NULL, NULL, pTime) <= 0 || !FD_ISSET(sock, &fdRead))
        return 0;
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        return 0;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
#endif

    if ((len=recv(sock, buf, CZL_TCP_BUF_SIZE, 0)) <= 0)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        if (len < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;
    #else //CZL_SYSTEM_LINUX
        extern int errno;
        if (len < 0 && EWOULDBLOCK == errno)
            return 0;
    #endif
        return 0;
    }

    return len;
}

char czl_tcp_recv(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);
    long time = fun->vars[1].val.inum;
    char buf[CZL_TCP_BUF_SIZE];
    unsigned long len;

    if (!extsrc)
        return 1;

    h = extsrc->src;
    if ((h->type != CZL_TCP_SRV && h->type != CZL_TCP_CLIENT) ||
        !(len=czl_tcp_data(gp, h->sock, time, buf)))
        return 1;

    if (!czl_str_create(gp, &fun->ret.val.str, len+1, len, buf))
        return 0;

    fun->ret.type = CZL_STRING;
    return 1;
}

char czl_tcp_send(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);

    fun->ret.type = CZL_INT;
    if (!extsrc)
        return 1;

    fun->ret.val.inum = czl_net_send(gp,
                                     ((czl_tcp_handler*)extsrc->src)->sock,
                                     buf->str, buf->len);
    return 1;
}

char czl_tcp_event(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);
    char *event = CZL_STR(fun->vars[1].val.str.Obj)->str;
    czl_fun *cb_fun = fun->vars[2].val.fun;

    fun->ret.type = CZL_INT;
    if (!extsrc)
        return 1;

    h = extsrc->src;
    if (h->type != CZL_TCP_SRV_MASTER && h->type != CZL_TCP_SRV_WORKER)
        return 1;

    if (czl_strcmp("login", event))
        h->login = cb_fun;
    else if (czl_strcmp("leave", event))
        h->leave = cb_fun;
    else if (czl_strcmp("block", event))
        h->block = cb_fun;
    else if (czl_strcmp("tcp", event))
        h->tcp = cb_fun;
#ifdef CZL_LIB_HTTP
    else if (czl_strcmp("http", event))
        h->http = cb_fun;
#endif
#ifdef CZL_LIB_WS
    else if (czl_strcmp("ws", event))
        h->ws = cb_fun;
#endif
    else
        return 1;

    fun->ret.val.inum = 1;
    return 1;
}

char czl_tcp_limit(czl_gp *gp, czl_fun *fun)
{
    czl_tcp_handler *h;
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);

    if (!extsrc)
        return 1;

    h = extsrc->src;
    if (h->type != CZL_TCP_SRV_MASTER && h->type != CZL_TCP_SRV_WORKER)
        return 1;

    h->limit = fun->vars[1].val.inum;
    return 1;
}

char czl_tcp_ip(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock;
    struct sockaddr_in in;
    int size = sizeof(in);

    if (CZL_INT == fun->vars->type)
        sock = fun->vars->val.inum;
    else if (CZL_SOURCE == fun->vars->type)
    {
        czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_tcp);
        if (!extsrc)
            return 1;
        sock = ((czl_tcp_handler*)extsrc->src)->sock;
    }
    else
        return 1;

    if (getpeername(sock, (struct sockaddr*)&in, &size) != SOCKET_ERROR)
    {
        char *ip = inet_ntoa(in.sin_addr);
        if (!czl_set_ret_str(gp, &fun->ret, ip, strlen(ip)))
            return 0;
    }

    return 1;
}
#endif //CZL_LIB_TCP

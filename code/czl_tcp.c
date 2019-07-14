#include "czl_tcp.h"

#ifdef CZL_LIB_TCP

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_tcp_server(czl_gp *gp, czl_fun *fun);  //创建服务器socket
char czl_tcp_listen(czl_gp *gp, czl_fun *fun);  //服务器侦听新客户端并获取socket
char czl_tcp_connect(czl_gp *gp, czl_fun *fun); //连接服务器并获得socket
char czl_tcp_close(czl_gp *gp, czl_fun *fun);   //关闭socket
char czl_tcp_recv(czl_gp *gp, czl_fun *fun);    //从socket接收数据
char czl_tcp_send(czl_gp *gp, czl_fun *fun);    //向socket发送数据
char czl_tcp_ip(czl_gp *gp, czl_fun *fun);      //获取socket的ip地址

//库函数表定义
const czl_sys_fun czl_lib_tcp[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"server",   czl_tcp_server,   3,        "str_v1,int_v2,int_v3=0"},
    {"listen",   czl_tcp_listen,   2,        "int_v1,int_v2=-1"},
    {"connect",  czl_tcp_connect,  2,        "str_v1,int_v2"},
    {"close",    czl_tcp_close,    2,        "&v1,int_v2=0"},
    {"recv",     czl_tcp_recv,     2,        "&v1,int_v2=-1"},
    {"send",     czl_tcp_send,     3,        "int_v1,str_v2,int_v3=-1"},
    {"ip",       czl_tcp_ip,       1,        "int_v1"},
};

#define CZL_TCP_BUF_SIZE 50*1024

//库函数定义
char czl_tcp_server(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;
    unsigned long type = 1; //非阻塞模式
    long maxFd = fun->vars[2].val.inum;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(CZL_STR(fun->vars[0].val.str.Obj)->str);
    service.sin_port = htons(fun->vars[1].val.inum);

    fun->ret.val.inum = 0;

    if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0 ||
    #ifdef CZL_SYSTEM_WINDOWS
        SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &type) ||
    #else //CZL_SYSTEM_LINUX
        SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &type, sizeof(int)) ||
    #endif
        SOCKET_ERROR == bind(sock, (struct sockaddr*)&service, sizeof(service)) ||
        SOCKET_ERROR == listen(sock, 5))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    if (maxFd)
    {
        void **obj;
        czl_tabkv *ele;
        if (!(obj=czl_table_create(gp, 1, 0, 0)) ||
            !(ele=czl_create_num_tabkv(gp, CZL_TAB(obj), sock)))
        {
        #ifdef CZL_SYSTEM_WINDOWS
            closesocket(sock);
        #else //CZL_SYSTEM_LINUX
            close(sock);
        #endif
            CZL_TAB_FREE(gp, obj);
            return 0;
        }
        if (maxFd < 0)
            maxFd = 1000000;
    #ifdef CZL_SYSTEM_WINDOWS
        ele->val.ext.v1.ptr = NULL;
    #else //CZL_SYSTEM_LINUX
        int kdpfd = -1;
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = sock;
        if ((kdpfd=epoll_create(maxFd)) < 0 ||
            epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0)
        {
            close(sock);
            close(kdpfd);
            czl_table_delete(gp, obj);
            return 1;
        }
        ele->val.ext.v1.i32 = kdpfd;
    #endif
        ele->val.ext.v2.u32 = maxFd;
        fun->ret.type = CZL_TABLE;
        fun->ret.val.Obj = obj;
    }
    else
        fun->ret.val.inum = sock;

    return 1;
}

char czl_tcp_listen(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    long time = fun->vars[1].val.inum;
#ifdef CZL_SYSTEM_WINDOWS
    fd_set fdRead;
    struct timeval timeout, *pTime = NULL;
#endif

#ifdef CZL_SYSTEM_WINDOWS
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(sock+1, &fdRead, NULL, NULL, pTime) <= 0)
        fun->ret.val.inum = 0;
    else if (FD_ISSET(sock, &fdRead))
        fun->ret.val.inum = accept(sock, NULL, NULL);
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        fun->ret.val.inum = 0;
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        return 1;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
    fun->ret.val.inum = accept(sock, NULL, NULL);
#endif

    return 1;
}

char czl_tcp_connect(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(CZL_STR(fun->vars[0].val.str.Obj)->str);
    service.sin_port = htons(fun->vars[1].val.inum);

    if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0 ||
        SOCKET_ERROR == connect(sock, (struct sockaddr*)&service, sizeof(service)))
    {
        fun->ret.val.inum = 0;
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    fun->ret.val.inum = sock;
    return 1;
}

char czl_tcp_close(czl_gp *gp, czl_fun *fun)
{
    czl_var *h = CZL_GRV(fun->vars);

    if (CZL_INT == h->type)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        fun->ret.val.inum = (SOCKET_ERROR == closesocket(h->val.inum) ? 0 : 1);
    #else //CZL_SYSTEM_LINUX
        fun->ret.val.inum = (SOCKET_ERROR == close(h->val.inum) ? 0 : 1);
    #endif
    }
    else if (CZL_TABLE == h->type)
    {
        czl_table *tab = CZL_TAB(h->val.Obj);
        if (fun->vars[1].val.inum > 0)
        {
            int sock = fun->vars[1].val.inum;
            if (tab->count <= 1)
            {
                fun->ret.val.inum = 0;
                return 1;
            }
        #ifdef CZL_SYSTEM_WINDOWS
            if (tab->eles_tail->val.ext.v1.ptr &&
                czl_find_num_tabkv(tab, sock) == tab->eles_tail->val.ext.v1.ptr)
            {
                czl_tabkv *ele = tab->eles_tail->val.ext.v1.ptr;
                tab->eles_tail->val.ext.v1.ptr = ele->next;
            }
        #else //CZL_SYSTEM_LINUX
            if (tab->count == tab->eles_tail->val.ext.v2.u32 + 1)
            {
                int lisfd = tab->eles_tail->key.inum;
                int kdpfd = tab->eles_tail->val.ext.v1.i32;
                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = lisfd;
                if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, lisfd, &ev) < 0)
                {
                    fun->ret.val.inum = 0;
                    return 1;
                }
            }
        #endif
            if (czl_delete_num_tabkv(gp, h, sock))
            {
            #ifdef CZL_SYSTEM_WINDOWS
                fun->ret.val.inum = (SOCKET_ERROR == closesocket(sock) ? 0 : 1);
            #else //CZL_SYSTEM_LINUX
                epoll_ctl(tab->eles_tail->val.ext.v1.i32, EPOLL_CTL_DEL, sock, NULL);
                fun->ret.val.inum = (SOCKET_ERROR == close(sock) ? 0 : 1);
            #endif
            }
            else
                fun->ret.val.inum = 0;
        }
        else if (tab->count > 0)
        {
            char flag = 1;
            czl_tabkv *p = CZL_TAB(h->val.Obj)->eles_head;
        #ifdef CZL_SYSTEM_LINUX
            int kdpfd = tab->eles_tail->val.ext.v1.i32;
        #endif
            while (p)
            {
            #ifdef CZL_SYSTEM_WINDOWS
                if (SOCKET_ERROR == closesocket(p->key.inum))
                    flag = 0;
            #else //CZL_SYSTEM_LINUX
                epoll_ctl(kdpfd, EPOLL_CTL_DEL, p->key.inum, NULL);
                if (SOCKET_ERROR == close(p->key.inum))
                    flag = 0;
                if (!p->next && SOCKET_ERROR == close(kdpfd))
                    flag = 0;
            #endif
                p = p->next;
            }
            if (0 == CZL_ORCD1(h->val.Obj))
                czl_table_delete(gp, h->val.Obj);
            h->type = CZL_INT;
            h->val.inum = 0;
            fun->ret.val.inum = flag;
        }
        else
            fun->ret.val.inum = 0;
    }
    else
        fun->ret.val.inum = 0;

    return 1;
}

char czl_tcp_recv_0(czl_gp *gp, czl_fun *fun, SOCKET sock)
{
    long time = fun->vars[1].val.inum;
    char buf[CZL_TCP_BUF_SIZE];
    long len;

#ifdef CZL_SYSTEM_WINDOWS
    fd_set fdRead;
    struct timeval timeout, *pTime = NULL;
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(0, &fdRead, NULL, NULL, pTime) <= 0 || !FD_ISSET(sock, &fdRead))
    {
        fun->ret.val.inum = 0;
        return 1;
    }
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        fun->ret.val.inum = 0;
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        return 1;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
#endif

    if ((len=recv(sock, buf, CZL_TCP_BUF_SIZE, 0)) <= 0)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        if (len < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        extern int errno;
        if (len < 0 && EWOULDBLOCK == errno)
        {
            fun->ret.val.inum = 0;
            return 1;
        }
        close(sock);
    #endif
        fun->ret.val.inum = -1;
        return 1;
    }

    if (!czl_str_create(gp, &fun->ret.val.str, len+1, len, buf))
        return 0;

    fun->ret.type = CZL_STRING;
    return 1;
}

char czl_tcp_get_data(czl_gp *gp, czl_table *tab, czl_tabkv *q)
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
                return -1;
        #else //CZL_SYSTEM_LINUX
            extern int errno;
            if (len < 0 && EWOULDBLOCK == errno)
                return -1;
        #endif
            if (!czl_val_del(gp, (czl_var*)q))
                return 0;
            q->type = CZL_INT;
            q->val.inum = -1;
            break;
        }
        else if (CZL_STRING == q->type)
        {
            czl_string *s = CZL_STR(q->val.str.Obj);
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

    if (q->last)
    {
        q->last->next = q->next;
        if (q->next)
            q->next->last = q->last;
        else
            tab->eles_tail = q->last;
        q->last = NULL;
        q->next = tab->eles_head;
        tab->eles_head = tab->eles_head->last = q;
    }

    return 1;
}

#ifdef CZL_SYSTEM_WINDOWS
char czl_tcp_recv_1(czl_gp *gp, czl_fun *fun, czl_var *h)
{
    long time = fun->vars[1].val.inum;
    czl_table *tab = CZL_TAB(h->val.Obj);
    czl_tabkv *p;
    unsigned long resCnt = 0;
    unsigned long i;
    unsigned long cnt = 0;
    unsigned long tmp = tab->count / FD_SETSIZE;
    unsigned long maxCnt = time / (tmp ? tmp : 1);
    struct timeval timeout = {0, (0 == time ? 0 : 1)};

    if (0 == tab->count || tab->eles_tail->type != CZL_INT)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (tab->rc > 1 && !(tab=czl_table_fork(gp, h)))
        return 0;

    do {
        if (tab->eles_tail->val.ext.v1.ptr)
            p = (czl_tabkv*)tab->eles_tail->val.ext.v1.ptr;
        else
            p = tab->eles_head;
        for (i = 0; i < tab->count && p; i += FD_SETSIZE)
        {
            czl_tabkv *next, *q = p;
            fd_set fdRead;
            unsigned long j;
            FD_ZERO(&fdRead);
            for (j = 0; j < FD_SETSIZE && p; ++j, p = p->next)
            {
                if (!p->next && tab->count > p->val.ext.v2.u32)
                    break;
                FD_SET(p->key.inum, &fdRead);
            }
            if (select(0, &fdRead, NULL, NULL, &timeout) <= 0)
                continue;
            for (j = 0; j < FD_SETSIZE && q; ++j, q = next)
            {
                SOCKET sock;
                next = q->next; //czl_tcp_get_data函数里可能会修改q->next指针，这里先备份
                if (!FD_ISSET(q->key.inum, &fdRead))
                    continue;
                if (q->next)
                {
                    char ret = czl_tcp_get_data(gp, tab, q);
                    if (1 == ret)
                        ++resCnt;
                    else if (0 == ret)
                        return 0;
                }
                else if ((sock=accept(q->key.inum, NULL, NULL)) > 0)
                {
                    czl_tabkv *ele = czl_create_key_num_tabkv(gp, tab, sock, NULL, 0);
                    if (!ele || !czl_val_del(gp, (czl_var*)ele))
                    {
                        closesocket(sock);
                        return 0;
                    }
                    ele->val.inum = 0;
                    ++resCnt;
                }
            }
            if (resCnt)
            {
                tab->eles_tail->val.ext.v1.ptr = q;
                fun->ret.val.inum = resCnt;
                return 1;
            }
        }
        tab->eles_tail->val.ext.v1.ptr = NULL;
    } while (time < 0 || ++cnt < maxCnt);

    fun->ret.val.inum = 0;
    return 1;
}
#else //CZL_SYSTEM_LINUX
char czl_tcp_recv_1(czl_gp *gp, czl_fun *fun, czl_var *h)
{
    long time = fun->vars[1].val.inum;
    czl_table *tab = CZL_TAB(h->val.Obj);
    struct epoll_event *events;
    int i, cnt, resCnt = 0;
    int kdpfd, lisfd;
    unsigned long maxFd;
    unsigned long size = tab->count*sizeof(struct epoll_event);

    if (0 == size)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if (tab->rc > 1 && !(tab=czl_table_fork(gp, h)))
        return 0;

    if (!(events=(struct epoll_event*)CZL_TMP_MALLOC(gp, size)))
        return 0;

    lisfd = tab->eles_tail->key.inum;
    kdpfd = tab->eles_tail->val.ext.v1.i32;
    maxFd = tab->eles_tail->val.ext.v2.u32;

    if ((cnt=epoll_wait(kdpfd, events, tab->count, time)) <= 0)
    {
        fun->ret.val.inum = 0;
        CZL_TMP_FREE(gp, events, size);
        return 1;
    }

    for (i = 0; i < cnt; ++i)
    {
        int sock = events[i].data.fd;
        if (lisfd == sock)
        {
            czl_tabkv *ele;
            struct epoll_event ev;
            if ((sock=accept(lisfd, NULL, NULL)) <= 0)
                continue;
            if (!(ele=czl_create_key_num_tabkv(gp, tab, sock, NULL, 0)) ||
                !czl_val_del(gp, (czl_var*)ele))
            {
                close(sock);
                CZL_TMP_FREE(gp, events, size);
                return 0;
            }
            ele->val.inum = 0;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = sock;
            if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0)
            {
                close(sock);
                czl_delete_num_tabkv(gp, h, sock);
            }
            else
                ++resCnt;
            //当连接数达到最大时从红黑树删除accept-sock避免“惊群现象”提高服务器性能
            if (tab->count == maxFd + 1)
                epoll_ctl(kdpfd, EPOLL_CTL_DEL, lisfd, NULL);
        }
        else
        {
            char ret;
            czl_tabkv *q = czl_find_num_tabkv(tab, sock);
            if (!q)
                continue;
            ret = czl_tcp_get_data(gp, tab, q);
            if (1 == ret)
                ++resCnt;
            else if (0 == ret)
            {
                CZL_TMP_FREE(gp, events, size);
                return 0;
            }
        }
    }

    fun->ret.val.inum = resCnt;
    CZL_TMP_FREE(gp, events, size);
    return 1;
}
#endif

char czl_tcp_recv(czl_gp *gp, czl_fun *fun)
{
    czl_var *h = CZL_GRV(fun->vars);

    switch (h->type)
    {
    case CZL_INT:
        //读取一个客户端sock数据，函数返回字符串
        return czl_tcp_recv_0(gp, fun, h->val.inum);
    case CZL_TABLE:
        //侦听所有sock，其中最后一个哈希表节点存服务器sock和最大连接数信息，
        //有数据或对端关闭的节点连接到哈希表头，函数返回有状态更新的节点个数
        return czl_tcp_recv_1(gp, fun, h);
    default:
        fun->ret.val.inum = 0;
        return 1;
    }
}

char czl_tcp_send(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    long time = fun->vars[2].val.inum;
#ifdef CZL_SYSTEM_WINDOWS
    fd_set fdWrite;
    struct timeval timeout, *pTime = NULL;
#endif

    if ((fun->ret.val.inum=send(sock, buf->str, buf->len, 0)) >= 0)
        return 1;

#ifdef CZL_SYSTEM_WINDOWS
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdWrite); FD_SET(sock, &fdWrite);
    if (select(0, NULL, &fdWrite, NULL, pTime) <= 0)
    {
        fun->ret.val.inum = 0;
        return 1;
    }
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLOUT;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        fun->ret.val.inum = 0;
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        return 1;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
#endif

    if (SOCKET_ERROR == (fun->ret.val.inum=send(sock, buf->str, buf->len, 0)))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            fun->ret.val.inum = 0;
        else
            closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        extern int errno;
        if (EWOULDBLOCK == errno || EAGAIN == errno)
            fun->ret.val.inum = 0;
        else
            close(sock);
    #endif
    }

    return 1;
}

char czl_tcp_ip(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in in;
    int size = sizeof(in);

    if (SOCKET_ERROR == getpeername(fun->vars[0].val.inum, (struct sockaddr*)&in, &size))
        fun->ret.val.inum = 0;
    else
    {
        char *ip = inet_ntoa(in.sin_addr);
        if (!czl_set_ret_str(gp, &fun->ret, ip, strlen(ip)))
            return 0;
    }

    return 1;
}
#endif //CZL_LIB_TCP

#include "czl_udp.h"

#ifdef CZL_LIB_UDP

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_udp_server(czl_gp *gp, czl_fun *fun);  //创建服务器socket
char czl_udp_connect(czl_gp *gp, czl_fun *fun); //连接服务器并获得socket
char czl_udp_recv(czl_gp *gp, czl_fun *fun);    //从socket接收数据
char czl_udp_send(czl_gp *gp, czl_fun *fun);    //向socket发送数据
char czl_udp_ip(czl_gp *gp, czl_fun *fun);      //获取socket的ip地址

//库函数表定义
const czl_sys_fun czl_lib_udp[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"server",   czl_udp_server,   2,        "str_v1,int_v2"},
    {"connect",  czl_udp_connect,  2,        "str_v1,int_v2"},
    {"recv",     czl_udp_recv,     2,        "src_v1,int_v2=-1"},
    {"send",     czl_udp_send,     4,        "src_v1,str_v2,int_v3=1024,int_v4=0"},
    {"ip",       czl_udp_ip,       1,        "src_v1"},
};

#define CZL_UDP_PACKET_MAX 65507 //udp单次数据发送不得超过65507
//UDP与TCP不同，UDP不会对数据包进行分割、合并、排序、重传，所以sendto与recvfrom总是一一对应的

typedef struct czl_udp_handle
{
    czl_gp *gp;
    SOCKET sock;
    unsigned long ip;
    unsigned long port;
} czl_udp_handle;

void czl_udp_close(czl_udp_handle *h)
{
#ifdef CZL_SYSTEM_WINDOWS
    closesocket(h->sock);
#else //CZL_SYSTEM_LINUX
    close(h->sock);
#endif
    CZL_TMP_FREE(h->gp, h, sizeof(czl_udp_handle));
}

//库函数定义
char czl_udp_server(czl_gp *gp, czl_fun *fun)
{
    czl_udp_handle *h;
    struct sockaddr_in service;
    SOCKET sock;
    unsigned long type = 1; //非阻塞模式
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

    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0 ||
    #ifdef CZL_SYSTEM_WINDOWS
        SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &type) ||
    #else //CZL_SYSTEM_LINUX
        SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &type, sizeof(int)) ||
    #endif
        SOCKET_ERROR == bind(sock, (struct sockaddr*)&service, sizeof(service)) ||
        !(h=(czl_udp_handle*)CZL_TMP_MALLOC(gp, sizeof(czl_udp_handle))))
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
    h->ip = service.sin_addr.s_addr;
    h->port = service.sin_port;

    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_udp_close,
                                             CZL_LIB_UDP_NAME, czl_lib_udp)))
    {
        czl_udp_close(h);
        return 0;
    }

    fun->ret.type = CZL_SOURCE;
    return 1;
}

char czl_udp_connect(czl_gp *gp, czl_fun *fun)
{
    czl_udp_handle *h;
    SOCKET sock;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
#endif //#ifdef CZL_SYSTEM_WINDOWS

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
        return 1;

    if (!(h=(czl_udp_handle*)CZL_TMP_MALLOC(gp, sizeof(czl_udp_handle))))
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
    h->ip = inet_addr(CZL_STR(fun->vars[0].val.str.Obj)->str);
    h->port = htons(fun->vars[1].val.inum);

    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_udp_close,
                                             CZL_LIB_UDP_NAME, czl_lib_udp)))
    {
        czl_udp_close(h);
        return 0;
    }
    else
        fun->ret.type = CZL_SOURCE;

    return 1;
}

char czl_udp_recv(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_udp);
    czl_udp_handle *h;
    long time = fun->vars[1].val.inum;
    SOCKET sock;
    struct sockaddr_in in;
    int in_len = sizeof(struct sockaddr);
    char buf[CZL_UDP_PACKET_MAX];
    long len;
    struct timeval timeout, *pTime = NULL;
    fd_set fdRead;

    if (!extsrc)
        return 1;

    h = extsrc->src;
    sock = h->sock;
    in.sin_family = AF_INET;
    in.sin_addr.s_addr = h->ip;
    in.sin_port = h->port;

#ifdef CZL_SYSTEM_WINDOWS
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(0, &fdRead, NULL, NULL, pTime) <= 0 || !FD_ISSET(sock, &fdRead))
        return 1;
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, time) <= 0)
    {
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
        return 1;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
#endif

    if ((len=recvfrom(sock, buf, CZL_UDP_PACKET_MAX, 0, (struct sockaddr*)&in, &in_len)) <= 0)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        if (len < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
            return 1;
    #else //CZL_SYSTEM_LINUX
        extern int errno;
        if (len < 0 && EWOULDBLOCK == errno)
            return 1;
    #endif
        return 1;
    }

    h->ip = in.sin_addr.s_addr;
    h->port = in.sin_port;
    return czl_set_ret_str(gp, &fun->ret, buf, len);
}

char czl_udp_send(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_udp);
    czl_udp_handle *h;
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    long size = fun->vars[2].val.inum;
    unsigned long delay = fun->vars[3].val.inum;
    SOCKET sock;
    struct sockaddr_in in;
    unsigned long cnt = 0;
    long len, ret;

    if (!extsrc)
        return 1;

    size = (size <= 0 || size > CZL_UDP_PACKET_MAX ? CZL_UDP_PACKET_MAX : size);
    len = (buf->len < size ? buf->len : size);

    h = extsrc->src;
    sock = h->sock;
    in.sin_family = AF_INET;
    in.sin_addr.s_addr = h->ip;
    in.sin_port = h->port;

#ifdef CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    ev.events = EPOLLOUT;
    ev.data.fd = sock;
    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0)
        return 1;
#endif

    for (;;)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        fd_set fdWrite;
        FD_ZERO(&fdWrite); FD_SET(sock, &fdWrite);
        if (select(0, NULL, &fdWrite, NULL, NULL) < 0)
            break;
    #else //CZL_SYSTEM_LINUX
        if (epoll_wait(gp->kdpfd, events, 1, -1) < 0)
            break;
    #endif
        ret = sendto(sock, buf->str+cnt, len, 0,
                     (struct sockaddr*)&in, sizeof(struct sockaddr));
        if (SOCKET_ERROR == ret)
        {
            //if (WSAGetLastError() == WSAEWOULDBLOCK)
            //extern int errno;
            //if (EWOULDBLOCK == errno || EAGAIN == errno)
            break;
        }
        else if (ret < len)
        {
            cnt += ret;
            break;
        }
        if ((cnt += len) == buf->len)
            break;
        if (buf->len - cnt < size)
            len = buf->len - cnt;
        if (delay)
            CZL_SLEEP(delay);
    }

    fun->ret.type = CZL_INT;
    fun->ret.val.inum = cnt;

#ifdef CZL_SYSTEM_LINUX
CZL_END:
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, sock, NULL);
#endif

    return 1;
}

char czl_udp_ip(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_udp);
    struct in_addr in;
    char *ip;

    if (!extsrc)
        return 1;

    memcpy(&in, &((czl_udp_handle*)extsrc->src)->ip, 4);
    ip = inet_ntoa(in);
    if (!czl_set_ret_str(gp, &fun->ret, ip, strlen(ip)))
        return 0;

    return 1;
}
#endif //CZL_LIB_UDP

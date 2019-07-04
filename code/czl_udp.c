#include "czl_udp.h"

#ifdef CZL_LIB_UDP

#define CZL_UDP_PACKET_MAX 65507 //udp单次数据发送不得超过65507
//UDP与TCP不同，UDP不会对数据包进行分割、合并、排序、重传，所以sendto与recvfrom总是一一对应的

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_udp_server(czl_gp *gp, czl_fun *fun);  //创建服务器socket
char czl_udp_connect(czl_gp *gp, czl_fun *fun); //连接服务器并获得socket
char czl_udp_close(czl_gp *gp, czl_fun *fun);   //关闭socket
char czl_udp_recv(czl_gp *gp, czl_fun *fun);    //从socket接收数据
char czl_udp_send(czl_gp *gp, czl_fun *fun);    //向socket发送数据
char czl_udp_ip(czl_gp *gp, czl_fun *fun);      //获取socket的ip地址

//库函数表定义
const czl_sys_fun czl_lib_udp[] =
{
    //函数名,     函数指针,          参数个数,  参数声明
    {"server",   czl_udp_server,   2,        "str_v1,int_v2"},
    {"connect",  czl_udp_connect,  2,        "str_v1,int_v2"},
    {"close",    czl_udp_close,    1,        "str_v1"},
    {"recv",     czl_udp_recv,     3,        "&str_v1,int_v2=-1,int_v3=0"},
    {"send",     czl_udp_send,     5,        "str_v1,str_v2,int_v3=1024,int_v4=0,int_v5=0"},
    {"ip",       czl_udp_ip,       1,        "str_v1"},
};

//库函数定义
char czl_udp_server(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;
    unsigned long type = 1; //非阻塞模式
    czl_str str;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(CZL_STR(fun->vars[0].val.str.Obj)->str);
    service.sin_port = htons(fun->vars[1].val.inum);

    fun->ret.val.inum = 0;

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
        return 1;

    if (
    #ifdef CZL_SYSTEM_WINDOWS
        SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &type) ||
    #else //CZL_SYSTEM_LINUX
        SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &type, sizeof(int)) ||
    #endif
        SOCKET_ERROR == bind(sock, (struct sockaddr*)&service, sizeof(service)) ||
        !czl_str_create(gp, &str, 11, 10, NULL))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    memcpy(CZL_STR(str.Obj)->str, &sock, 4);
    memcpy(CZL_STR(str.Obj)->str+4, &service.sin_addr.s_addr, 4);
    memcpy(CZL_STR(str.Obj)->str+8, &service.sin_port, 2);
    fun->ret.type = CZL_STRING;
    fun->ret.val.str = str;
    return 1;
}

char czl_udp_connect(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;
    czl_str str;

#ifdef CZL_SYSTEM_WINDOWS
    WSADATA wsaData;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(CZL_STR(fun->vars[0].val.str.Obj)->str);
    service.sin_port = htons(fun->vars[1].val.inum);

    fun->ret.val.inum = 0;

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;
#endif //#ifdef CZL_SYSTEM_WINDOWS

    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
        return 1;

    if (!czl_str_create(gp, &str, 11, 10, NULL))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    memcpy(CZL_STR(str.Obj)->str, &sock, 4);
    memcpy(CZL_STR(str.Obj)->str+4, &service.sin_addr.s_addr, 4);
    memcpy(CZL_STR(str.Obj)->str+8, &service.sin_port, 2);
    fun->ret.type = CZL_STRING;
    fun->ret.val.str = str;
    return 1;
}

char czl_udp_close(czl_gp *gp, czl_fun *fun)
{
    czl_string *s = CZL_STR(fun->vars[0].val.str.Obj);

    fun->ret.val.inum = 1;

    if (s->len != 10)
        fun->ret.val.inum = 0;

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == closesocket(*((int*)s->str)))
        fun->ret.val.inum = 0;
#else //CZL_SYSTEM_LINUX
    if (SOCKET_ERROR == close(*((int*)s->str)))
        fun->ret.val.inum = 0;
#endif

    return 1;
}

char czl_udp_recv(czl_gp *gp, czl_fun *fun)
{
    czl_var *str = CZL_GCRV(fun->vars);
    long time = fun->vars[1].val.inum;
    czl_string *s;
    SOCKET sock;
    struct sockaddr_in in;
    int in_len = sizeof(struct sockaddr);
    char buf[CZL_UDP_PACKET_MAX];
    long len;
    struct timeval timeout, *pTime = NULL;
    fd_set fdRead;
#ifdef CZL_SYSTEM_LINUX
    char mode = fun->vars[2].val.inum; //0: epoll, 1: select
#endif

    if (!str)
        return 0;

    fun->ret.val.inum = 0;

    s = CZL_STR(str->val.str.Obj);
    if (s->len != 10)
        return 1;

    if (s->rc > 1)
    {
        czl_str tmp;
        if (!czl_str_create(gp, &tmp, s->len+1, s->len, s->str))
            return 0;
        str->val.str = tmp;
        s = CZL_STR(tmp.Obj);
    }

    sock = *((int*)s->str);
    in.sin_family = AF_INET;
    in.sin_addr.s_addr = *((unsigned long*)(s->str+4));
    in.sin_port = *((unsigned short*)(s->str+8));

#ifdef CZL_SYSTEM_WINDOWS
    if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
    FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
    if (select(sock+1, &fdRead, NULL, NULL, pTime) <= 0 || !FD_ISSET(sock, &fdRead))
        return 1;
#else //CZL_SYSTEM_LINUX
    if (mode)
    {
        if (time >= 0) { timeout.tv_sec = 0; timeout.tv_usec = time*1000; pTime = &timeout; }
        FD_ZERO(&fdRead); FD_SET(sock, &fdRead);
        if (select(sock+1, &fdRead, NULL, NULL, pTime) <= 0 || !FD_ISSET(sock, &fdRead))
            return 1;
    }
    else
    {
        int kdpfd = -1;
        struct epoll_event ev, events[1];
        ev.events = EPOLLIN;
        ev.data.fd = sock;
        if ((kdpfd=epoll_create(1)) < 0 ||
            epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
            epoll_wait(kdpfd, events, 1, time) <= 0)
        {
            close(kdpfd);
            return 1;
        }
        close(kdpfd);
    }
#endif

    if ((len=recvfrom(sock, buf, CZL_UDP_PACKET_MAX, 0, (struct sockaddr*)&in, &in_len)) <= 0)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        if (len < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
            return 1;
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        extern int errno;
        if (len < 0 && EWOULDBLOCK == errno)
            return 1;
        close(sock);
    #endif
        fun->ret.val.inum = -1;
        return 1;
    }

    if (!czl_str_create(gp, &fun->ret.val.str, len+1, len, buf))
        return 0;

    fun->ret.type = CZL_STRING;

    memcpy(s->str+4, &in.sin_addr.s_addr, 4);
    memcpy(s->str+8, &in.sin_port, 2);

    return 1;
}

char czl_udp_send(czl_gp *gp, czl_fun *fun)
{
    czl_string *s = CZL_STR(fun->vars[0].val.str.Obj);
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    long size = fun->vars[2].val.inum;
    unsigned long delay = fun->vars[3].val.inum;
    SOCKET sock;
    struct sockaddr_in in;
    unsigned long cnt = 0;
    unsigned long len;
#ifdef CZL_SYSTEM_LINUX
    char mode = fun->vars[4].val.inum; //0: epoll, 1: select
#endif

    size = (size <= 0 || size > CZL_UDP_PACKET_MAX ? CZL_UDP_PACKET_MAX : size);
    len = (buf->len < size ? buf->len : size);

    fun->ret.val.inum = 0;

    if (s->len != 10)
        return 1;

    sock = *((int*)s->str);
    in.sin_family = AF_INET;
    in.sin_addr.s_addr = *((unsigned long*)(s->str+4));
    in.sin_port = *((unsigned short*)(s->str+8));

    do {
    #ifdef CZL_SYSTEM_WINDOWS
        fd_set fdWrite;
        FD_ZERO(&fdWrite); FD_SET(sock, &fdWrite);
        if (select(sock+1, NULL, &fdWrite, NULL, NULL) < 0)
            return 1;
    #else //CZL_SYSTEM_LINUX
        if (mode)
        {
            fd_set fdWrite;
            FD_ZERO(&fdWrite); FD_SET(sock, &fdWrite);
            if (select(sock+1, NULL, &fdWrite, NULL, NULL) < 0)
                return 1;
        }
        else
        {
            int kdpfd = -1;
            struct epoll_event ev, events[1];
            ev.events = EPOLLOUT;
            ev.data.fd = sock;
            if ((kdpfd=epoll_create(1)) < 0 ||
                epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev) < 0 ||
                epoll_wait(kdpfd, events, 1, -1) < 0)
            {
                close(kdpfd);
                return 1;
            }
            close(kdpfd);
        }
    #endif
        if (SOCKET_ERROR == sendto(sock, buf->str+cnt, len, 0,
                                   (struct sockaddr*)&in, sizeof(struct sockaddr)))
        {
        #ifdef CZL_SYSTEM_WINDOWS
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                fun->ret.val.inum = cnt;
                return 1;
            }
            closesocket(sock);
        #else //CZL_SYSTEM_LINUX
            extern int errno;
            if (EWOULDBLOCK == errno || EAGAIN == errno)
            {
                fun->ret.val.inum = cnt;
                return 1;
            }
            close(sock);
        #endif
            fun->ret.val.inum = -1;
            return 1;
        }
        cnt += len;
        if (size == len && buf->len - cnt > 0 && buf->len - cnt < size)
            len = buf->len - cnt;
        if (cnt >= buf->len)
            break;
        else if (delay)
            CZL_SLEEP(delay);
    } while (1);

    fun->ret.val.inum = buf->len;
    return 1;
}

char czl_udp_ip(czl_gp *gp, czl_fun *fun)
{
    czl_string *s = CZL_STR(fun->vars[0].val.str.Obj);
    struct in_addr in;
    char *ip;

    if (s->len != 10)
        fun->ret.val.inum = 0;

    memcpy(&in, s->str+4, 4);
    ip = inet_ntoa(in);
    if (!czl_set_ret_str(gp, &fun->ret, ip, strlen(ip)))
        return 0;

    return 1;
}
#endif //CZL_LIB_UDP

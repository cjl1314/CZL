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
    {"server",   czl_tcp_server,   2,        "str_v1,int_v2"},
    {"listen",   czl_tcp_listen,   2,        "int_v1,int_v2=-1"},
    {"connect",  czl_tcp_connect,  2,        "str_v1,int_v2"},
    {"close",    czl_tcp_close,    1,        "int_v1"},
    {"recv",     czl_tcp_recv,     2,        "int_v1,int_v2=-1"},
    {"send",     czl_tcp_send,     2,        "int_v1,str_v2"},
    {"ip",       czl_tcp_ip,       1,        "int_v1"},
};

//库函数定义
char czl_tcp_server(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;
    unsigned long type = 1; //非阻塞模式

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

    if (INVALID_SOCKET == (sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
        return 1;

    if (
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

    fun->ret.val.inum = sock;
    return 1;
}

char czl_tcp_listen(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    long time = fun->vars[1].val.inum;
    fd_set fdRead;
    int ret;

    FD_ZERO(&fdRead);
    FD_SET(sock, &fdRead);

    if (time < 0)
        ret = select(sock+1, &fdRead, NULL, NULL, NULL);
    else
    {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = time*1000;
        ret = select(sock+1, &fdRead, NULL, NULL, &timeout);
    }

    fun->ret.val.inum = 0;

    if (SOCKET_ERROR == ret)
        return 1;

    if (FD_ISSET(sock, &fdRead))
        fun->ret.val.inum = accept(sock, NULL,NULL);

    return 1;
}

char czl_tcp_connect(czl_gp *gp, czl_fun *fun)
{
    struct sockaddr_in service;
    SOCKET sock;

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

    if (INVALID_SOCKET == (sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
        return 1;

    if (SOCKET_ERROR == connect(sock, (struct sockaddr*)&service, sizeof(service)))
    {
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
    fun->ret.val.inum = 1;

#ifdef CZL_SYSTEM_WINDOWS
    if (SOCKET_ERROR == closesocket(fun->vars[0].val.inum))
        fun->ret.val.inum = 0;
#else //CZL_SYSTEM_LINUX
    if (SOCKET_ERROR == close(fun->vars[0].val.inum))
        fun->ret.val.inum = 0;
#endif

    return 1;
}

char czl_tcp_recv(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    long time = fun->vars[1].val.inum;
    struct timeval timeout;
    fd_set fdRead;
    char buf[65536];
    long len;

    if (time < 0)
    {
        timeout.tv_sec = 2000000000;
        timeout.tv_usec = 0;
    }
    else
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = time*1000;
    }

    FD_ZERO(&fdRead);
    FD_SET(sock, &fdRead);
    if (SOCKET_ERROR == select(sock+1, &fdRead, NULL, NULL, &timeout) ||
        !FD_ISSET(sock, &fdRead))
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    if ((len=recv(sock, buf, 65536, 0)) < 0)
    {
        fun->ret.val.inum = -1;
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
        close(sock);
    #endif
        return 1;
    }

    if (!czl_str_create(gp, &fun->ret.val.str, len+1, len, buf))
        return 0;

    fun->ret.type = CZL_STRING;
    return 1;
}

char czl_tcp_send(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    fd_set fdWrite;

    FD_ZERO(&fdWrite);
    FD_SET(sock, &fdWrite);
    if (SOCKET_ERROR == select(sock+1, NULL, &fdWrite, NULL, NULL))
        fun->ret.val.inum = -1;
    else if (SOCKET_ERROR == (fun->ret.val.inum=send(sock, buf->str, buf->len, 0)))
    {
    #ifdef CZL_SYSTEM_WINDOWS
        closesocket(sock);
    #else //CZL_SYSTEM_LINUX
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

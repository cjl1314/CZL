#include "czl_com.h"

#ifdef CZL_LIB_COM
//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_com_open(czl_gp *gp, czl_fun *fun);    //打开串口
char czl_com_write(czl_gp *gp, czl_fun *fun);   //写串口
char czl_com_read(czl_gp *gp, czl_fun *fun);    //读串口
char czl_com_list(czl_gp *gp, czl_fun *fun);    //打印串口列表

//库函数表定义
const czl_sys_fun czl_lib_com[] =
{
    //函数名,    函数指针,        参数个数,  参数声明
    {"open",    czl_com_open,   3,       "str_v1,int_v2=9600,int_v3=-1"},
    {"write",   czl_com_write,  2,       "src_v1,str_v2"},
    {"read",    czl_com_read,   2,       "src_v1,int_v2=1024"},
    {"list",    czl_com_list,   0,       NULL},
};

#ifdef CZL_SYSTEM_WINDOWS
typedef struct czl_com_handler
{
    czl_gp *gp;
    HANDLE hCom;
    OVERLAPPED wOv;
    OVERLAPPED rOv;
} czl_com_handler;

void czl_com_close(czl_com_handler *h)
{
    CloseHandle(h->hCom);
    CloseHandle(h->wOv.hEvent);
    CloseHandle(h->rOv.hEvent);
    if (!h->gp->end_flag)
        CZL_TMP_FREE(h->gp, h, sizeof(czl_com_handler));
}
#else //CZL_SYSTEM_LINUX
typedef struct czl_com_handler
{
    czl_gp *gp;
    int fd;
    unsigned long timeout;
} czl_com_handler;

void czl_com_close(czl_com_handler *h)
{
    close(h->fd);
    CZL_TMP_FREE(h->gp, h, sizeof(czl_com_handler));
}
#endif

char czl_com_open(czl_gp *gp, czl_fun *fun)
{
    czl_com_handler *h;

#ifdef CZL_SYSTEM_WINDOWS
    DCB sDcb;
    COMMTIMEOUTS timeOuts;
    char *portName = CZL_STR(fun->vars[0].val.str.Obj)->str;
    DWORD baudRate = (unsigned long)fun->vars[1].val.inum;
    long timeOut = fun->vars[2].val.inum;
    DWORD type = (timeOut < 0 ? 0 : FILE_FLAG_OVERLAPPED);
    HANDLE hCom = CreateFile(portName,
                             GENERIC_READ|GENERIC_WRITE,
                             0, NULL, OPEN_EXISTING, type, NULL);

    if (INVALID_HANDLE_VALUE == hCom)
        return 1;

    SetupComm(hCom, 1024, 1024); //配置输入输出缓冲区大小
    PurgeComm(hCom, PURGE_RXCLEAR|PURGE_TXCLEAR); //清除串口缓冲区

    timeOuts.ReadIntervalTimeout = 50;   //读间隔超时，即读取相邻两个字符之间的间隔

    if (type) //异步通信时需要设置超时时间对应OVERLAPPED里的事件
    {
        //设定读超时，单位ms
        timeOuts.ReadTotalTimeoutMultiplier = 0;
        timeOuts.ReadTotalTimeoutConstant = timeOut;
        //读总超时 = ReadTotalTimeoutMultiplier*10 + ReadTotalTimeoutConstant, 即读取一次串口的最大时间

        //设定写超时，单位ms
        timeOuts.WriteTotalTimeoutMultiplier = 0;
        timeOuts.WriteTotalTimeoutConstant = 0;
        //写总超时 = WriteTotalTimeoutMultiplier*10 + WriteTotalTimeoutConstant, 即写一次串口的最大时间
    }

    //设置超时时间
    SetCommTimeouts(hCom, &timeOuts);

    //配置DCB结构参数
    GetCommState(hCom, &sDcb);  //获取DCB结构指针
    sDcb.BaudRate = baudRate;   //设置波特率
    sDcb.ByteSize = 8;          //设置数据位
    sDcb.StopBits = ONESTOPBIT; //设置停止位
    sDcb.Parity = NOPARITY;     //设置校验模式
    SetCommState(hCom, &sDcb);  //设置DCB结构参数

    if (!(h=(czl_com_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_com_handler))))
    {
        CloseHandle(hCom);
        return 1;
    }

    h->gp = gp;
    h->hCom = hCom;

    if (!type)
        h->wOv.hEvent = h->rOv.hEvent = NULL;
    else if (!(h->wOv.hEvent=CreateEvent(NULL, TRUE, TRUE, NULL)) ||
             !(h->rOv.hEvent=CreateEvent(NULL, TRUE, TRUE, NULL)))
    {
        CloseHandle(h->hCom);
        CloseHandle(h->wOv.hEvent);
        CZL_TMP_FREE(gp, h, sizeof(czl_com_handler));
        return 1;
    }
#else //CZL_SYSTEM_LINUX
    char *portName = CZL_STR(fun->vars[0].val.str.Obj)->str;
    unsigned long timeOut = (unsigned long)fun->vars[2].val.inum;
    int baudRate;
    struct termios uart;
    int fd;

    if((fd=open(portName, O_RDWR|O_NDELAY)) == -1 ||
       fcntl(fd, F_SETFL, 0) < 0 ||
       tcgetattr(fd, &uart) != 0)
    {
        close(fd);
        return 1;
    }

    //矫正波特率
    if (baudRate >= 230400) baudRate = B230400;
    else if (baudRate >= 115200) baudRate = B115200;
    else if (baudRate >= 57600) baudRate = B57600;
    else if (baudRate >= 38400) baudRate = B38400;
    else if (baudRate >= 19200) baudRate = B19200;
    else if (baudRate >= 9600) baudRate = B9600;
    else if (baudRate >= 4800) baudRate = B4800;
    else if (baudRate >= 2400) baudRate = B2400;
    else if (baudRate >= 1800) baudRate = B1800;
    else if (baudRate >= 1200) baudRate = B1200;
    else if (baudRate >= 600) baudRate = B600;
    else if (baudRate >= 300) baudRate = B300;
    else baudRate = B200;

    //波特率
    cfsetispeed(&uart, baudRate);
    cfsetospeed(&uart, baudRate);
    uart.c_cflag &= ~CSIZE;
    //数据位: 8
    uart.c_cflag |= CS8;
    //奇偶校验: 无
    uart.c_cflag &= ~CSTOPB;
    uart.c_cflag &= ~PARENB;
    //停止位: 1
    uart.c_cflag &= ~CSTOPB;
    //数据经过处理后输出
    uart.c_oflag &= ~OPOST;
    //使串口工作在原始模式下
    uart.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG );
    //设置等待时间为0
    uart.c_cc[VTIME] = 0;
    //设置最小接收字符为1
    uart.c_cc[VMIN] = 1;
    //清空输入输出缓冲区
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &uart) < 0 ||
        !(h=(czl_com_handler*)CZL_TMP_MALLOC(gp, sizeof(czl_com_handler))))
    {
        close(fd);
        return 1;
    }

    h->gp = gp;
    h->fd = fd;
    h->timeout = timeOut;
#endif

    if (!(fun->ret.val.Obj=czl_extsrc_create(gp, h, czl_com_close,
                                             CZL_LIB_COM_NAME, czl_lib_com)))
    {
        czl_com_close(h);
        return 0;
    }
    else
        fun->ret.type = CZL_SOURCE;

    return 1;
}

char czl_com_write(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_com);

#ifdef CZL_SYSTEM_WINDOWS
    char *buf = CZL_STR(fun->vars[1].val.str.Obj)->str;
    DWORD len = CZL_STR(fun->vars[1].val.str.Obj)->len;
    czl_com_handler *h;

    if (!extsrc)
        return 1;

    h = extsrc->src;

    if (!h->wOv.hEvent)
    {
        char ret = WriteFile(h->hCom, buf, len, &len, NULL);
        if (ret)
            fun->ret.val.inum = len;
    }
    else if (!WriteFile(h->hCom, buf, len, &len, &h->wOv) &&
            GetLastError() == ERROR_IO_PENDING)
    {
        WaitForSingleObject(h->wOv.hEvent, INFINITE);
        GetOverlappedResult(h->hCom, &h->wOv, &len, TRUE);
        fun->ret.val.inum = len;
    }
#else //CZL_SYSTEM_LINUX
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    int fd;
    struct epoll_event ev, events[1];

    if (!extsrc)
        return 1;

    fd = ((czl_com_handler*)(extsrc->src))->fd;

    if ((fun->ret.val.inum=write(fd, buf->str, buf->len)) >= 0)
        return 1;

    ev.events = EPOLLOUT;
    ev.data.fd = fd;

    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, fd, &ev) >= 0 &&
        epoll_wait(gp->kdpfd, events, 1, -1) > 0)
        fun->ret.val.inum = write(fd, buf->str, buf->len);

    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, fd, NULL);
#endif

    fun->ret.type = CZL_INT;
    return 1;
}

char czl_com_read(czl_gp *gp, czl_fun *fun)
{
    czl_extsrc *extsrc = czl_extsrc_get(fun->vars->val.Obj, czl_lib_com);

#ifdef CZL_SYSTEM_WINDOWS
    DWORD size;
    char tmp[1024];
    char *buf = tmp;
    char ret = 1;
    czl_com_handler *h;

    if (!extsrc)
        return 1;

    h = extsrc->src;

    if (!h->rOv.hEvent)
    {
        long len = (long)fun->vars[1].val.inum;
        if (len < 0 || (len > 1024 && !(buf=(char*)CZL_TMP_MALLOC(gp, len))))
            return 1;
        if (!ReadFile(h->hCom, buf, len, &size, NULL))
        {
            if (len > 1024)
                CZL_TMP_FREE(gp, buf, len);
            return 1;
        }
        ret = czl_set_ret_str(gp, &fun->ret, buf, size);
        if (len > 1024)
            CZL_TMP_FREE(gp, buf, len);
    }
    else if (!ReadFile(h->hCom, buf, 1024, &size, &h->rOv) &&
            GetLastError() == ERROR_IO_PENDING)
    {
        WaitForSingleObject(h->rOv.hEvent, INFINITE);
        GetOverlappedResult(h->hCom, &h->rOv, &size, TRUE);
        if (size)
            ret = czl_set_ret_str(gp, &fun->ret, buf, size);
    }
#else //CZL_SYSTEM_LINUX
    struct epoll_event ev, events[1];
    long len = fun->vars[1].val.inum;
    long ret;
    char tmp[1024];
    char *buf = tmp;
    int fd;
    unsigned long timeout;

    if (!extsrc)
        return 1;

    fd = ((czl_com_handler*)(extsrc->src))->fd;
    timeout = ((czl_com_handler*)(extsrc->src))->timeout;

    ev.events = EPOLLIN;
    ev.data.fd = fd;

    if (epoll_ctl(gp->kdpfd, EPOLL_CTL_ADD, fd, &ev) < 0 ||
        epoll_wait(gp->kdpfd, events, 1, timeout) <= 0 ||
        (len > 1024 && !(buf=(char*)CZL_TMP_MALLOC(gp, len))))
    {
        epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, fd, NULL);
        return 1;
    }
    epoll_ctl(gp->kdpfd, EPOLL_CTL_DEL, fd, NULL);

    if ((ret=read(fd, buf, len)) > 0)
        ret = czl_str_create(gp, &fun->ret.val.str, ret+1, ret, buf);
    else
        ret = -1;

    if (len > 1024)
        CZL_TMP_FREE(gp, buf, len);

    if (ret > 0)
        fun->ret.type = CZL_STRING;
#endif

    return ret;
}

//库函数定义
char czl_com_list(czl_gp *gp, czl_fun *fun)
{
    unsigned long i;
    void **obj = NULL;
    czl_array *arr = NULL;

    for (i = 0; i < 100; i++)
    {
    #ifdef CZL_SYSTEM_WINDOWS
        HANDLE hCom;
        char portName[8] = "COM";
        ltoa(i, portName+3, 10);
        hCom = CreateFile(portName, GENERIC_READ|GENERIC_WRITE,
                          0, NULL, OPEN_EXISTING, 0, NULL);
        if (hCom != INVALID_HANDLE_VALUE)
    #else //CZL_SYSTEM_LINUX
        int fd;
        char portName[8] = "ttyS";
        czl_itoa(i, portName+4);
        fd = open(portName, O_RDWR|O_NDELAY);
        if (fd >= 0)
    #endif
        {
            unsigned long len = strlen(portName);
        #ifdef CZL_SYSTEM_WINDOWS
            CloseHandle(hCom);
        #else //CZL_SYSTEM_LINUX
            close(fd);
        #endif
            if (!obj)
            {
                if (!(obj=czl_array_create(gp, 1, 0)))
                    return 0;
                arr = CZL_ARR(obj);
            }
            if ((++arr->cnt > arr->sum && !czl_array_resize(gp, arr, arr->cnt)) ||
                !czl_str_create(gp, &arr->vars[arr->cnt-1].val.str, len+1, len, portName))
            {
                czl_array_delete(gp, obj);
                return 0;
            }
            arr->vars[arr->cnt-1].type = CZL_STRING;
        }
    }

    if (obj)
    {
        fun->ret.type = CZL_ARRAY;
        fun->ret.val.Obj = obj;
    }

    return 1;
}
#endif //CZL_LIB_COM

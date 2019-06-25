#include "czl_com.h"

#ifdef CZL_LIB_COM
//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_com_open(czl_gp *gp, czl_fun *fun);    //打开串口
char czl_com_close(czl_gp *gp, czl_fun *fun);   //关闭串口
char czl_com_write(czl_gp *gp, czl_fun *fun);   //写串口
char czl_com_read(czl_gp *gp, czl_fun *fun);    //读串口
char czl_com_list(czl_gp *gp, czl_fun *fun);    //打印串口列表

//库函数表定义
const czl_sys_fun czl_lib_com[] =
{
    //函数名,    函数指针,        参数个数,  参数声明
    {"open",    czl_com_open,   3,       "str_v1,int_v2=9600,int_v3=0"},
    {"close",   czl_com_close,  1,       NULL},
    {"write",   czl_com_write,  2,       "&v1,str_v2"},
    {"read",    czl_com_read,   2,       "&v1,int_v2=1024"},
    {"list",    czl_com_list,   0,       NULL},
};

char czl_com_open(czl_gp *gp, czl_fun *fun)
{
#ifdef CZL_SYSTEM_WINDOWS
	void **obj;
    czl_array *arr;
    DCB sDcb;
    COMMTIMEOUTS timeOuts;
    char *portName = CZL_STR(fun->vars[0].val.str.Obj)->str;
    DWORD baudRate = (unsigned long)fun->vars[1].val.inum;
    unsigned long timeOut = (unsigned long)fun->vars[2].val.inum;
    DWORD type = (timeOut ? FILE_FLAG_OVERLAPPED : 0);
    HANDLE hCom = CreateFile(portName,
                             GENERIC_READ|GENERIC_WRITE,
                             0, NULL, OPEN_EXISTING, type, NULL);

    if (INVALID_HANDLE_VALUE == hCom)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

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

    if (!(obj=czl_array_create(gp, 3, 3)))
    {
        CloseHandle(hCom);
        fun->ret.val.inum = 0;
        return 1;
    }
	arr = CZL_ARR(obj);

    arr->vars[0].val.inum = (unsigned long)hCom;

    if (type)
    {
        czl_str str1, str2;
        HANDLE event1 = NULL, event2 = NULL;
        if (!(event1=CreateEvent(NULL, TRUE, TRUE, NULL)) ||
            !(event2=CreateEvent(NULL, TRUE, TRUE, NULL)))
        {
            CloseHandle(hCom);
            CloseHandle(event1);
            czl_array_delete(gp, obj);
            fun->ret.val.inum = 0;
            return 1;
        }
        if (!czl_str_create(gp, &str1, sizeof(OVERLAPPED),
                            sizeof(OVERLAPPED), NULL) ||
            !czl_str_create(gp, &str2, sizeof(OVERLAPPED),
                            sizeof(OVERLAPPED), NULL))
        {
            CloseHandle(hCom);
            CloseHandle(event1);
            CloseHandle(event2);
            czl_array_delete(gp, obj);
            fun->ret.val.inum = 0;
            return 1;
        }
        memset(CZL_STR(str1.Obj)->str, 0, sizeof(OVERLAPPED));
        memcpy(CZL_STR(str1.Obj)->str+sizeof(OVERLAPPED)-4, &event1, 4);
        memset(CZL_STR(str2.Obj)->str, 0, sizeof(OVERLAPPED));
        memcpy(CZL_STR(str2.Obj)->str+sizeof(OVERLAPPED)-4, &event2, 4);
        arr->vars[1].type = CZL_STRING;
        arr->vars[1].val.str = str1;
        arr->vars[2].type = CZL_STRING;
        arr->vars[2].val.str = str2;
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;
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
        fun->ret.val.inum = 0;
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

    if (tcsetattr(fd, TCSANOW, &uart) < 0)
    {
        close(fd);
        fun->ret.val.inum = 0;
        return 1;
    }

    fun->ret.val.ext.v1.i32 = fd;
    fun->ret.val.ext.v2.i32 = timeOut;
    return 1;
#endif
}

char czl_com_close(czl_gp *gp, czl_fun *fun)
{
#ifdef CZL_SYSTEM_WINDOWS
    czl_array *arr;

    if (fun->vars->type != CZL_ARRAY)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    arr = CZL_ARR(fun->vars->val.Obj); //获取数组指针

    if (3 == arr->cnt && CZL_INT == arr->vars[0].type)
    {
        fun->ret.val.inum = CloseHandle((HANDLE)((unsigned long)arr->vars[0].val.inum));
        if (CZL_STRING == arr->vars[1].type &&
            sizeof(OVERLAPPED) == CZL_STR(arr->vars[1].val.str.Obj)->len &&
            CZL_STRING == arr->vars[2].type &&
            sizeof(OVERLAPPED) == CZL_STR(arr->vars[2].val.str.Obj)->len)
        {
            HANDLE event;
            memcpy(&event, CZL_STR(arr->vars[1].val.str.Obj)->str+sizeof(OVERLAPPED)-4, 4);
            fun->ret.val.inum = CloseHandle(event);
            memcpy(&event, CZL_STR(arr->vars[2].val.str.Obj)->str+sizeof(OVERLAPPED)-4, 4);
            fun->ret.val.inum = CloseHandle(event);
        }
    }
    else
        fun->ret.val.inum = 0;
    return 1;
#else //CZL_SYSTEM_LINUX
    if (fun->vars->type != CZL_INT || -1 == close(fun->vars->val.ext.v1.i32))
        fun->ret.val.inum = 0;
    else
        fun->ret.val.inum = 1;
    return 1;
#endif
}

char czl_com_write(czl_gp *gp, czl_fun *fun)
{
#ifdef CZL_SYSTEM_WINDOWS
    czl_array *arr;
    czl_var *h = CZL_GRV(fun->vars);
    HANDLE hCom;
    char *buf = CZL_STR(fun->vars[1].val.str.Obj)->str;
    DWORD len = CZL_STR(fun->vars[1].val.str.Obj)->len;

    fun->ret.val.inum = 0;

    if (h->type != CZL_ARRAY)
        return 1;

    arr = CZL_ARR(h->val.Obj);
    if (arr->cnt != 3 || arr->vars[0].type != CZL_INT)
        return 1;

    hCom = (HANDLE)((unsigned long)arr->vars[0].val.inum);

    if (CZL_INT == arr->vars[1].type)
    {
        char ret = WriteFile(hCom, buf, len, &len, NULL);
        if (ret)
            fun->ret.val.inum = len;
    }
    else if (CZL_STRING == arr->vars[1].type &&
             sizeof(OVERLAPPED) == CZL_STR(arr->vars[1].val.str.Obj)->len)
    {
        OVERLAPPED ov;
        memcpy(&ov, CZL_STR(arr->vars[1].val.str.Obj)->str, sizeof(OVERLAPPED));
        if (!WriteFile(hCom, buf, len, &len, &ov) && GetLastError() == ERROR_IO_PENDING)
        {
            WaitForSingleObject(ov.hEvent, INFINITE);
            GetOverlappedResult(hCom, &ov, &len, TRUE);
            fun->ret.val.inum = len;
        }
        memcpy(CZL_STR(arr->vars[1].val.str.Obj)->str, &ov, sizeof(OVERLAPPED));
    }

    return 1;
#else //CZL_SYSTEM_LINUX
    czl_var *h = CZL_GRV(fun->vars);
    czl_string *buf = CZL_STR(fun->vars[1].val.str.Obj);
    fd_set fdWrite;
    int fd;

    if (h->type != CZL_INT)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    fd = h->val.ext.v1.i32;

    FD_ZERO(&fdWrite);
    FD_SET(fd, &fdWrite);
    if (select(fd+1, NULL, &fdWrite, NULL, NULL) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum = write(fd, buf->str, buf->len);

    return 1;
#endif
}

char czl_com_read(czl_gp *gp, czl_fun *fun)
{
#ifdef CZL_SYSTEM_WINDOWS
    czl_var *h = CZL_GRV(fun->vars);
    czl_array *arr;
    DWORD size;
    HANDLE hCom;
    char tmp[1024];
    char *buf = tmp;
    char ret = 1;

    fun->ret.val.inum = 0;

    if (h->type != CZL_ARRAY)
        return 1;

    arr = CZL_ARR(h->val.Obj);
    if (arr->cnt != 3 || arr->vars[0].type != CZL_INT)
        return 1;

    hCom = (HANDLE)((unsigned long)arr->vars[0].val.inum);

    if (CZL_INT == arr->vars[1].type)
    {
        long len = (long)fun->vars[1].val.inum;
        if (len < 0 || (len > 1024 && !(buf=(char*)CZL_TMP_MALLOC(gp, len))))
            return 1;
        if (!ReadFile(hCom, buf, len, &size, NULL))
        {
            if (len > 1024)
                CZL_TMP_FREE(gp, buf, len);
            return 1;
        }
        ret = czl_set_ret_str(gp, &fun->ret, buf, size);
        if (len > 1024)
            CZL_TMP_FREE(gp, buf, len);
    }
    else if (CZL_STRING == arr->vars[2].type &&
             sizeof(OVERLAPPED) == CZL_STR(arr->vars[2].val.str.Obj)->len)
    {
        OVERLAPPED ov;
        memcpy(&ov, CZL_STR(arr->vars[2].val.str.Obj)->str, sizeof(OVERLAPPED));
        if (!ReadFile(hCom, buf, 1024, &size, &ov) && GetLastError() == ERROR_IO_PENDING)
        {
            WaitForSingleObject(ov.hEvent, INFINITE);
            GetOverlappedResult(hCom, &ov, &size, TRUE);
            if (size)
                ret = czl_set_ret_str(gp, &fun->ret, buf, size);
        }
        memcpy(CZL_STR(arr->vars[2].val.str.Obj)->str, &ov, sizeof(OVERLAPPED));
    }

    return ret;
#else //CZL_SYSTEM_LINUX
    czl_var *h = CZL_GRV(fun->vars);
    struct timeval timeout;
    fd_set fdRead;
    int fd;
    long time;
    long ret;
    long len = fun->vars[1].val.inum;
    char tmp[1024];
    char *buf = tmp;

    fun->ret.val.inum = 0;

    if (h->type != CZL_INT || len <= 0)
        return 1;

    fd = h->val.ext.v1.i32;
    time = h->val.ext.v2.i32;

    if (time)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = time*1000;
    }
    else
    {
        timeout.tv_sec = 2000000000;
        timeout.tv_usec = 0;
    }

    FD_ZERO(&fdRead);
    FD_SET(fd, &fdRead);
    if (select(fd+1, &fdRead, NULL, NULL, &timeout) < 0 ||
        !FD_ISSET(fd, &fdRead) ||
        (len > 1024 && !(buf=(char*)CZL_TMP_MALLOC(gp, len))))
        return 1;

    if ((ret=read(fd, buf, len)) > 0)
        ret = czl_str_create(gp, &fun->ret.val.str, ret+1, ret, buf);
    else
        ret = -1;

    if (len > 1024)
        CZL_TMP_FREE(gp, buf, len);

    if (ret > 0)
        fun->ret.type = CZL_STRING;

    return ret;
#endif
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
    else
        fun->ret.val.inum = 0;

    return 1;
}
#endif //CZL_LIB_COM

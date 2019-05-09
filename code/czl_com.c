#include "czl_com.h"

#ifdef CZL_LIB_COM
//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_com_list(czl_gp *gp, czl_fun *fun);    //打印串口列表
char czl_com_open(czl_gp *gp, czl_fun *fun);    //打开串口
char czl_com_close(czl_gp *gp, czl_fun *fun);   //关闭串口
char czl_com_write(czl_gp *gp, czl_fun *fun);   //写串口
char czl_com_read(czl_gp *gp, czl_fun *fun);    //读串口

//库函数表定义
const czl_sys_fun czl_lib_com[] =
{
    //函数名,    函数指针,        参数个数,  参数声明
    {"list",    czl_com_list,   0,       NULL},
    {"open",    czl_com_open,   3,       "str_v1,int_v2=9600,int_v3=0"},
    {"close",   czl_com_close,  1,       "&arr_v1"},
    {"write",   czl_com_write,  2,       "&arr_v1,str_v2"},
    {"read",    czl_com_read,   2,       "&arr_v1,int_v2=1024"},
};

//库函数定义
char czl_com_list(czl_gp *gp, czl_fun *fun)
{
    char *portName;
    unsigned long i;
    unsigned char *pBite = NULL;    //包含PORT_INFO_1 或 PORT_INFO_2结构的缓冲区
    unsigned long pcbNeeded = 0;	//lpbPorts缓冲区中的字符数量
    unsigned long pcReturned = 0;	//载入缓冲区的结构数量（用于那些能返回多个结构的函数）
    PORT_INFO_2 *pPort;
    czl_array *arr = NULL;
	void **obj;

    //获取端口信息，能得到端口信息的大小 pcbNeeded
    EnumPorts(NULL, 2, pBite, 0, &pcbNeeded, &pcReturned);
    pBite = (unsigned char*)CZL_TMP_MALLOC(gp, pcbNeeded*sizeof(unsigned char));
    //枚举端口，能得到端口的具体信息 pBite 以及端口的的个数 pcReturned
    EnumPorts(NULL, 2, pBite, pcbNeeded, &pcbNeeded, &pcReturned);
    pPort = (PORT_INFO_2*)pBite;
    for (i = 0; i < pcReturned; i++)
    {
        portName = pPort[i].pPortName;
        //串口信息的具体确定
        if (memcmp("COM", portName, 3) == 0)
        {
            HANDLE hCom;
            unsigned long len = strlen(portName)-1;
            portName[len] = '\0';
            hCom = CreateFile(portName,
                              GENERIC_READ|GENERIC_WRITE,
                              0, NULL, OPEN_EXISTING, 0, NULL);
            if (hCom != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hCom);
				 if (!obj)
				{
					if (!(obj=czl_array_create(gp, 1, 0)))
						goto CZL_END;
					arr = CZL_ARR(obj);
				}
                if (++arr->cnt > arr->sum &&
                    !czl_array_resize(gp, arr, arr->cnt))
                {
                    czl_array_delete(gp, obj);
                    goto CZL_END;
                }
                if (!czl_str_create(gp, &arr->vars[arr->cnt-1].val.str,
                                    len+1, len, portName))
                {
                    czl_array_delete(gp, obj);
                    goto CZL_END;
                }
                arr->vars[arr->cnt-1].type = CZL_STRING;
            }
        }
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;

CZL_END:
    fun->ret.val.inum = 0;
    CZL_TMP_FREE(gp, pBite, pcbNeeded*sizeof(unsigned char));
    return 1;
}

char czl_com_open(czl_gp *gp, czl_fun *fun)
{
	void **obj;
    czl_array *arr;
    czl_str str1, str2;
    HANDLE event1 = NULL, event2 = NULL;
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

    if (type)
    {
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
    }

    arr->vars[0].val.inum = (unsigned long)hCom;
    if (type)
    {
        arr->vars[1].type = CZL_STRING;
        arr->vars[1].val.str = str1;
        arr->vars[2].type = CZL_STRING;
        arr->vars[2].val.str = str2;
    }

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;

    return 1;
}

char czl_com_close(czl_gp *gp, czl_fun *fun)
{
    czl_array *arr;
    czl_var *h = CZL_GCRV(fun->vars); //获取引用指向的实际变量
    if (!h)
        return 0; //返回0表示入参不正确，系统抛出异常

    arr = CZL_ARR(h->val.Obj); //获取数组指针
    if (3 == arr->cnt && CZL_INT == arr->vars[0].type)
    {
        CloseHandle((HANDLE)((unsigned long)arr->vars[0].val.inum));
        if (CZL_STRING == arr->vars[1].type &&
            sizeof(OVERLAPPED) == CZL_STR(arr->vars[1].val.str.Obj)->len &&
            CZL_STRING == arr->vars[2].type &&
            sizeof(OVERLAPPED) == CZL_STR(arr->vars[2].val.str.Obj)->len)
        {
            HANDLE event;
            memcpy(&event, CZL_STR(arr->vars[1].val.str.Obj)->str+sizeof(OVERLAPPED)-4, 4);
            CloseHandle(event);
            memcpy(&event, CZL_STR(arr->vars[2].val.str.Obj)->str+sizeof(OVERLAPPED)-4, 4);
            CloseHandle(event);
        }
        fun->ret.val.inum = 1; //"close"函数返回值是1
    }
    else
    {
        fun->ret.val.inum = 0; //"close"函数返回值是0
    }
    return 1; //返回1表示函数执行正确，正常返回
}

char czl_com_write(czl_gp *gp, czl_fun *fun)
{
    czl_array *arr;
    czl_var *h = CZL_GCRV(fun->vars);
    if (!h)
        return 0;

    arr = CZL_ARR(h->val.Obj);
    if (3 == arr->cnt && CZL_INT == arr->vars[0].type)
    {
        HANDLE hCom = (HANDLE)((unsigned long)arr->vars[0].val.inum);
        char *buf = CZL_STR(fun->vars[1].val.str.Obj)->str;
        DWORD len = CZL_STR(fun->vars[1].val.str.Obj)->len;
        if (CZL_INT == arr->vars[1].type)
        {
            char ret = WriteFile(hCom, buf, len, &len, NULL);
            fun->ret.val.inum = (ret ? 1 : 0);
        }
        else if (CZL_STRING == arr->vars[1].type &&
                 sizeof(OVERLAPPED) == CZL_STR(arr->vars[1].val.str.Obj)->len)
        {
            OVERLAPPED ov;
            memcpy(&ov, CZL_STR(arr->vars[1].val.str.Obj)->str, sizeof(OVERLAPPED));
            if (!WriteFile(hCom, buf, len, &len, &ov))
            {
                if (GetLastError() != ERROR_IO_PENDING)
                    fun->ret.val.inum = 0;
                else
                {
                    WaitForSingleObject(ov.hEvent, INFINITE);
                    GetOverlappedResult(hCom, &ov, &len, TRUE);
                    fun->ret.val.inum = 1;
                }
            }
            memcpy(CZL_STR(arr->vars[1].val.str.Obj)->str, &ov, sizeof(OVERLAPPED));
        }
        else
            fun->ret.val.inum = 0;
    }
    else
        fun->ret.val.inum = 0;

    return 1;
}

char czl_com_read(czl_gp *gp, czl_fun *fun)
{
    char ret = 1;
    czl_array *arr;
    czl_var *h = CZL_GCRV(fun->vars);
    if (!h)
        return 0;

    arr = CZL_ARR(h->val.Obj);
    if (3 == arr->cnt && CZL_INT == arr->vars[0].type)
    {
        DWORD size;
        char tmp[1024];
        char *buf = tmp;
        HANDLE hCom = (HANDLE)((unsigned long)arr->vars[0].val.inum);
        if (CZL_INT == arr->vars[1].type)
        {
            long len = (long)fun->vars[1].val.inum;
            if (len < 0 || (len > 1024 && !(buf=(char*)CZL_TMP_MALLOC(gp, len))))
            {
                fun->ret.val.inum = 0;
                return 1;
            }
            if (!ReadFile(hCom, buf, len, &size, NULL))
            {
                if (len > 1024)
                    CZL_TMP_FREE(gp, buf, len);
                fun->ret.val.inum = 0;
                return 1;
            }
            ret = czl_set_ret_str(gp, fun, buf, size);
            if (len > 1024)
                CZL_TMP_FREE(gp, buf, len);
        }
        else if (CZL_STRING == arr->vars[2].type &&
                 sizeof(OVERLAPPED) == CZL_STR(arr->vars[2].val.str.Obj)->len)
        {
            OVERLAPPED ov;
            memcpy(&ov, CZL_STR(arr->vars[2].val.str.Obj)->str, sizeof(OVERLAPPED));
            if (!ReadFile(hCom, buf, 1024, &size, &ov))
            {
                if (GetLastError() != ERROR_IO_PENDING)
                    fun->ret.val.inum = 0;
                else
                {
                    WaitForSingleObject(ov.hEvent, INFINITE);
                    GetOverlappedResult(hCom, &ov, &size, TRUE);
                    if (size)
                        ret = czl_set_ret_str(gp, fun, buf, size);
                    else
                        fun->ret.val.inum = 0;
                }
            }
            memcpy(CZL_STR(arr->vars[2].val.str.Obj)->str, &ov, sizeof(OVERLAPPED));
        }
        else
            fun->ret.val.inum = 0;
    }
    else
        fun->ret.val.inum = 0;

    return ret;
}
#endif //CZL_LIB_COM

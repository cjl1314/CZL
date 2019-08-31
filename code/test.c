#include "czl_lib.h"

#ifndef CZL_CONSOLE
char czl_ext_rand(czl_gp *gp, czl_fun *fun);

const czl_sys_fun czl_lib_ext[] =
{
    {"rand", czl_ext_rand, 0, NULL},
};

char czl_ext_rand(czl_gp *gp, czl_fun *fun)
{
    fun->ret.type = CZL_INT;
    fun->ret.val.inum = rand();
    return 1;
}

//定义CZL虚拟机通信数据结构
typedef struct czl_data
{
    int v1;
    float v2;
    char *v3;
} czl_data;

int main(void)
{
    czl_data data = {1, 1.5, "foobar"};

    //打开一个CZL虚拟机，脚本是main.czl，log打印到终端，数据通信类名是data，使用扩展库czl_lib_ext
    czl_gp *gp = czl_open("main.czl", NULL, "data", czl_lib_ext, 1);
    if (!gp)
        return 0;

    czl_insert(gp, 0, &data);

    czl_exec(gp); //执行shell.txt
    czl_get(gp, 1, &data);

    czl_exec(gp); //再次执行shell.txt
    czl_get(gp, 0, &data);

    czl_close(gp); //关闭虚拟机，否则会产生内存泄露
    return 0;
}
#endif //#ifndef CZL_CONSOLE

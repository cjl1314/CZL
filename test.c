#include "czl_lib.h"

#ifndef CZL_CONSOLE
int main(void)
{
    czl_gp *gp = czl_open("shell.txt", "czl.log"); //打开一个CZL虚拟机，入口脚本是shell.txt，log打印到czl.log文件
    if (!gp)
        return 0;

    czl_exec(gp); //执行shell.txt

    czl_var res;
    while (czl_pop(gp, &res)) //获取虚拟机栈数据
    {
        switch (czl_type(res)) //根据不同类型打印数据
        {
        case CZL_INT: printf(": %d\n", czl_inum(res)); break;
        case CZL_FLOAT: printf(": %lf\n", czl_fnum(res)); break;
        default:
            printf(": %d, %s\n", czl_strlen(res), czl_str(res));
            czl_dels(gp, res); //字符串用完必须释放内存，否则会导致内存泄露
            break;
        }
    }

    czl_pushi(gp, 10); //向虚拟机栈压入数据
    czl_pushf(gp, 6.7);
    czl_pushs(gp, "foobar");
    czl_exec(gp); //再次执行shell.txt

    czl_close(gp); //关闭虚拟机，否则会产生内存泄露

    return 0;
}
#endif //#ifndef CZL_CONSOLE

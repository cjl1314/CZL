#include "czl_lib.h"

#ifndef CZL_CONSOLE
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
    //打开一个CZL虚拟机，入口脚本是main.czl，log打印到czl.log文件，数据结构是data
    czl_gp *gp = czl_open("main.czl", "czl.log", "data");
    if (!gp)
        return 0;

    czl_insert(gp, 0, &data);

    czl_exec(gp); //执行shell.txt

    czl_get(gp, 1, &data);
    printf("%d\n", data.v1);
    printf("%f\n", data.v2);
    printf("%s\n", data.v3);

    czl_exec(gp); //再次执行shell.txt

    czl_get(gp, 0, &data);
    printf("%d\n", data.v1);
    printf("%f\n", data.v2);
    printf("%s\n", data.v3);

    czl_close(gp); //关闭虚拟机，否则会产生内存泄露

    return 0;
}
#endif //#ifndef CZL_CONSOLE

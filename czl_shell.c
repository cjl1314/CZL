#include "czl_paser.h"
#include "czl_vm.h"

#ifdef CZL_CONSOLE
int main(int argc, char **argv)
{
    czl_gp gp; //全局参数句柄

#ifdef CZL_DEBUG
    char *shell_path = "shell.txt";
#else
    char *shell_path;
    if (1 == argc)
        return 1;
    if (strcmp("-v", argv[1]) == 0)
    {
        printf("CZL 0.0.1 (built: October 29 2018 12:00:00)\n");
        printf("Copyright 2017-2018 by JL.C\n");
        printf("CZL included register-model, multi-thread, gc, class, table and so on\n");
        printf("CZL is high-performance, provincial-memory, powerful-function and easy-to-use\n");
        return 1;
    }
    shell_path = argv[1];
#endif

    //系统初始化
    if (!czl_sys_init(&gp, 1))
    {
        czl_init_free(&gp, 1);
        czl_log(&gp, "system init error!\n");
        return 0;
    }

    czl_exec_shell(&gp, shell_path); //解析执行脚本
    czl_memory_free(&gp); //释放内存

    return 1;
}
#endif //#ifdef CZL_CONSOLE

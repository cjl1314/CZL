#include "czl_paser.h"
#include "czl_vm.h"

#ifdef CZL_CONSOLE
int main(int argc, char **argv)
{
    czl_gp gp;

#ifdef CZL_DEBUG
    char *shell_path = "main.czl";
#else
    char *shell_path;
    if (1 == argc)
        return 1;
    if (strcmp("-v", argv[1]) == 0)
    {
        printf("CZL 1.0.0 (built: April 05 2019 12:00:00)\n");
        printf("Copyright 2017-2019 by JL.C\n");
        printf("CZL can run never restart\n");
        return 1;
    }
    shell_path = argv[1];
#endif

    memset(&gp, 0, sizeof(czl_gp));

    if (argc > 2)
    {
        gp.enter_var.quality = CZL_FUNRET_VAR;
        gp.enter_var.val.ext.v1.ptr = argv+2;
        gp.enter_var.val.ext.v2.u32 = argc-2;
    }

    if (!czl_sys_init(&gp))
    {
        czl_init_free(&gp, 1);
        czl_log(&gp, "system init error!\n");
        return 0;
    }

    czl_exec_shell(&gp, shell_path, 1);
    czl_memory_free(&gp);

    return 1;
}
#endif //#ifdef CZL_CONSOLE

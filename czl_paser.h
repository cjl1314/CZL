#ifndef CZL_PASER_H
#define CZL_PASER_H

#include "czl_vm.h"

char czl_get_para_ot(czl_gp*, char*);
char czl_is_constant(czl_gp*, char**, czl_exp_node**);
char czl_sys_init(czl_gp*);
char czl_global_paras_init(czl_gp*);
char czl_exec_shell(czl_gp*, char*, char);
void czl_mm_print(czl_gp*);
#ifndef CZL_CONSOLE
char czl_exec_prepare(czl_gp*);
#endif //#ifndef CZL_CONSOLE

#endif //CZL_PASER_H

#ifndef CZL_LIB_H
#define CZL_LIB_H

#include "czl_vm.h"

extern const czl_sys_lib czl_syslibs[];
extern const unsigned long czl_syslibs_num;

#define CZL_STRUCT_FILE_MAX_SZIE 1*1024*1024*1024 //1G

#ifdef CZL_SYSTEM_LINUX
    #define CZL_CLOCK (clock()/1000)
#elif defined CZL_SYSTEM_WINDOWS
    #define CZL_CLOCK clock()
#endif

czl_var* CZL_GCRV(czl_var*);
char czl_ret_clean(czl_gp*, czl_var*);
char czl_set_ret_str(czl_gp*, czl_fun*, const char*, unsigned long);
//
unsigned long czl_itoa(czl_long, char*);
unsigned long czl_ftoa(double, char*, int);
char* czl_get_number_from_str(char*, czl_var*);
char czl_line_read(czl_gp*, FILE*, czl_var*);
int czl_get_int_mode(char*);
//
#ifdef CZL_MULT_THREAD
void czl_thread_lock
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION*
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t*
#endif
);
void czl_thread_unlock
(
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION*
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t*
#endif
);
void czl_event_send
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE*
#elif defined CZL_SYSTEM_LINUX
    sem_t*
#endif
);
char czl_notify_paras_create(czl_gp*, czl_var*, czl_thread_pipe*);
void czl_thread_pipe_delete(czl_thread_pipe*);
#endif //#ifdef CZL_MULT_THREAD
//
#ifndef CZL_CONSOLE
#define czl_type(res) res.type
#define czl_inum(res) res.val.inum
#define czl_fnum(res) res.val.fnum
#define czl_str(res) res.val.str.s->str
#define czl_strlen(res) res.val.str.s->len
char czl_pushi(czl_gp*, czl_long);
char czl_pushf(czl_gp*, double);
char czl_pushs(czl_gp*, char*);
char czl_pop(czl_gp*, czl_var*);
void czl_dels(czl_gp*, czl_var);
unsigned long czl_size(czl_gp*);
char czl_clean(czl_gp*);
char czl_exec(czl_gp*);
char czl_exit(czl_gp*);
czl_gp* czl_open(char*, char*);
void czl_close(czl_gp*);
#endif //#ifndef CZL_CONSOLE

#endif //CZL_LIB_H

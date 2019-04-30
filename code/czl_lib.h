#ifndef CZL_LIB_H
#define CZL_LIB_H

#include "czl_vm.h"

extern const czl_sys_lib czl_syslibs[];
extern const unsigned long czl_syslibs_num;

#define CZL_FILE_MAX_SZIE   CZL_MM_4GB
#define CZL_FILE_CHECK_SUM  0xF1E2C3B4

#ifdef CZL_SYSTEM_LINUX
    #define CZL_CLOCK (clock()/1000)
    #define CZL_SLEEP(t) usleep(t*1000)
#elif defined CZL_SYSTEM_WINDOWS
    #define CZL_CLOCK clock()
    #define CZL_SLEEP(t) Sleep(t)
#else
    #define CZL_CLOCK ()
    #define CZL_SLEEP(t) (t)
#endif

czl_var* CZL_GCRV(czl_var*);
char czl_ret_clean(czl_gp*, czl_var*);
char czl_set_ret_str(czl_gp*, czl_var*, const char*, unsigned long);
//
unsigned long czl_itoa(czl_long, char*);
unsigned long czl_ftoa(double, char*, int);
char* czl_get_number_from_str(char*, czl_var*);
char czl_line_read(czl_gp*, FILE*, czl_var*);
char czl_obj_read(czl_gp*, FILE*, czl_var*);
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
char czl_thread_para_get(czl_gp*, czl_var*, czl_thread_pipe*);
void czl_thread_pipe_delete(czl_thread_pipe*);
#endif //#ifdef CZL_MULT_THREAD
//
#ifndef CZL_CONSOLE
char czl_insert(czl_gp*, int, void*);
char czl_get(czl_gp*, int, void*);
char czl_delete(czl_gp*, int);
unsigned long czl_count(czl_gp*);
char czl_clean(czl_gp*);
char czl_exec(czl_gp*);
czl_gp* czl_open(char*, char*, char*);
void czl_close(czl_gp*);
#endif //#ifndef CZL_CONSOLE

#endif //CZL_LIB_H

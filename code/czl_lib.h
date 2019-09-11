#ifndef CZL_LIB_H
#define CZL_LIB_H

///////////////////////////////////////////////////////////////
#define CZL_LIB_COM
#define CZL_LIB_TCP
#define CZL_LIB_UDP
#define CZL_LIB_HTTP
#define CZL_LIB_WS
#define CZL_LIB_REG
#define CZL_LIB_SQL
///////////////////////////////////////////////////////////////
#include "czl_vm.h"
extern czl_sys_lib czl_syslibs[];
extern const unsigned long czl_syslibs_num;
extern const czl_sys_fun czl_lib_os[];
///////////////////////////////////////////////////////////////
//获取引用变量: get ref var
#define CZL_GRV(p) \
((p)->val.ref.inx < 0 ? (p)->val.ref.var : \
 (czl_var*)(((czl_array*)(p)->val.ref.var)->vars + (p)->val.ref.inx))
///////////////////////////////////////////////////////////////
#define CZL_BUF_SIZE 10*1024
extern const unsigned long CZL_CHECK_SUM;
char czl_sizeof_obj(czl_gp*, char, const czl_var*, unsigned long*);
char* czl_get_obj_buf(czl_gp*, const czl_var*, char*);
///////////////////////////////////////////////////////////////
czl_var* CZL_GCRV(czl_var*);
char czl_ret_clean(czl_gp*, czl_var*);
char czl_set_ret_str(czl_gp*, czl_var*, const char*, unsigned long);
///////////////////////////////////////////////////////////////
unsigned long czl_itoa(czl_long, char*);
unsigned long czl_ftoa(double, char*, int);
char* czl_get_number_from_str(char*, czl_var*);
char czl_obj_read(czl_gp*, FILE*, czl_var*);
char czl_line_read(czl_gp*, FILE*, czl_var*);
unsigned long czl_rand_str(char*);
czl_long czl_get_file_size(FILE*);
int czl_get_int_mode(char*);
char czl_strcmp(const char*, const char*);
char* czl_numstr_ignore_sign_filt(char*);
#ifdef CZL_SYSTEM_WINDOWS
char czl_bytes_read(czl_gp*, FILE*, unsigned char, czl_var*, long);
#else
char czl_bytes_read(czl_gp*, FILE*, czl_var*, long);
#endif
char* czl_dns(char*);
long czl_net_send(czl_gp*, int, char*, long);
///////////////////////////////////////////////////////////////
#ifdef CZL_MULT_THREAD
typedef struct czl_threads_handler
{
    czl_gp *gp;
    czl_thread *head;
    czl_thread *cur;
    czl_thread *recv;
    unsigned long count;
    unsigned long limit;
    unsigned long cnt;
    unsigned long sum;
    czl_str shell_path;
#ifndef CZL_CONSOLE
    czl_str log_path;
#endif
    czl_var para;
} czl_threads_handler;

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
void czl_event_wait
(
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE *event
#elif defined CZL_SYSTEM_LINUX
    sem_t *event
#endif
);
char czl_thread_para_get(czl_gp*, czl_var*, czl_thread_pipe*);
void czl_thread_pipe_delete(czl_thread_pipe*);
#endif //#ifdef CZL_MULT_THREAD
///////////////////////////////////////////////////////////////
#ifndef CZL_CONSOLE
#if defined CZL_SYSTEM_WINDOWS && defined CZL_DLL
    #define CZL_EXT  extern __declspec(dllimport)
#else
    #define CZL_EXT extern
#endif
CZL_EXT char czl_insert(czl_gp*, int, void*);
CZL_EXT char czl_get(czl_gp*, int, void*);
CZL_EXT char czl_delete(czl_gp*, int);
CZL_EXT unsigned long czl_count(czl_gp*);
CZL_EXT char czl_clean(czl_gp*);
CZL_EXT char czl_exec(czl_gp*);
CZL_EXT czl_gp* czl_open(char*, char*, char*, const czl_sys_fun*, unsigned long);
CZL_EXT void czl_close(czl_gp*);
#endif //#ifndef CZL_CONSOLE
///////////////////////////////////////////////////////////////
#endif //CZL_LIB_H

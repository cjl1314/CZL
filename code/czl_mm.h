#ifndef CZL_MM_H
#define CZL_MM_H

#define CZL_NUMBER_64bit    //64位数值类型支持宏
#define CZL_MULT_THREAD     //多线程支持宏
#define CZL_TIMER           //定时器支持宏

#define CZL_MM_MODULE       //内存管理模块宏
#define CZL_CONSOLE         //控制台运行方式宏，关了就运行在扩展方式
//#define CZL_DEBUG           //调试模式

///////////////////////////////////////////////////////////////
#ifdef _WIN32
    #define CZL_SYSTEM_WINDOWS  //运行在windows系统宏
#elif defined __linux
    #define CZL_SYSTEM_LINUX    //运行在linux系统宏
#endif

#ifdef CZL_SYSTEM_WINDOWS
    #include <windows.h>
    #include <sys\stat.h>
    #ifdef CZL_NUMBER_64bit
        typedef __int64 czl_long;
        typedef unsigned long czl_ulong;
        typedef double czl_float;
    #else
        typedef long czl_long;
        typedef unsigned long czl_ulong;
        typedef float czl_float;
    #endif
#elif defined CZL_SYSTEM_LINUX
    #include <unistd.h>
    #ifdef CZL_MULT_THREAD
        #include <pthread.h>
        #include <semaphore.h>
    #endif
    #ifdef CZL_TIMER
        #include <signal.h>
        #include <time.h>
    #endif
    #ifdef CZL_NUMBER_64bit
        typedef long long czl_long;
        typedef unsigned long czl_ulong;
        typedef double czl_float;
    #else
        typedef long czl_long;
        typedef unsigned long czl_ulong;
        typedef float czl_float;
    #endif
#endif
///////////////////////////////////////////////////////////////
#define CZL_MM_4GB   0xFFFFFFFFUL
#define CZL_MM_3GB   3*1024*1024*1024UL
#define CZL_MM_50MB  50*1024*1024UL
#define CZL_MM_4MB   4*1024*1024UL
///////////////////////////////////////////////////////////////
#ifdef CZL_MM_MODULE

#define CZL_MM_RT_GC
#define CZL_MM_CACHE
///////////////////////////////////////////////////////////////
#ifdef CZL_MM_CACHE

#define CZL_MM_CACHE_RANGE   8188 //1048576 //160kb
#define CZL_MM_CACHE_GAP     128
//由 CZL_MM_SP_515B+1 + n*CZL_MM_CACHE_GAP >= 1024*1024 推出 n >= 8188
#define CZL_MM_1MB  1024*1024

#define CZL_MM_CACHE_HEAD_SIZE  12

#define CZL_MM_CACHE_GET(head) (heap-CZL_MM_CACHE_HEAD_SIZE)

typedef struct czl_mm_cache
{
    struct czl_mm_cache *next;
    struct czl_mm_cache *last;
    unsigned char null; //保留
    unsigned char state;
    unsigned short index;
    char heap[4];
} czl_mm_cache;

typedef struct czl_mm_cache_pool
{
    czl_mm_cache *head;
    czl_mm_cache *tail;
    unsigned long size;
} czl_mm_cache_pool;

typedef struct czl_mm_cache_handle
{
    czl_mm_cache_pool *pools;
    unsigned long count;
    unsigned char flag;
} czl_mm_cache_handle;

#endif //#ifdef CZL_MM_CACHE
///////////////////////////////////////////////////////////////
#define CZL_MM_SP_RANGE         40
#define CZL_MM_SP_515B          515

#define CZL_MM_SP_HEAP_NUM_MIN  1
#define CZL_MM_SP_HEAP_NUM_MAX  200

#define CZL_MM_SP_HEAD_SIZE     24

#define CZL_MM_OBJ_HEAP_MSG_SIZE 7
#define CZL_MM_VAR_HEAP_MSG_SIZE 3
#define CZL_MM_BUF_HEAP_MSG_SIZE 1

#define CZL_MM_OBJ_SP_SIZE(size, num) \
(CZL_MM_SP_HEAD_SIZE + (CZL_MM_OBJ_HEAP_MSG_SIZE+size)*(num))

#define CZL_MM_VAR_SP_SIZE(size, num) \
(CZL_MM_SP_HEAD_SIZE + (CZL_MM_VAR_HEAP_MSG_SIZE+size)*(num))

#define CZL_MM_BUF_SP_SIZE(size, num) \
(CZL_MM_SP_HEAD_SIZE + (CZL_MM_BUF_HEAP_MSG_SIZE+size)*(num))

//偏移id必须强转为unsigned char型，否则当id=128时会出错
#define CZL_MM_OBJ_HEAP_GET(head, id, size) \
(head + (CZL_MM_OBJ_HEAP_MSG_SIZE+size)*((unsigned char)id-1))

#define CZL_MM_VAR_HEAP_GET(head, id, size) \
(head + (CZL_MM_VAR_HEAP_MSG_SIZE+size)*((unsigned char)id-1))

#define CZL_MM_BUF_HEAP_GET(head, id, size) \
(head + (CZL_MM_BUF_HEAP_MSG_SIZE+size)*((unsigned char)id-1))

#define CZL_MM_OBJ_PAGE_GET(heap, size) \
(heap - (CZL_MM_OBJ_HEAP_MSG_SIZE+size)*(*((unsigned char*)heap-6)-1) - \
         CZL_MM_OBJ_HEAP_MSG_SIZE - CZL_MM_SP_HEAD_SIZE)

#define CZL_MM_VAR_PAGE_GET(heap, size) \
(heap - (CZL_MM_VAR_HEAP_MSG_SIZE+size)*(*((unsigned char*)heap-2)-1) - \
         CZL_MM_VAR_HEAP_MSG_SIZE - CZL_MM_SP_HEAD_SIZE)

#define CZL_MM_BUF_PAGE_GET(heap, size) \
(heap - (CZL_MM_BUF_HEAP_MSG_SIZE+size)*(*((unsigned char*)heap-1)-1) - \
         CZL_MM_BUF_HEAP_MSG_SIZE - CZL_MM_SP_HEAD_SIZE)

#define CZL_MM_SP_THRESHOLD(rank, num) (0.005*rank*num)

typedef enum czl_mm_sp_type_enum
{
    CZL_MM_OBJ_SP,
    CZL_MM_VAR_SP,
    CZL_MM_BUF_SP,
    CZL_MM_UNDEF_SP
} czl_mm_sp_type_enum;

typedef struct czl_mm_sp
{
    unsigned char heapNum;
    unsigned char useNum;
    unsigned char freeNum;
    unsigned char useHead;
    unsigned char freeHead;
    unsigned char restHead;
    unsigned char rank;
    unsigned char threshold;
    struct czl_mm_sp *next;
    struct czl_mm_sp *last;
    struct czl_mm_sp *freeNext;
    struct czl_mm_sp *freeLast;
    unsigned char heap[4];
} czl_mm_sp;

typedef struct czl_mm_sp_pool
{
    unsigned char type; //czl_mm_sp_type_enum
    unsigned char num;
    unsigned short min;
    unsigned short max;
    void **obj;
    czl_mm_sp *head;
    czl_mm_sp *freeHead;
    czl_ulong cnt, sum;
} czl_mm_sp_pool;
///////////////////////////////////////////////////////////////
#define CZL_MM_SH_HEAD_SIZE 16
#define CZL_MM_SYS_HEAP_GET(heap) ((czl_mm_sys_heap*)(heap-CZL_MM_SH_HEAD_SIZE))

typedef struct czl_mm_sys_heap
{
    struct czl_mm_sys_heap *next;
    struct czl_mm_sys_heap *last;
    unsigned long size;
    unsigned char null[2]; //保留
    short flag; //必须与 czl_mm_cache 的 index 内存对其
} czl_mm_sys_heap;
///////////////////////////////////////////////////////////////
void czl_mm_pool_init(czl_mm_sp_pool*, unsigned short, unsigned char);
#endif //#ifdef CZL_MM_MODULE

#endif //CZL_MM_H

#ifndef CZL_MM_H
#define CZL_MM_H

#define CZL_SYSTEM_WINDOWS  //运行在windows系统宏
//#define CZL_SYSTEM_LINUX    //运行在linux系统宏

#define CZL_SYSTEM_64bit    //64位支持宏
#define CZL_MULT_THREAD     //多线程支持宏

#define CZL_MM_MODULE       //内存管理模块宏
#define CZL_CONSOLE         //控制台运行方式宏，关了就运行在扩展方式
//#define CZL_TIMER           //线程异步定时器宏

//#define CZL_DEBUG           //调试模式

///////////////////////////////////////////////////////////////
#ifdef CZL_SYSTEM_WINDOWS
    //#define CZL_VC6
    #include <windows.h>
    #ifdef CZL_SYSTEM_64bit
        typedef __int64 czl_long;
        typedef unsigned __int64 czl_ulong;
    #else
        typedef long czl_long;
        typedef unsigned long czl_ulong;
    #endif
#elif defined CZL_SYSTEM_LINUX
    #include <unistd.h>
    #ifdef CZL_MULT_THREAD
        #include <pthread.h>
        #include <semaphore.h>
    #endif
    #ifdef CZL_SYSTEM_64bit
        typedef long long czl_long;
        typedef unsigned long long czl_ulong;
    #else
        typedef long czl_long;
        typedef unsigned long czl_ulong;
    #endif
#else
    #undef CZL_SYSTEM_64bit
    #undef CZL_MULT_THREAD
    typedef long czl_long;
    typedef unsigned long czl_ulong;
#endif
///////////////////////////////////////////////////////////////
#define CZL_MM_2GB   2*1024*1024*1024UL
#define CZL_MM_50MB  50*1024*1024UL

#ifdef CZL_MM_MODULE

//#define CZL_MM_SLAB

#ifdef CZL_MM_SLAB
#define CZL_MM_SLAB_RANGE   8188 //1048576 //160kb
#define CZL_MM_SLAB_GAP     128
//由 CZL_MM_SP_512B+1 + n*CZL_MM_SLAB_GAP >= 1024*1024 推出 n >= 8188
#define CZL_MM_1MB  1024*1024

typedef struct czl_mm_heap
{
    char color;
    unsigned short index;
    struct czl_mm_heap *next;
    struct czl_mm_heap *last;
    struct czl_mm_heap *hash_next;
    char heap[4];
} czl_mm_heap;

typedef struct czl_mm_slab
{
    czl_mm_heap *head;
    czl_mm_heap *tail;
    unsigned long size;
    unsigned long freq;
    unsigned long count;
} czl_mm_slab;

typedef struct czl_mm_hash
{
    czl_mm_heap **datas;    //bucket数组
    long mask;              //-size
    unsigned long size;     //bucket个数: 2的幂
    unsigned long count;    //总数据节点个数: 与size比较进行扩容和回收
} czl_mm_hash;

typedef struct czl_mm_slab_pool
{
    czl_mm_hash gc;
    czl_mm_slab *array;
    unsigned long count;
} czl_mm_slab_pool;


#endif //#ifdef CZL_MM_SLAB
///////////////////////////////////////////////////////////////
#define CZL_MM_HEAP_MSG_SIZE 5 //1B工作状态 + 2B大小 + 2B偏移量
#define CZL_MM_HEAP_MIN_SIZE (CZL_MM_HEAP_MSG_SIZE + 2)
#define CZL_MM_SP_HEAD_SIZE 28

#define CZL_MM_SP_RANGE 3
#define CZL_MM_SP_128B  128
#define CZL_MM_SP_256B  256
#define CZL_MM_SP_512B  512
#define CZL_MM_SP_4KB   (4*1024 - CZL_MM_SP_HEAD_SIZE)
#define CZL_MM_SP_20KB  (20*1024 - CZL_MM_SP_HEAD_SIZE)
#define CZL_MM_SP_40KB  (40*1024 - CZL_MM_SP_HEAD_SIZE) //不能大于65535

#define CZL_MM_SP_CHECKSUM 0x9e3c7d2b
#define CZL_MM_SP_ERROR(page) \
(page->id > 2 || page->checksum != CZL_MM_SP_CHECKSUM)

typedef enum czl_mm_color_enum
{
    CZL_MM_COLOR_WHITE,
    CZL_MM_COLOR_BLACK,
    CZL_MM_COLOR_RED
} czl_mm_color_enum;

typedef struct czl_mm_sp
{
    unsigned char id;
    unsigned char state;
    unsigned short rest;
    unsigned short piece;
    unsigned char null[2];
    unsigned long checksum;
    struct czl_mm_sp *next;
    struct czl_mm_sp *last;
    struct czl_mm_sp *Next;
    struct czl_mm_sp *Last;
    char heap[4];
} czl_mm_sp;

typedef struct czl_mm_sp_pool
{
    czl_mm_sp *head;
    czl_mm_sp *Head;
    czl_mm_sp *at;
    char *addr;
    unsigned short size;
    unsigned short len;
    czl_ulong rest;
    czl_ulong count;
} czl_mm_sp_pool;
///////////////////////////////////////////////////////////////
void czl_mm_pool_init(czl_mm_sp_pool*, unsigned short);
#endif //#ifdef CZL_MM_MODULE

#endif //CZL_MM_H

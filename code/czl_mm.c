#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "czl_mm.h"
#include "czl_vm.h"

#ifdef CZL_MM_MODULE
//非定长小堆内存(1~515B)和定长堆内存采用页管理方式；
    //非定长小堆内存将515B划分为40个范围等级，每个等级定长分配；
    //数据结构： 链表分配，链表回收；
    //算法： 页间和页内均采用链表组织，空闲链表，复制对其；
    //特点： 分配和回收时间复杂度固定为O(1)，内存利用率定长100%、非定长统计平均95%，外部碎片自动整理；
    //说明： 内存不常驻，任意页完全空闲立即释放交还系统；
//中等大小堆内存(516B~1MB)采用cache机制；
    //数据结构： 链表分配，链表回收；
    //算法： 空闲节点在链表头部，繁忙节点在链表尾部，范围分级；
    //特点： 分配和回收时间复杂度固定为O(1)，内存利用率统计平均大于90%；
    //说明： 内存常驻，cache大小可配置
//大块内存(>1MB)暂时由系统管理。
//////////////////////////////////////////////////////////////////
static void* czl_mm_sys_alloc(czl_gp*, czl_ulong);
static void czl_mm_sys_free(czl_gp*, char*);
static void* czl_mm_diff_page_resize(czl_gp*, char*, czl_ulong,
                                     czl_ulong, czl_mm_sp_pool*);
//////////////////////////////////////////////////////////////////
#ifdef CZL_MM_CACHE
static czl_mm_cache* czl_mm_cache_create
(
    czl_gp *gp,
    unsigned short index,
    czl_mm_cache_pool *pool
)
{
    czl_mm_cache *p = malloc(CZL_MM_CACHE_HEAD_SIZE+pool->size);
    if (!p)
        return NULL;

    p->state = 1;
    p->index = index;

    p->next = NULL;
    if (NULL == pool->head)
    {
        pool->head = p;
        p->last = NULL;
    }
    else
    {
        pool->tail->next = p;
        p->last = pool->tail;
    }
    pool->tail = p;

    gp->mm_cache.count += (CZL_MM_CACHE_HEAD_SIZE+pool->size);
    gp->mm_cnt += (CZL_MM_CACHE_HEAD_SIZE+pool->size);

    return p;
}

char czl_mm_cache_pools_init(czl_gp *gp)
{
	int i;

    gp->mm_cache.pools = calloc(CZL_MM_CACHE_RANGE, sizeof(czl_mm_cache_pool));
    if (!gp->mm_cache.pools)
        return 0;

    for (i = 0; i < CZL_MM_CACHE_RANGE; i++)
    {
        gp->mm_cache.pools[i].head = NULL;
        gp->mm_cache.pools[i].tail = NULL;
        gp->mm_cache.pools[i].size = CZL_MM_SP_515B + (i+1)*CZL_MM_CACHE_GAP;
    }

    gp->mm_cnt += (CZL_MM_CACHE_RANGE*sizeof(czl_mm_cache_pool));
    gp->mm_cache.flag = 1;

    return 1;
}

void czl_mm_cache_pools_destroy(czl_gp *gp, czl_mm_cache_handle *handle)
{
	int i;
    czl_mm_cache *p, *q;
    czl_mm_cache_pool *a = handle->pools;

    if (!a)
        return;

    for (i = 0; i < CZL_MM_CACHE_RANGE; i++)
    {
        for (p = a[i].head; p; p = q)
        {
            q = p->next;
            free(p);
            gp->mm_cnt -= (CZL_MM_CACHE_HEAD_SIZE+a[i].size);
        }
    }

    free(handle->pools);
    gp->mm_cnt -= (CZL_MM_CACHE_RANGE*sizeof(czl_mm_cache_pool));
}

static int czl_mm_cache_index(czl_gp *gp, unsigned long size)
{
    czl_mm_cache_pool *pool;
    int l = 0, h = CZL_MM_CACHE_RANGE-1, m;

    pool = gp->mm_cache.pools;
    do {
        m = (l+h)/2;
        if (size <= pool[m].size - CZL_MM_CACHE_GAP)
            h = m-1;
        else if (size > pool[m].size)
            l = m+1;
        else
            return m;
    } while (l <= h);

    return -1;
}

static void* czl_mm_cache_alloc(czl_gp *gp, unsigned long size)
{
    int inx;
    czl_mm_cache *node;
    czl_mm_cache_pool *pool;

    if (!gp->mm_cache.flag && !czl_mm_cache_pools_init(gp))
        return NULL;

    inx = czl_mm_cache_index(gp, size);
    pool = gp->mm_cache.pools + inx;
    node = pool->head;
    if (node && !node->state)
    {
        node->state = 1;
        if (node->next)
        {
            node->next->last = NULL;
            pool->head = node->next;
            node->next = NULL;
            node->last = pool->tail;
            pool->tail->next = node;
            pool->tail = node;
        }
    }
    else
    {
        if (gp->mm_cache_size < gp->mm_cache.count + CZL_MM_CACHE_HEAD_SIZE + size)
            return czl_mm_sys_alloc(gp, size);
        if (!(node=czl_mm_cache_create(gp, inx, pool)))
            return NULL;
    }

    return node->heap;
}

static char czl_mm_cache_scan(czl_gp *gp, unsigned long size)
{
    int inx;
    czl_mm_cache *node;
    czl_mm_cache_pool *pool;

    if (size > CZL_MM_1MB || !gp->mm_cache.count)
        return 0;

    inx = czl_mm_cache_index(gp, size);
    do {
        pool = gp->mm_cache.pools + inx;
        if (CZL_MM_CACHE_HEAD_SIZE+pool->size >= size)
        {
            node = pool->head;
            if (node && !node->state)
            {
                pool->head = node->next;
                if (pool->head)
                    pool->head->last = NULL;
                else
                    pool->tail = NULL;
                free(node);
                gp->mm_cache.count -= (CZL_MM_CACHE_HEAD_SIZE+pool->size);
                gp->mm_cnt -= (CZL_MM_CACHE_HEAD_SIZE+pool->size);
                return 1;
            }
        }
    } while(++inx < CZL_MM_CACHE_RANGE);

    return 0;
}

static void czl_mm_cache_gc(czl_gp *gp, unsigned long size)
{
    void *buf = NULL;
    unsigned long i;
    czl_mm_cache *node;
    czl_mm_cache_pool *pool;
    czl_ulong cnt = gp->mm_cnt;

    if (!gp->mm_cache.count)
        return;

    for (i = 0; i < CZL_MM_CACHE_RANGE; ++i)
    {
        pool = gp->mm_cache.pools + i;
        for (node = pool->head; node && !node->state; node = pool->head)
        {
            pool->head = node->next;
            if (pool->head)
                pool->head->last = NULL;
            else
                pool->tail = NULL;
            free(node);
            gp->mm_cache.count -= (CZL_MM_CACHE_HEAD_SIZE+pool->size);
            gp->mm_cnt -= (CZL_MM_CACHE_HEAD_SIZE+pool->size);
            if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
                goto CZL_END;
        }
    }

CZL_END:
    free(buf);
}

static void* czl_mm_cache_resize
(
    czl_gp *gp,
    char *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pool
)
{
	unsigned long size;
    czl_mm_cache *node;
    char *tmp = (CZL_MM_OBJ_SP == pool->type ? *((char**)heap) : heap);

    if (-1 == *((short*)(tmp-2)))
        return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pool);

    node = (czl_mm_cache*)CZL_MM_CACHE_GET(tmp);
    size = CZL_MM_SP_515B + node->index*CZL_MM_CACHE_GAP;
    if (new_size > size && new_size <= size+CZL_MM_CACHE_GAP)
        return heap;
    else
        return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pool);
}

static void czl_mm_cache_free(czl_gp *gp, char *heap)
{
    czl_mm_cache *node;
    czl_mm_cache_pool *pool;

    if (-1 == *((short*)(heap-2)))
    {
        czl_mm_sys_free(gp, heap);
        return;
    }

    node = (czl_mm_cache*)CZL_MM_CACHE_GET(heap);
    node->state = 0;
    pool = gp->mm_cache.pools + node->index;
    if (node->last && node->last->state)
    {
        node->last->next = node->next;
        if (node->next)
            node->next->last = node->last;
        else
            pool->tail = node->last;
        node->last = NULL;
        node->next = pool->head;
        pool->head = node;
    }
}
#endif //#ifdef CZL_MM_CACHE
//////////////////////////////////////////////////////////////////
static void czl_mm_free(czl_gp*, char*, czl_ulong, czl_mm_sp_pool*);
static void czl_mm_free_heap(czl_gp*, char*, czl_mm_sp_pool*);
static void* czl_mm_malloc_heap(czl_gp*, czl_mm_sp_pool*);
//////////////////////////////////////////////////////////////////
static void* czl_mm_sp_create
(
    czl_gp *gp,
    czl_mm_sp_pool *pool
)
{
    czl_mm_sp *p;
    char **obj = NULL;
    unsigned char rank = 0;
    unsigned long pageSize;

CZL_AGAIN:
    switch (pool->type)
    {
    case CZL_MM_OBJ_SP:
        pageSize = CZL_MM_OBJ_SP_SIZE(pool->max, pool->num);
        break;
    case CZL_MM_VAR_SP:
        pageSize = CZL_MM_VAR_SP_SIZE(pool->max, pool->num);
        break;
    default: //CZL_MM_BUF_SP
        pageSize = CZL_MM_BUF_SP_SIZE(pool->max, pool->num);
        break;
    }
    if (gp->mm_cnt + pageSize > gp->mm_limit || !(p=(czl_mm_sp*)malloc(pageSize)))
    {
        if (pool->num/2 == 0)
        {
            pool->num = CZL_MM_SP_HEAP_NUM_MIN;
            return NULL;
        }
        pool->num /= 2;
        goto CZL_AGAIN;
    }

    if (CZL_MM_OBJ_SP == pool->type &&
        !(obj=(char**)czl_mm_malloc_heap(gp, &gp->mmp_obj)))
    {
        free(p);
        return NULL;
    }

    gp->mm_cnt += pageSize;

    if (pool->type != CZL_MM_BUF_SP)
    {
        rank = ((gp->mmp_selfAdapt && pool->cnt) ? pool->sum/pool->cnt : gp->mmp_rank);
        ++pool->cnt;
        pool->sum += rank;
    }

    p->heapNum = pool->num;
    p->freeNum = 0;
    p->rank = rank;
    p->threshold = CZL_MM_SP_THRESHOLD(rank, pool->num);
    p->useHead = 1;
    p->freeHead = 0;
    p->restHead = (pool->num > 1 ? 2 : 0);

    p->last = NULL;
    p->next = pool->head;
    if (pool->head)
        pool->head->last = p;
    pool->head = p;

    if (pool->num+1 <= CZL_MM_SP_HEAP_NUM_MAX) //每页heap上限个数
        ++pool->num;

    if (CZL_MM_BUF_SP == pool->type)
    {
        p->heap[0] = 1; //current id
        return p->heap+CZL_MM_BUF_HEAP_MSG_SIZE;
    }
    else
    {
        p->heap[0] = 0; //   last id
        p->heap[1] = 1; //current id
        p->heap[2] = 0; //   next id
        if (CZL_MM_VAR_SP == pool->type)
            return p->heap+CZL_MM_VAR_HEAP_MSG_SIZE;
        else
        {
            char *heap = p->heap;
            *obj = heap+CZL_MM_OBJ_HEAP_MSG_SIZE;
            *((char***)(heap+3)) = obj;
            return obj;
        }
    }
}

static void czl_mm_sp_delete
(
    czl_gp *gp,
    czl_mm_sp_pool *pool,
    czl_mm_sp *node
)
{
    if (CZL_MM_BUF_SP == pool->type)
        gp->mm_cnt -= CZL_MM_BUF_SP_SIZE(pool->max, node->heapNum);
    else
    {
        --pool->cnt;
        pool->sum -= node->rank;
        if (CZL_MM_OBJ_SP == pool->type)
            gp->mm_cnt -= CZL_MM_OBJ_SP_SIZE(pool->max, node->heapNum);
        else
            gp->mm_cnt -= CZL_MM_VAR_SP_SIZE(pool->max, node->heapNum);
    }

    if (node->last)
        node->last->next = node->next;
    else
        pool->head = node->next;
    if (node->next)
        node->next->last = node->last;
    free(node);
}

static void czl_mm_sp_insert
(
    czl_mm_sp_pool *pool,
    czl_mm_sp *node
)
{
    node->freeLast = NULL;
    node->freeNext = pool->freeHead;
    if (pool->freeHead)
        pool->freeHead->freeLast = node;
    pool->freeHead = node;
}

static void czl_mm_sp_break
(
    czl_mm_sp_pool *pool,
    czl_mm_sp *node
)
{
    if (node->freeLast)
        node->freeLast->freeNext = node->freeNext;
    else
        pool->freeHead = node->freeNext;
    if (node->freeNext)
        node->freeNext->freeLast = node->freeLast;
}
//////////////////////////////////////////////////////////////////
static void czl_mm_sys_free(czl_gp *gp, char *heap)
{
    czl_mm_sys_heap *node;

#ifdef CZL_MM_CACHE
    if (*((short*)(heap-2)) != -1)
    {
        czl_mm_cache_free(gp, heap);
        return;
    }
#endif //#ifdef CZL_MM_CACHE

    node = CZL_MM_SYS_HEAP_GET(heap);

    gp->mm_cnt -= node->size;

    if (node->last)
        node->last->next = node->next;
    else
        gp->mmp_sh_head = node->next;
    if (node->next)
        node->next->last = node->last;
    free(node);
}

static void* czl_mm_sys_alloc(czl_gp *gp, czl_ulong size)
{
    char *buf;
    czl_mm_sys_heap *p;

    size += CZL_MM_SH_HEAD_SIZE;

    if (gp->mm_cnt+size > gp->mm_limit || !(buf=malloc(size)))
        return NULL;

    p = (czl_mm_sys_heap*)buf;
    p->last = NULL;
    p->next = gp->mmp_sh_head;
    if (gp->mmp_sh_head)
        gp->mmp_sh_head->last = p;
    gp->mmp_sh_head = p;

    p->flag = -1; //必须小于0
    p->size = size;

    gp->mm_cnt += size;

    return buf+CZL_MM_SH_HEAD_SIZE;
}

static void* czl_mm_sys_realloc
(
    czl_gp *gp,
    char *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pool
)
{
    char *buf, *old;
    czl_mm_sys_heap *p;

    if (new_size-old_size+gp->mm_cnt > gp->mm_limit)
        return NULL;

    old = (CZL_MM_OBJ_SP == pool->type ? *((char**)heap) : heap);
    if (*((short*)(old-2)) != -1)
        return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pool);

    old -= CZL_MM_SH_HEAD_SIZE;
    if (!(buf=realloc(old, CZL_MM_SH_HEAD_SIZE+new_size)))
        return NULL;

    gp->mm_cnt += (new_size-old_size);

    p = (czl_mm_sys_heap*)buf;
    p->size = CZL_MM_SH_HEAD_SIZE+new_size;

    if (buf == old)
        return heap;

    if (p->last)
        p->last->next = p;
    else
        gp->mmp_sh_head = p;
    if (p->next)
        p->next->last = p;

    p->flag = -1;

    if (CZL_MM_OBJ_SP == pool->type)
    {
        *((char**)heap) = buf+CZL_MM_SH_HEAD_SIZE;
        return heap;
    }
    else
        return buf+CZL_MM_SH_HEAD_SIZE;
}
//////////////////////////////////////////////////////////////////
static unsigned char czl_mm_sp_get(unsigned short size, czl_mm_sp_pool *pool)
{
    if (size <= pool[19].max)
        if (size <= pool[9].max)
            if (size <= pool[4].max)
                if (size <= pool[2].max)
                    if (size <= pool[1].max)
                        if (size <= pool[0].max) return 0;
                        else return 1;
                    else return 2;
                else
                    if (size <= pool[3].max) return 3;
                    else return 4;
            else
                if (size <= pool[7].max)
                    if (size <= pool[6].max)
                        if (size <= pool[5].max) return 5;
                        else return 6;
                    else return 7;
                else
                    if (size <= pool[8].max) return 8;
                    else return 9;
        else
            if (size <= pool[14].max)
                if (size <= pool[12].max)
                    if (size <= pool[11].max)
                        if (size <= pool[10].max) return 10;
                        else return 11;
                    else return 12;
                else
                    if (size <= pool[13].max) return 13;
                    else return 14;
            else
                if (size <= pool[17].max)
                    if (size <= pool[16].max)
                        if (size <= pool[15].max) return 15;
                        else return 16;
                    else return 17;
                else
                    if (size <= pool[18].max) return 18;
                    else return 19;
    else
        if (size <= pool[29].max)
            if (size <= pool[24].max)
                if (size <= pool[22].max)
                    if (size <= pool[21].max)
                        if (size <= pool[20].max) return 20;
                        else return 21;
                    else return 22;
                else
                    if (size <= pool[23].max) return 23;
                    else return 24;
            else
                if (size <= pool[27].max)
                    if (size <= pool[26].max)
                        if (size <= pool[25].max) return 25;
                        else return 26;
                    else return 27;
                else
                    if (size <= pool[28].max) return 28;
                    else return 29;
        else
            if (size <= pool[34].max)
                if (size <= pool[32].max)
                    if (size <= pool[31].max)
                        if (size <= pool[30].max) return 30;
                        else return 31;
                    else return 32;
                else
                    if (size <= pool[33].max) return 33;
                    else return 34;
            else
                if (size <= pool[37].max)
                    if (size <= pool[36].max)
                        if (size <= pool[35].max) return 35;
                        else return 36;
                    else return 37;
                else
                    if (size <= pool[38].max) return 38;
                    else return 39;
}

static void* czl_mm_malloc_heap(czl_gp *gp, czl_mm_sp_pool *pool)
{
    if (pool->head)
    {
        if (pool->head->restHead)
        {
            czl_mm_sp *page = pool->head;
            if (CZL_MM_BUF_SP == pool->type)
            {
                char *heap = CZL_MM_BUF_HEAP_GET(page->heap, page->restHead, pool->max);
                //将新节点插入到使用链表
                ++page->useHead;
                heap[0] = page->restHead;
                if (page->restHead++ == page->heapNum)
                    page->restHead = 0;
                return heap+CZL_MM_BUF_HEAP_MSG_SIZE;
            }
            else
            {
                char *heap;
                char *useHead;
                char **obj = NULL;
                if (CZL_MM_OBJ_SP == pool->type)
                {
                    if (!(obj=(char**)czl_mm_malloc_heap(gp, &gp->mmp_obj)))
                        return NULL;
                    heap = CZL_MM_OBJ_HEAP_GET(page->heap, page->restHead, pool->max);
                    useHead = heap - CZL_MM_OBJ_HEAP_MSG_SIZE - pool->max;
                    *obj = heap+CZL_MM_OBJ_HEAP_MSG_SIZE;
                    *((char***)(heap+3)) = obj;
                }
                else
                {
                    heap = CZL_MM_VAR_HEAP_GET(page->heap, page->restHead, pool->max);
                    useHead = heap - CZL_MM_VAR_HEAP_MSG_SIZE - pool->max;
                }
                //将新节点插入到使用链表
                heap[0] = 0;
                heap[1] = page->restHead;
                heap[2] = page->useHead;
                page->useHead = useHead[0] = heap[1];
                if (page->restHead++ == page->heapNum)
                    page->restHead = 0;
                if (CZL_MM_OBJ_SP == pool->type)
                    return obj;
                else
                    return heap+CZL_MM_VAR_HEAP_MSG_SIZE;
            }
        }
        else if (pool->freeHead)
        {
            czl_mm_sp *page = pool->freeHead;
            if (CZL_MM_BUF_SP == pool->type)
            {
                char *heap = CZL_MM_BUF_HEAP_GET(page->heap, page->freeHead, pool->max);
                ++page->useHead;
                //从空闲链表中删除该节点
                page->freeHead = heap[1];
                if (!page->freeHead)
                    czl_mm_sp_break(pool, page);
                return heap+CZL_MM_BUF_HEAP_MSG_SIZE;
            }
            else
            {
                char *heap;
                char *useHead;
                char **obj = NULL;
                --page->freeNum;
                if (CZL_MM_OBJ_SP == pool->type)
                {
                    if (!(obj=(char**)czl_mm_malloc_heap(gp, &gp->mmp_obj)))
                        return NULL;
                    heap = CZL_MM_OBJ_HEAP_GET(page->heap, page->freeHead, pool->max);
                    useHead = CZL_MM_OBJ_HEAP_GET(page->heap, page->useHead, pool->max);
                    *obj = heap+CZL_MM_OBJ_HEAP_MSG_SIZE;
                    *((char***)(heap+3)) = obj;
                }
                else
                {
                    heap = CZL_MM_VAR_HEAP_GET(page->heap, page->freeHead, pool->max);
                    useHead = CZL_MM_VAR_HEAP_GET(page->heap, page->useHead, pool->max);
                }
                //从空闲链表中删除该节点
                page->freeHead = heap[2];
                if (!page->freeHead)
                    czl_mm_sp_break(pool, page);
                //将该节点插入到使用链表
                heap[0] = 0;
                heap[2] = page->useHead;
                if (page->useHead)
                    useHead[0] = heap[1];
                page->useHead = heap[1];
                if (CZL_MM_OBJ_SP == pool->type)
                    return obj;
                else
                    return heap+CZL_MM_VAR_HEAP_MSG_SIZE;
            }
        }
    }

    return czl_mm_sp_create(gp, pool);
}

static void* czl_mm_malloc(czl_gp *gp, czl_ulong size, czl_mm_sp_pool *pool)
{
    if (pool->min != pool->max)
    {
        if (0 == size)
            return NULL;
        else if (size <= CZL_MM_SP_515B)
            pool += czl_mm_sp_get(size, pool);
        else
        {
            char *heap;
            char **obj = NULL;
            if (CZL_MM_OBJ_SP == pool->type &&
                !(obj=(char**)czl_mm_malloc_heap(gp, &gp->mmp_obj)))
                return NULL;
        #ifdef CZL_MM_CACHE
            if (gp->mm_cache_size && size <= CZL_MM_1MB)
                heap = czl_mm_cache_alloc(gp, size);
            else
        #endif //#ifdef CZL_MM_CACHE
                heap = czl_mm_sys_alloc(gp, size);
            if (!heap)
            {
                if (CZL_MM_OBJ_SP == pool->type)
                    czl_mm_free_heap(gp, (char*)obj, &gp->mmp_obj);
                return NULL;
            }
            if (CZL_MM_OBJ_SP == pool->type)
            {
                *obj = heap;
                return obj;
            }
            else
                return heap;
        }
    }

    return czl_mm_malloc_heap(gp, pool);
}

static void* czl_mm_calloc
(
    czl_gp *gp,
    czl_ulong num,
    czl_ulong size,
    czl_mm_sp_pool *pool
)
{
    void *heap = czl_mm_malloc(gp, (size*=num), pool);
    if (!heap)
        return NULL;

    if (CZL_MM_OBJ_SP == pool->type)
    {
        void *tmp = *((char**)heap);
        memset(tmp, 0, size);
    }
    else
        memset(heap, 0, size);

    return heap;
}

static void* czl_mm_diff_page_resize
(
    czl_gp *gp,
    char *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pool
)
{
    void *buf = czl_mm_malloc(gp, new_size, pool);
    if (!buf)
        return NULL;
    if (CZL_MM_OBJ_SP == pool->type)
        memcpy(*((char**)buf), *((char**)heap),
               (old_size < new_size ? old_size : new_size));
    else
        memcpy(buf, heap, (old_size < new_size ? old_size : new_size));
    czl_mm_free(gp, heap, old_size, pool);
    return buf;
}

static void* czl_mm_realloc
(
    czl_gp *gp,
    char *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pools
)
{
    if (!heap || 0 == old_size)
        return czl_mm_malloc(gp, new_size, pools);
    if (new_size == old_size)
        return heap;

    if (new_size <= CZL_MM_SP_515B && old_size <= CZL_MM_SP_515B)
    {
        czl_mm_sp_pool *pool = pools + czl_mm_sp_get(old_size, pools);
        if (new_size >= pool->min && new_size <= pool->max)
            return heap;
        else
            return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pools);
    }
    else
    {
#ifdef CZL_MM_CACHE
        if (gp->mm_cache_size && new_size <= CZL_MM_1MB)
            return czl_mm_cache_resize(gp, heap, new_size, old_size, pools);
        else if (old_size > CZL_MM_1MB && new_size > CZL_MM_1MB)
            return czl_mm_sys_realloc(gp, heap, new_size, old_size, pools);
        else
            return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pools);
#else
        if (new_size > CZL_MM_SP_515B && old_size > CZL_MM_SP_515B)
            return czl_mm_sys_realloc(gp, heap, new_size, old_size, pools);
        else
            return czl_mm_diff_page_resize(gp, heap, new_size, old_size, pools);
#endif //#ifdef CZL_MM_CACHE
    }
}

static void czl_mm_update_tabkv
(
    char *heap,
    char *tmp,
    czl_mm_sp_pool *pool
)
{
    czl_table *tab = CZL_TAB(pool->obj);
    czl_tabkv *kv = (czl_tabkv*)heap;

    if ((czl_tabkv*)tmp == tab->foreach_inx)
        tab->foreach_inx = kv;
    if (CZL_VAR_EXIST_REF(kv))
        czl_ref_obj_update((czl_var*)kv);

    if (kv->last)
        kv->last->next = kv;
    else
        tab->eles_head = kv;
    if (kv->next)
        kv->next->last = kv;
    else
        tab->eles_tail = kv;

    if ((czl_tabkv**)kv->clsLast >= tab->buckets &&
        (czl_tabkv**)kv->clsLast <= tab->buckets+tab->size)
        *((czl_tabkv**)kv->clsLast) = kv;
    else
        kv->clsLast->clsNext = kv;

    if (kv->clsNext)
        kv->clsNext->clsLast = kv;
}

#define CZL_MM_CMP(selfAdapt, size, pool, p, page, cnt) { \
    if (p->freeNum > page->freeNum) { page = p; goto CZL_AGAIN; } \
    if (selfAdapt) { \
        if (size) { \
            if (p->rank > 1) { \
                --pool->sum; \
                p->threshold = CZL_MM_SP_THRESHOLD(--p->rank, p->heapNum); \
            } \
        } \
        else if (p->rank < CZL_MM_SP_HEAP_NUM_MAX) { \
            ++pool->sum; \
            p->threshold = CZL_MM_SP_THRESHOLD(++p->rank, p->heapNum); \
        } \
    } \
    cnt += p->freeNum; \
}

static char czl_mm_fill_page
(
    czl_gp *gp,
    czl_mm_sp *page,
    czl_mm_sp_pool *pool,
    czl_ulong size
)
{
    char ret = 1;
    char *tmp, *heap;
    czl_mm_sp *p, *q;
    unsigned long cnt, sum;

    if (!page) return 0;

    if (page->heapNum == page->freeNum)
        goto CZL_FREE;

CZL_AGAIN:
    p = page->freeNext;
    q = page->freeLast;
    cnt = 0;
    sum = page->heapNum - page->freeNum;
    while (p && cnt < sum)
    {
        CZL_MM_CMP(gp->mmp_selfAdapt, size, pool, p, page, cnt);
        p = p->freeNext;
    }
    while (q && cnt < sum)
    {
        CZL_MM_CMP(gp->mmp_selfAdapt, size, pool, q, page, cnt);
        q = q->freeLast;
    }
    if (cnt < sum)
        return 0;

    if (page->freeNext) { p = page->freeNext; q = NULL; }
    else                { p = page->freeLast; q = page; }

    for (;;)
    {
        --p->freeNum;
        if (CZL_MM_OBJ_SP == pool->type)
        {
            tmp = CZL_MM_OBJ_HEAP_GET(p->heap, p->freeHead, pool->max);
            heap = CZL_MM_OBJ_HEAP_GET(p->heap, p->useHead, pool->max);
        }
        else
        {
            tmp = CZL_MM_VAR_HEAP_GET(p->heap, p->freeHead, pool->max);
            heap = CZL_MM_VAR_HEAP_GET(p->heap, p->useHead, pool->max);
        }
        p->freeHead = tmp[2];
        tmp[0] = 0;
        tmp[2] = p->useHead;
        if (p->useHead)
             heap[0] = tmp[1];
        p->useHead = tmp[1];
        if (!p->freeHead)
        {
            czl_mm_sp_break(pool, p);
            if (q) p = p->freeLast;
            else if (!(p=p->freeNext)) p = q = page->freeLast;
        }
        //
        if (CZL_MM_OBJ_SP == pool->type)
        {
            char **obj;
            heap = CZL_MM_OBJ_HEAP_GET(page->heap, page->useHead, pool->max);
            page->useHead = heap[2];
            memcpy(tmp+3, heap+3, pool->max+4);
            obj = *((char***)(heap+3));
            *obj = tmp+CZL_MM_OBJ_HEAP_MSG_SIZE;
        }
        else
        {
            heap = CZL_MM_VAR_HEAP_GET(page->heap, page->useHead, pool->max);
            page->useHead = heap[2];
            tmp += CZL_MM_VAR_HEAP_MSG_SIZE;
            heap += CZL_MM_VAR_HEAP_MSG_SIZE;
            memcpy(tmp, heap, pool->max);
            czl_mm_update_tabkv(tmp, heap, pool);
        }
        if (!page->useHead)
        {
        CZL_FREE:
            if (size &&
                ((CZL_MM_OBJ_SP == pool->type &&
                  size > (czl_ulong)CZL_MM_OBJ_SP_SIZE(pool->max, page->heapNum)) ||
                 (CZL_MM_VAR_SP == pool->type &&
                  size > (czl_ulong)CZL_MM_VAR_SP_SIZE(pool->max, page->heapNum))))
                ret = 0;
            czl_mm_sp_break(pool, page);
            czl_mm_sp_delete(gp, pool, page);
            return ret;
        }
    }
}

static void czl_mm_free_ov
(
    czl_gp *gp,
    char *heap,
    czl_mm_sp *page,
    czl_mm_sp_pool *pool
)
{
    if (CZL_MM_OBJ_SP == pool->type)
    {
        heap -= 7;
        //从使用链表中删除该节点
        if (heap[0])
        {
            char *last = CZL_MM_OBJ_HEAP_GET(page->heap, heap[0], pool->max);
            last[2] = heap[2];
        }
        else if (!(page->useHead=heap[2]) && pool->freeHead && pool->freeHead->next)
        {
            if (page->freeHead)
                czl_mm_sp_break(pool, page);
            czl_mm_sp_delete(gp, pool, page);
            return;
        }
        if (heap[2])
        {
            char *next = CZL_MM_OBJ_HEAP_GET(page->heap, heap[2], pool->max);
            next[0] = heap[0];
        }
    }
    else
    {
        heap -= 3;
        //从使用链表中删除该节点
        if (heap[0])
        {
            char *last = CZL_MM_VAR_HEAP_GET(page->heap, heap[0], pool->max);
            last[2] = heap[2];
        }
        else if (!(page->useHead=heap[2]) && pool->freeHead && pool->freeHead->next)
        {
            if (page->freeHead)
                czl_mm_sp_break(pool, page);
            czl_mm_sp_delete(gp, pool, page);
            return;
        }
        if (heap[2])
        {
            char *next = CZL_MM_VAR_HEAP_GET(page->heap, heap[2], pool->max);
            next[0] = heap[0];
        }
    }

    //将该节点插入到空闲链表
    if (!page->freeHead)
        czl_mm_sp_insert(pool, page);
    heap[2] = page->freeHead;
    page->freeHead = heap[1];

    //碎片率检查
    ++page->freeNum;
    if (page->rank && page->freeNum > page->threshold && page != pool->head)
        czl_mm_fill_page(gp, page, pool, 0);
}

static void czl_mm_fill_heap
(
    czl_gp *gp,
    char *heap,
    czl_mm_sp *page,
    czl_mm_sp_pool *pool
)
{
    char *tmp;

    if (page == pool->freeHead || (!pool->freeHead && page == pool->head))
    {
        czl_mm_free_ov(gp, heap, page, pool);
        return;
    }

    page = (pool->freeHead ? pool->freeHead : pool->head);

    if (CZL_MM_OBJ_SP == pool->type)
    {
        char **obj;
        tmp = CZL_MM_OBJ_HEAP_GET(page->heap, page->useHead, pool->max) +
              CZL_MM_OBJ_HEAP_MSG_SIZE;
        memcpy(heap-4, tmp-4, pool->max+4);
        obj = *((char***)(tmp-4));
        *obj = heap;
    }
    else
    {
        tmp = CZL_MM_VAR_HEAP_GET(page->heap, page->useHead, pool->max) +
              CZL_MM_VAR_HEAP_MSG_SIZE;
        memcpy(heap, tmp, pool->max);
        czl_mm_update_tabkv(heap, tmp, pool);
    }

    czl_mm_free_ov(gp, tmp, page, pool);
}

static void czl_mm_free_buf
(
    czl_gp *gp,
    char *heap,
    czl_mm_sp_pool *pool
)
{
    czl_mm_sp *page = (czl_mm_sp*)CZL_MM_BUF_PAGE_GET(heap, pool->max);
    if (0 == --page->useHead && pool->freeHead && pool->freeHead->next)
    {
        if (page->freeHead)
            czl_mm_sp_break(pool, page);
        czl_mm_sp_delete(gp, pool, page);
        return;
    }

    //将该节点插入到空闲链表
    if (!page->freeHead)
        czl_mm_sp_insert(pool, page);
    *heap = page->freeHead;
    page->freeHead = *(heap-1);
}

static void czl_mm_free_heap
(
    czl_gp *gp,
    char *heap,
    czl_mm_sp_pool *pool
)
{
    if (CZL_MM_BUF_SP == pool->type)
        czl_mm_free_buf(gp, heap, pool);
    else
    {
        czl_mm_sp *page;
        if (CZL_MM_VAR_SP == pool->type)
            page = (czl_mm_sp*)CZL_MM_VAR_PAGE_GET(heap, pool->max);
        else
        {
            char *tmp = *((char**)heap);
            czl_mm_free_buf(gp, heap, &gp->mmp_obj);
            heap = tmp;
            page = (czl_mm_sp*)CZL_MM_OBJ_PAGE_GET(heap, pool->max);
        }
        if (page->rank)
            czl_mm_free_ov(gp, heap, page, pool);
        else
            czl_mm_fill_heap(gp, heap, page, pool);
    }
}

static void czl_mm_free
(
    czl_gp *gp,
    char *heap,
    czl_ulong size,
    czl_mm_sp_pool *pool
)
{
    if (pool->min != pool->max)
    {
        if (0 == size || !heap)
            return;
    #ifdef CZL_MM_CACHE
        if (gp->mm_cache_size && size > CZL_MM_SP_515B && size <= CZL_MM_1MB)
        {
            if (CZL_MM_OBJ_SP == pool->type)
            {
                char *tmp = *((char**)heap);
                czl_mm_free_buf(gp, heap, &gp->mmp_obj);
                heap = tmp;
            }
            czl_mm_cache_free(gp, heap);
            return;
        }
        else
    #endif //#ifdef CZL_MM_CACHE
        if (size > CZL_MM_SP_515B)
        {
            if (CZL_MM_OBJ_SP == pool->type)
            {
                char *tmp = *((char**)heap);
                czl_mm_free_buf(gp, heap, &gp->mmp_obj);
                heap = tmp;
            }
            czl_mm_sys_free(gp, heap);
            return;
        }
        pool += czl_mm_sp_get(size, pool);
    }

    czl_mm_free_heap(gp, heap, pool);
}
//////////////////////////////////////////////////////////////////
void czl_mm_sp_destroy(czl_gp *gp, czl_mm_sp_pool *pool)
{
    czl_mm_sp *p = pool->head, *q;

    switch (pool->type)
    {
    case CZL_MM_OBJ_SP:
        while (p)
        {
            gp->mm_cnt -= CZL_MM_OBJ_SP_SIZE(pool->max, p->heapNum);
            q = p->next;
            free(p);
            p = q;
        }
        break;
    case CZL_MM_VAR_SP:
        while (p)
        {
            gp->mm_cnt -= CZL_MM_VAR_SP_SIZE(pool->max, p->heapNum);
            q = p->next;
            free(p);
            p = q;
        }
        break;
    case CZL_MM_BUF_SP:
        while (p)
        {
            gp->mm_cnt -= CZL_MM_BUF_SP_SIZE(pool->max, p->heapNum);
            q = p->next;
            free(p);
            p = q;
        }
        break;
    default: //CZL_MM_UNDEF_SP
        break;
    }
}
//////////////////////////////////////////////////////////////////
static void czl_mm_sp_pool_init
(
    czl_mm_sp_pool *pool,
    unsigned char type,
    unsigned short min,
    unsigned short max
)
{
    pool->type = type;
    pool->num = CZL_MM_SP_HEAP_NUM_MIN;
    pool->min = min;
    pool->max = max;
    pool->head = pool->freeHead = NULL;
    pool->cnt = 0;
    pool->sum = 0;
}

void czl_mm_pool_init
(
    czl_mm_sp_pool *pool,
    unsigned short size,
    unsigned char type
)
{
#ifndef CZL_MM_RT_GC
    if (type != CZL_MM_BUF_SP)
        type = CZL_MM_BUF_SP;
#endif //#ifndef CZL_MM_RT_GC

    if (size > 0)
        czl_mm_sp_pool_init(pool, type, size, size);
    else
    {
        unsigned short min = 5;
        czl_mm_sp_pool_init(pool, type, 1, 2);
        czl_mm_sp_pool_init(++pool, type, 3, 4);
        do {
            unsigned short max = min + min/9;
            czl_mm_sp_pool_init(++pool, type, min, max);
            min = max+1;
        } while(min <= 512);
    }
}

void czl_mm_module_init
(
    czl_gp *gp,
    unsigned char rank,
    unsigned char selfAdapt,
    czl_ulong gc_size
#ifdef CZL_MM_CACHE
    , czl_ulong cache_size
#endif
)
{
    gp->mmp_rank = rank;
    gp->mmp_selfAdapt = selfAdapt;
    gp->mmp_gc_size = gc_size;

    czl_mm_pool_init(gp->mmp_tmp, 0, CZL_MM_BUF_SP);
    czl_mm_pool_init(gp->mmp_rt, 0, CZL_MM_BUF_SP);
    czl_mm_pool_init(gp->mmp_stack, 0, CZL_MM_BUF_SP);
    czl_mm_pool_init(&gp->mmp_obj, sizeof(void**), CZL_MM_BUF_SP);

    czl_mm_pool_init(gp->mmp_str, 0, CZL_MM_OBJ_SP);
    czl_mm_pool_init(&gp->mmp_tab, sizeof(czl_table), CZL_MM_OBJ_SP);
    czl_mm_pool_init(&gp->mmp_arr, sizeof(czl_array), CZL_MM_OBJ_SP);
    czl_mm_pool_init(&gp->mmp_sq, sizeof(czl_sq), CZL_MM_OBJ_SP);
    czl_mm_pool_init(&gp->mmp_cor, sizeof(czl_coroutine), CZL_MM_OBJ_SP);
    czl_mm_pool_init(&gp->mmp_file, sizeof(czl_file), CZL_MM_OBJ_SP);

    czl_mm_pool_init(&gp->mmp_ref, sizeof(czl_ref_var), CZL_MM_BUF_SP);
#ifdef CZL_MULT_THREAD
    czl_mm_pool_init(&gp->mmp_thread, sizeof(czl_thread), CZL_MM_BUF_SP);
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_MM_CACHE
    gp->mm_cache_size = cache_size;
    gp->mm_cache.pools = NULL;
    gp->mm_cache.count = 0;
    gp->mm_cache.flag = 0;
#endif //#ifdef CZL_MM_CACHE
}
//////////////////////////////////////////////////////////////////
static void czl_mm_scan_pool(czl_gp *gp, czl_ulong size)
{
    czl_class *c;
    czl_mm_sp *p;
    unsigned long i;
    unsigned char id;
    void *buf = NULL;
    czl_ulong cnt = gp->mm_cnt;

    for (c = gp->class_head; c; c = c->next)
        while (czl_mm_fill_page(gp, c->pool.freeHead, &c->pool, 0))
            if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
                goto CZL_END;

    for (i = 0; i < CZL_MM_SP_RANGE; ++i)
        while (czl_mm_fill_page(gp, gp->mmp_str[i].freeHead, gp->mmp_str+i, 0))
            if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
                goto CZL_END;

    while (czl_mm_fill_page(gp, gp->mmp_tab.freeHead, &gp->mmp_tab, 0))
        if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
            goto CZL_END;

    while (czl_mm_fill_page(gp, gp->mmp_sq.freeHead, &gp->mmp_sq, 0))
        if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
            goto CZL_END;

    while (czl_mm_fill_page(gp, gp->mmp_cor.freeHead, &gp->mmp_cor, 0))
        if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
            goto CZL_END;

    while (czl_mm_fill_page(gp, gp->mmp_file.freeHead, &gp->mmp_file, 0))
        if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
            goto CZL_END;

    while (czl_mm_fill_page(gp, gp->mmp_arr.freeHead, &gp->mmp_arr, 0))
        if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
            goto CZL_END;

    for (i = 0, p = gp->mmp_tab.head; p && i < CZL_MM_SP_HEAP_NUM_MAX; p = p->next) {
        char *tmp;
        for (id = p->useHead; id && i < CZL_MM_SP_HEAP_NUM_MAX; id = tmp[2], ++i) {
			czl_table *tab;
            tmp = CZL_MM_OBJ_HEAP_GET(p->heap, id, gp->mmp_tab.max);
            tab = (czl_table*)(tmp+CZL_MM_OBJ_HEAP_MSG_SIZE);
            while (czl_mm_fill_page(gp, tab->pool.freeHead, &tab->pool, 0))
                if ((cnt - gp->mm_cnt >= gp->mmp_gc_size) || (buf=malloc(size)))
                    goto CZL_END;
        }
    }

#ifdef CZL_MM_CACHE
    czl_mm_cache_gc(gp, size);
#endif //#ifdef CZL_MM_CACHE

CZL_END:
    free(buf);
}

static void czl_mm_scan_page(czl_gp *gp, czl_ulong size)
{
    czl_class *c;
    czl_mm_sp *p;
    unsigned long i;
    unsigned char id;

#ifndef CZL_MM_RT_GC
    return;
#endif //#ifndef CZL_MM_RT_GC

    size += 60; //60保证大于一个heap的头部信息

    for (c = gp->class_head; c; c = c->next)
        if (czl_mm_fill_page(gp, c->pool.freeHead, &c->pool, size))
            return;

    for (i = 0; i < CZL_MM_SP_RANGE; ++i)
        if (czl_mm_fill_page(gp, gp->mmp_str[i].freeHead, gp->mmp_str+i, size))
            return;

    if (czl_mm_fill_page(gp, gp->mmp_tab.freeHead, &gp->mmp_tab, size))
        return;

    if (czl_mm_fill_page(gp, gp->mmp_sq.freeHead, &gp->mmp_sq, size))
        return;

    if (czl_mm_fill_page(gp, gp->mmp_cor.freeHead, &gp->mmp_cor, size))
        return;

    if (czl_mm_fill_page(gp, gp->mmp_file.freeHead, &gp->mmp_file, size))
        return;

    if (czl_mm_fill_page(gp, gp->mmp_arr.freeHead, &gp->mmp_arr, size))
        return;

    for (i = 0, p = gp->mmp_tab.head; p && i < CZL_MM_SP_HEAP_NUM_MAX; p = p->next) {
        char *tmp;
        for (id = p->useHead; id && i < CZL_MM_SP_HEAP_NUM_MAX; id = tmp[2], ++i) {
			czl_table *tab;
            tmp = CZL_MM_OBJ_HEAP_GET(p->heap, id, gp->mmp_tab.max);
            tab = (czl_table*)(tmp+CZL_MM_OBJ_HEAP_MSG_SIZE);
            if (czl_mm_fill_page(gp, tab->pool.freeHead, &tab->pool, size))
                return;
        }
    }

#ifdef CZL_MM_CACHE
    if (czl_mm_cache_scan(gp, size))
        return;
#endif //#ifdef CZL_MM_CACHE

    czl_mm_scan_pool(gp, size);
}

czl_ulong czl_mm_gc(czl_gp *gp)
{
    czl_ulong cnt = gp->mm_cnt;
    czl_mm_scan_page(gp, CZL_MM_4GB-60);
    return cnt - gp->mm_cnt;
}
//////////////////////////////////////////////////////////////////
#endif //#ifdef CZL_MM_MODULE

void* czl_malloc
(
    czl_gp *gp,
    czl_ulong size
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool *pool
#endif
)
{
#ifdef CZL_MM_MODULE
    void *buf = czl_mm_malloc(gp, size, pool);
#else
    void *buf = NULL;
    if (size+gp->mm_cnt < gp->mm_limit && (buf=malloc(size)))
        gp->mm_cnt += size;
#endif
    if (!buf && size)
    {
    #ifdef CZL_MM_MODULE
        czl_mm_scan_page(gp, size);
        if (!(buf=czl_mm_malloc(gp, size, pool)))
    #endif
            gp->exceptionCode = CZL_EXCEPTION_OUT_OF_MEMORY;
    }
    else if (gp->mm_cnt > gp->mm_max)
        gp->mm_max = gp->mm_cnt;
    return buf;
}

void* czl_calloc
(
    czl_gp *gp,
    czl_ulong num,
    czl_ulong size
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool *pool
#endif
)
{
#ifdef CZL_MM_MODULE
    void *buf = czl_mm_calloc(gp, num, size, pool);
#else
    void *buf = NULL;
    if ((num*size)+gp->mm_cnt < gp->mm_limit && (buf=calloc(num, size)))
        gp->mm_cnt += (num*size);
#endif
    if (!buf && num)
    {
    #ifdef CZL_MM_MODULE
        czl_mm_scan_page(gp, num*size);
        if (!(buf=czl_mm_calloc(gp, num, size, pool)))
    #endif
            gp->exceptionCode = CZL_EXCEPTION_OUT_OF_MEMORY;
    }
    else if (gp->mm_cnt > gp->mm_max)
        gp->mm_max = gp->mm_cnt;
    return buf;
}

void* czl_realloc
(
    czl_gp *gp,
    void *buf,
    czl_ulong new_size,
    czl_ulong old_size
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool *pool
#endif
)
{
#ifdef CZL_MM_MODULE
    void *new_buf = czl_mm_realloc(gp, (char*)buf, new_size, old_size, pool);
#else
	void *new_buf = NULL;
    if (0 == new_size)
    {
        free(buf);
        gp->mm_cnt += new_size-old_size;
        return NULL;
    }
    if (new_size-old_size+gp->mm_cnt < gp->mm_limit &&
        (new_buf=realloc(buf, new_size)))
        gp->mm_cnt += new_size-old_size;
#endif
    if (!new_buf && new_size)
    {
    #ifdef CZL_MM_MODULE
        czl_mm_scan_page(gp, new_size);
        if (!(new_buf=czl_mm_realloc(gp, (char*)buf, new_size, old_size, pool)))
    #endif
            gp->exceptionCode = CZL_EXCEPTION_OUT_OF_MEMORY;
    }
    else if (gp->mm_cnt > gp->mm_max)
        gp->mm_max = gp->mm_cnt;
    return new_buf;
}

void czl_free
(
    czl_gp *gp,
    void *buf,
    czl_ulong size
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool *pool
#endif
)
{
#ifdef CZL_MM_MODULE
    czl_mm_free(gp, (char*)buf, size, pool);
#else
    if (buf)
    {
        free(buf);
        gp->mm_cnt -= size;
    }
#endif
}

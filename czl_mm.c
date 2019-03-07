#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "czl_mm.h"
#include "czl_vm.h"

#ifdef CZL_MM_MODULE
//碎片内存(1~512B)采用页管理方式；
    //一级: 1 ~ 128B ->  4KB/page very good
    //二级: 129~256B -> 20KB/page good
    //三级: 257~512B -> 40KB/page just so so
    //数据结构： 链表分配，链表回收；
    //算法： 分配节点和回收节点融合；
    //特点： 分配速度快，内存利用率高，频繁alloc/free稳定性一般；
    //内存不常驻，任意页完全空闲立即释放交还系统，页数量(limit)可配置；
//小块内存(513B~1MB)采用slab机制；
    //数据结构： 链表分配，HASH回收；
    //算法： 链表节点与HASH节点融合，着色标记；
    //特点： 分配速度快，内存利用率稍低，频繁alloc/free稳定性高；
    //内存常驻，cache大小可配置
//大块内存(>1MB)暂时由系统管理。

//////////////////////////////////////////////////////////////////
static void* czl_mm_sys_alloc(czl_gp*, czl_ulong);
static void* czl_mm_sys_realloc(czl_gp*, void*, czl_ulong, czl_ulong);
static void* czl_mm_different_page_resize(czl_gp*, void*, czl_ulong,
                                          czl_ulong, czl_mm_sp_pool*);
//////////////////////////////////////////////////////////////////
#ifdef CZL_MM_SLAB
static unsigned long czl_mm_hash_index(void *heap, long mask)
{
    long index = (unsigned long)heap | mask;

    if (index < 0)
        index = -index;

    return index-1;
}

static char czl_mm_hash_resize
(
    czl_gp *gp,
    unsigned long new_size,
    czl_mm_hash *table
)
{
	unsigned long i;
    unsigned long index;
    czl_mm_heap *p, *q;
    czl_mm_heap **datas = (czl_mm_heap**)calloc(new_size,
                                         sizeof(czl_mm_heap*));
    if (!datas)
        return 0;

    table->mask = -new_size;

    for (i = 0; i < table->size; i++)
    {
        for (p = table->datas[i]; p; p = q)
        {
            q = p->hash_next;
            index = czl_mm_hash_index(p->heap, table->mask);
            p->hash_next = datas[index];
            datas[index] = p;
        }
    }

    free(table->datas);

    gp->mm_cnt -= (table->size*sizeof(czl_mm_heap*));
    gp->mm_cnt += (new_size*sizeof(czl_mm_heap*));

    table->size = new_size;
    table->datas = datas;

    return 1;
}

static char czl_mm_hash_insert(czl_gp *gp, czl_mm_heap *node, czl_mm_hash *table)
{
	unsigned long index;

    if (table->count >= table->size &&
        !czl_mm_hash_resize(gp, table->size<<1, table))
        return 0;

    index = czl_mm_hash_index(node->heap, table->mask);
    node->hash_next = table->datas[index];
    table->datas[index] = node;
    table->count++;

    return 1;
}

static czl_mm_heap* czl_mm_hash_find(void *heap, czl_mm_hash *table)
{
    unsigned long index = czl_mm_hash_index(heap, table->mask);
    czl_mm_heap *p = table->datas[index];

    while (p && heap != p->heap)
        p = p->hash_next;

    return p;
}

char czl_mm_hash_init(czl_gp *gp, czl_mm_hash *table)
{
    if (!(table->datas=calloc(1, sizeof(czl_mm_heap*))))
        return 0;

    table->size = 1;
    table->mask = -table->size;
    table->count = 0;

    gp->slab_pool.count += sizeof(czl_mm_heap*);
    gp->mm_cnt += sizeof(czl_mm_heap*);

    return 1;
}

static czl_mm_heap* czl_mm_heap_create
(
    czl_gp *gp,
    unsigned short index,
    czl_mm_slab *slab
)
{
    czl_mm_heap *p = malloc(16+slab->size);
    if (!p)
        return NULL;

    p->color = CZL_MM_COLOR_BLACK;
    p->index = index;
    p->hash_next = NULL;

    p->next = NULL;
    if (NULL == slab->head)
    {
        slab->head = p;
        p->last = NULL;
    }
    else
    {
        slab->tail->next = p;
        p->last = slab->tail;
    }
    slab->tail = p;

    gp->slab_pool.count += (16+slab->size);
    gp->mm_cnt += (16+slab->size);

    return p;
}

static void czl_mm_heap_delete
(
    czl_mm_heap **head,
    czl_mm_heap **tail,
    czl_mm_heap *node
)
{
    if (node->last)
        node->last->next = node->next;
    else
        *head = node->next;

    if (node->next)
        node->next->last = node->last;
    else
        *tail = node->last;
}

char czl_mm_slabs_init(czl_gp *gp)
{
	int i;

    if (!czl_mm_hash_init(gp, &gp->slab_pool.gc))
        return 0;

    gp->slab_pool.array = calloc(CZL_MM_SLAB_RANGE, sizeof(czl_mm_slab));
    if (!gp->slab_pool.array)
        return 0;

    for (i = 0; i < CZL_MM_SLAB_RANGE; i++)
    {
        gp->slab_pool.array[i].head = NULL;
        gp->slab_pool.array[i].tail = NULL;
        gp->slab_pool.array[i].size = CZL_MM_SP_512B +
                                      (i+1)*CZL_MM_SLAB_GAP;
        gp->slab_pool.array[i].freq = 0;
        gp->slab_pool.array[i].count = 0;
    }

    gp->slab_pool.count += (CZL_MM_SLAB_RANGE*sizeof(czl_mm_slab));
    gp->mm_cnt += (CZL_MM_SLAB_RANGE*sizeof(czl_mm_slab));

    return 1;
}

void czl_mm_slabs_destroy(czl_gp *gp, czl_mm_slab_pool *pool)
{
	int i;
	czl_mm_heap *p, *q;
    czl_mm_slab *a = pool->array;

    if (!a)
        return;

    free(pool->gc.datas);
    gp->mm_cnt -= (pool->gc.size*sizeof(czl_mm_heap*));

    for (i = 0; i < CZL_MM_SLAB_RANGE; i++)
    {
        for (p = a[i].head; p; p = q)
        {
            q = p->next;
            free(p);
            gp->mm_cnt -= (16+a[i].size);
        }
    }

    free(pool->array);
    gp->mm_cnt -= (CZL_MM_SLAB_RANGE*sizeof(czl_mm_slab));
}

static void* czl_mm_slab_alloc(czl_gp *gp, unsigned long size)
{
	czl_mm_slab *s;
	czl_mm_heap *node;
	int l = 0, h = CZL_MM_SLAB_RANGE-1, m;

    if (!gp->slab_pool.count && !czl_mm_slabs_init(gp))
        return NULL;

    s = gp->slab_pool.array;
    do {
        m = (l+h)/2;
        if (size <= s[m].size - CZL_MM_SLAB_GAP)
            h = m-1;
        else if (size > s[m].size)
            l = m+1;
        else
            break;
    } while (l <= h);

    s += m;
    node = s->head;
    if (node && CZL_MM_COLOR_WHITE == node->color)
    {
        node->color = CZL_MM_COLOR_BLACK;
        if (node->next)
        {
            node->next->last = NULL;
            s->head = node->next;
            node->next = NULL;
            node->last = s->tail;
            s->tail->next = node;
            s->tail = node;
        }
    }
    else
    {
        if (gp->mm_cache < gp->slab_pool.count + 16 + size)
            return czl_mm_sys_alloc(gp, size);
        if (!(node=czl_mm_heap_create(gp, m, s)))
            return NULL;
        if (!czl_mm_hash_insert(gp, node, &gp->slab_pool.gc))
            return NULL;
        s->count++;
    }

    s->freq++;

    return node->heap;
}

static void* czl_mm_slab_resize
(
    czl_gp *gp,
    void *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pool
)
{
	unsigned long size;
    czl_mm_heap *node = czl_mm_hash_find(heap, &gp->slab_pool.gc);
    if (!node)
        return czl_mm_different_page_resize(gp, heap, new_size, old_size, pool);

    size = CZL_MM_SP_512B + node->index*CZL_MM_SLAB_GAP;
    if (new_size > size && new_size <= size+CZL_MM_SLAB_GAP)
        return heap;
    else
        return czl_mm_different_page_resize(gp, heap, new_size, old_size, pool);
}

static void czl_mm_slab_free(czl_gp *gp, void *heap, unsigned long size)
{
	czl_mm_slab *s;
    czl_mm_heap *node = czl_mm_hash_find(heap, &gp->slab_pool.gc);
    if (!node)
    {
        free(heap);
        gp->mm_cnt -= size;
        return;
    }

    node->color = CZL_MM_COLOR_WHITE;
    s = gp->slab_pool.array + node->index;
    if (node->last && node->last->color != CZL_MM_COLOR_WHITE)
    {
        czl_mm_heap_delete(&s->head, &s->tail, node);
        node->last = NULL;
        node->next = s->head;
        s->head->last = node;
        s->head = node;
    }
}
#endif //#ifdef CZL_MM_SLAB
//////////////////////////////////////////////////////////////////
static void czl_mm_free(czl_gp*, char*, czl_ulong, czl_mm_sp_pool*);
//////////////////////////////////////////////////////////////////
static czl_mm_sp* czl_mm_sp_create(czl_mm_sp_pool *pool)
{
    czl_mm_sp *p = (czl_mm_sp*)malloc(CZL_MM_SP_HEAD_SIZE+pool->size);
    if (!p)
        return NULL;

    if (pool->size <= CZL_MM_SP_4KB)
        p->id = 0;
    else if (pool->size <= CZL_MM_SP_20KB)
        p->id = 1;
    else //<= CZL_MM_SP_40KB
        p->id = 2;

    p->state = 1;
    p->rest = p->piece = pool->size;
    p->checksum = CZL_MM_SP_CHECKSUM;

    p->last = NULL;
    p->next = pool->head;
    if (pool->head)
        pool->head->last = p;
    pool->head = p;

    return p;
}

static void czl_mm_sp_delete
(
    czl_mm_sp_pool *pool,
    czl_mm_sp *node
)
{
    if (node->last)
        node->last->next = node->next;
    else
        pool->head = node->next;

    if (node->next)
        node->next->last = node->last;

    free(node);
}

static void czl_mm_sp_break
(
    czl_mm_sp_pool *pool,
    czl_mm_sp *node
)
{
    if (node->state)
        return;

    node->state = 1;

    if (node->Last)
        node->Last->Next = node->Next;
    else
        pool->Head = node->Next;

    if (node->Next)
        node->Next->Last = node->Last;

    if (node == pool->at)
    {
        pool->at = node->Next;
        pool->addr = NULL;
    }
}

static void czl_mm_sp_insert(czl_mm_sp **Head, czl_mm_sp *node)
{
    node->state = 0;

    node->Last = NULL;
    node->Next = *Head;
    if (*Head)
        (*Head)->Last = node;
    *Head = node;
}

void czl_mm_sp_destroy(czl_gp *gp, czl_mm_sp_pool *pool)
{
    czl_mm_sp *p = pool->head, *q;
    const unsigned long size = CZL_MM_SP_HEAD_SIZE + pool->size;

    while (p)
    {
        q = p->next;
        free(p);
        p = q;
        gp->mm_cnt -= size;
    }
}
//////////////////////////////////////////////////////////////////
static void czl_mm_set_white
(
    char *heap,
    czl_mm_sp_pool *pool,
    czl_mm_sp *page
)
{
	char *here, *end;

    //前向合并
    if (CZL_MM_COLOR_BLACK == *(heap-1))
        *(heap-1) = CZL_MM_COLOR_WHITE;
    else //CZL_MM_COLOR_RED
    {
        unsigned short size = *((unsigned short*)heap);
        unsigned short len = *((unsigned short*)(heap-3));
        heap -= len;
        *((unsigned short*)heap) += (CZL_MM_HEAP_MSG_SIZE+size);
        if (page == pool->at && (heap+len-1) == pool->addr)
            pool->addr = heap - 1;
    }

    //后向合并
    here = heap + 4 + *((unsigned short*)heap);
    end = page->heap + pool->size;
    if (here == end)
    {
        page->rest = CZL_MM_HEAP_MSG_SIZE + *((unsigned short*)heap);
    }
    else
    {
        if (CZL_MM_COLOR_WHITE == *here)
        {
			char *tmp;
            *((unsigned short*)heap) += (CZL_MM_HEAP_MSG_SIZE +
                                         *((unsigned short*)(here+1)));
            tmp = heap + *((unsigned short*)heap);
            if (end == tmp+4)
                page->rest = CZL_MM_HEAP_MSG_SIZE +
                             *((unsigned short*)heap);
            else
                *((unsigned short*)(tmp+2)) = CZL_MM_HEAP_MSG_SIZE +
                                              *((unsigned short*)heap);
            if (page == pool->at && here == pool->addr)
                pool->addr = heap - 1;
        }
        else //CZL_MM_COLOR_BLACK
        {
            *here = CZL_MM_COLOR_RED;
            *((unsigned short*)(here-2)) = CZL_MM_HEAP_MSG_SIZE +
                                           *((unsigned short*)heap);
        }
    }
}

static void czl_mm_set_black
(
    char *page,
    char *old_here,
    char *new_here,
    unsigned short size,
    unsigned short rest
)
{
    *old_here = CZL_MM_COLOR_BLACK;
    *((unsigned short*)(old_here+1)) = size;
    *((unsigned short*)(old_here+3)) = old_here - page;

    if (rest)
    {
        *new_here = CZL_MM_COLOR_WHITE;
        *((unsigned short*)(new_here+1)) = rest - CZL_MM_HEAP_MSG_SIZE;
        *((unsigned short*)(new_here+3)) = new_here - page;
    }
}
//////////////////////////////////////////////////////////////////
static void* czl_mm_sys_alloc(czl_gp *gp, czl_ulong size)
{
    void *buf;

    if (gp->mm_cnt+size > gp->mm_limit)
        return NULL;

    if ((buf=malloc(size)))
        gp->mm_cnt += size;
    return buf;
}

static void* czl_mm_sp_rest_alloc
(
    czl_mm_sp_pool *pool,
    czl_mm_sp *page,
    unsigned short size
)
{
    char *heap = page->heap + pool->size - page->rest;
    page->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
    page->piece -= (CZL_MM_HEAP_MSG_SIZE+size);
    pool->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
    if (page->rest < CZL_MM_HEAP_MIN_SIZE)
    {
        size += page->rest;
        pool->rest -= page->rest;
        page->piece -= page->rest;
        if (0 == page->piece)
            czl_mm_sp_break(pool, page);
        page->rest = 0;
    }
    czl_mm_set_black(page->heap, heap,
                     heap+CZL_MM_HEAP_MSG_SIZE+size,
                     size, page->rest);
    return heap+CZL_MM_HEAP_MSG_SIZE;
}

static void* czl_last_addr_alloc
(
    czl_mm_sp_pool *pool,
    unsigned short size
)
{
    char *heap = pool->addr;
    char *end = pool->at->heap + pool->size;
    unsigned short len;

CZL_BEGIN:
    for (; heap != end; heap += (CZL_MM_HEAP_MSG_SIZE+len))
    {
        len = *((unsigned short*)(heap+1));
        if (*heap != CZL_MM_COLOR_WHITE || len < size)
            continue;
        if (len-size < CZL_MM_HEAP_MIN_SIZE)
        {
            size = len;
            pool->addr = heap + CZL_MM_HEAP_MSG_SIZE + size;
            pool->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
            pool->at->piece -= (CZL_MM_HEAP_MSG_SIZE+size);
            *heap = CZL_MM_COLOR_BLACK;
            if (pool->addr == end)
                pool->at->rest = 0;
            else
                *pool->addr = CZL_MM_COLOR_BLACK;
        }
        else
        {
            pool->addr = heap + CZL_MM_HEAP_MSG_SIZE + size;
            pool->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
            pool->at->piece -= (CZL_MM_HEAP_MSG_SIZE+size);
            czl_mm_set_black(pool->at->heap, heap,
                             heap+CZL_MM_HEAP_MSG_SIZE+size,
                             size, len-size);
            if (pool->addr == end)
                pool->at->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
            else
                *((unsigned short*)(heap+len+3)) = len-size;
        }
        if (0 == pool->at->piece)
            czl_mm_sp_break(pool, pool->at);
        return heap+CZL_MM_HEAP_MSG_SIZE;
    }

    if (pool->len && 4*pool->at->piece > pool->size)
    {
        heap = pool->at->heap;
        goto CZL_BEGIN;
    }

    pool->at = pool->at->next;
    pool->addr = NULL;
    return NULL;
}

static void* czl_mm_sp_slow_alloc
(
    czl_mm_sp_pool *pool,
    unsigned short size
)
{
    czl_mm_sp *p = (pool->at ? pool->at : pool->Head), *q = NULL;

CZL_BEGIN:
    for (; p != q; p = p->Next)
    {
		unsigned short len;
		char *heap, *end;
        if (p->piece < CZL_MM_HEAP_MSG_SIZE+size)
            continue;
		end = p->heap + pool->size;
        for (heap = p->heap; heap != end; heap += (CZL_MM_HEAP_MSG_SIZE+len))
        {
            len = *((unsigned short*)(heap+1));
            if (*heap != CZL_MM_COLOR_WHITE || len < size)
                continue;
            pool->at = p;
            if (len-size < CZL_MM_HEAP_MIN_SIZE)
            {
                size = len;
                pool->addr = heap + CZL_MM_HEAP_MSG_SIZE + size;
                pool->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
                p->piece -= (CZL_MM_HEAP_MSG_SIZE+size);
                *heap = CZL_MM_COLOR_BLACK;
                if (pool->addr == end)
                    p->rest = 0;
                else
                    *pool->addr = CZL_MM_COLOR_BLACK;
            }
            else
            {
                pool->addr = heap + CZL_MM_HEAP_MSG_SIZE + size;
                pool->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
                p->piece -= (CZL_MM_HEAP_MSG_SIZE+size);
                czl_mm_set_black(p->heap, heap,
                                 pool->addr,
                                 size, len-size);
                if (pool->addr == end)
                    p->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
                else
                    *((unsigned short*)(heap+len+3)) = len-size;
            }
            if (0 == p->piece)
                czl_mm_sp_break(pool, p);
            return heap+CZL_MM_HEAP_MSG_SIZE;
        }
    }

    if (pool->at)
    {
        p = pool->Head;
        q = pool->at;
        pool->at = NULL;
        goto CZL_BEGIN;
    }

    return NULL;
}

static void* czl_mm_malloc(czl_gp *gp, czl_ulong size, czl_mm_sp_pool *pool)
{
	czl_mm_sp *page;

    if (0 == pool->len)
    {
        if (0 == size)
            return NULL;
        else if (size <= CZL_MM_SP_128B)
        {
            if (1 == size) size = 2;
        }
        else if (size <= CZL_MM_SP_256B)
            ++pool;
        else if (size <= CZL_MM_SP_512B)
            pool += 2;
    #ifdef CZL_MM_SLAB
        else if (gp->mm_cache && size <= CZL_MM_1MB)
            return czl_mm_slab_alloc(gp, size);
    #endif //#ifdef CZL_MM_SLAB
        else
            return czl_mm_sys_alloc(gp, size);
    }

    if (pool->head)
    {
        if (pool->head->rest >= CZL_MM_HEAP_MSG_SIZE+size)
            return czl_mm_sp_rest_alloc(pool, pool->head, size);
        else if (pool->head->rest && pool->head->state)
            czl_mm_sp_insert(&pool->Head, pool->head);
    }
    if (pool->addr)
    {
        void *heap = czl_last_addr_alloc(pool, size);
        if (heap)
            return heap;
    }
    if (4*pool->rest > pool->count)
    {
        //牺牲25%内存换取极端情况下能实现平均最多4次循环就能找到合适内存分配
        void *heap = czl_mm_sp_slow_alloc(pool, size);
        if (heap)
            return heap;
    }

    if (gp->mm_cnt+CZL_MM_SP_HEAD_SIZE+pool->size > gp->mm_limit)
        return NULL;

    if (!(page=czl_mm_sp_create(pool)))
        return NULL;
    pool->count += pool->size;
    gp->mm_cnt += (CZL_MM_SP_HEAD_SIZE+pool->size);

    page->rest -= (CZL_MM_HEAP_MSG_SIZE+size);
    page->piece = page->rest;
    pool->rest += page->rest;
    czl_mm_set_black(page->heap,
                     page->heap,
                     page->heap+CZL_MM_HEAP_MSG_SIZE+size,
                     size, page->rest);
    return page->heap+CZL_MM_HEAP_MSG_SIZE;
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

    memset(heap, 0, size);
    return heap;
}

static void* czl_mm_different_page_resize
(
    czl_gp *gp,
    void *heap,
    czl_ulong new_size,
    czl_ulong old_size,
    czl_mm_sp_pool *pool
)
{
    void *buf = czl_mm_malloc(gp, new_size, pool);
    if (!buf)
        return NULL;
    if (new_size > old_size)
        memcpy(buf, heap, old_size);
    else
        memcpy(buf, heap, new_size);
    czl_mm_free(gp, (char*)heap, old_size, pool);
    return buf;
}

static void* czl_mm_same_page_add
(
    czl_gp *gp,
    char *heap,
    unsigned long new_size,
    unsigned long old_size,
    czl_mm_sp *page,
    czl_mm_sp_pool *pool,
    czl_mm_sp_pool *pools
)
{
    unsigned long heap_size = *((unsigned short*)(heap-4));
    if (heap_size >= new_size)
        return heap;

    if (heap + heap_size != page->heap + pool->size &&
        CZL_MM_COLOR_WHITE == *(heap+heap_size))
    {
        unsigned long size = heap_size +
                             CZL_MM_HEAP_MSG_SIZE +
                             *((unsigned short*)(heap+heap_size+1));
        if (size < new_size)
            goto CZL_RESIZE;
        if (heap + size == page->heap + pool->size)
        {
            if (size-new_size < CZL_MM_HEAP_MIN_SIZE)
            {
                *((unsigned short*)(heap-4)) = size;
                page->rest -= (size-heap_size);
                pool->rest -= (size-heap_size);
                page->piece -= (size-heap_size);
                if (0 == page->piece)
                    czl_mm_sp_break(pool, page);
            }
            else
            {
                *((unsigned short*)(heap-4)) = new_size;
                page->rest -= (new_size-heap_size);
                pool->rest -= (new_size-heap_size);
                page->piece -= (new_size-heap_size);
                *(heap+new_size) = CZL_MM_COLOR_WHITE;
                *((unsigned short*)
                  (heap+new_size+1)) = page->rest - CZL_MM_HEAP_MSG_SIZE;
                *((unsigned short*)
                  (heap+new_size+3)) = heap + new_size - page->heap;
            }
        }
        else
        {
            if (size-new_size < CZL_MM_HEAP_MIN_SIZE)
            {
                *((unsigned short*)(heap-4)) = size;
                *(heap+size) = CZL_MM_COLOR_BLACK;
                pool->rest -= (size-heap_size);
                page->piece -= (size-heap_size);
                if (0 == page->piece)
                    czl_mm_sp_break(pool, page);
            }
            else
            {
                *((unsigned short*)(heap-4)) = new_size;
                pool->rest -= (new_size-heap_size);
                page->piece -= (new_size-heap_size);
                *(heap+new_size) = CZL_MM_COLOR_WHITE;
                *((unsigned short*)
                  (heap+new_size+1)) = size - new_size - CZL_MM_HEAP_MSG_SIZE;
                *((unsigned short*)
                  (heap+new_size+3)) = heap + new_size - page->heap;
                *((unsigned short*)(heap+size-2)) = size - new_size;
            }
        }
        if (page == pool->at && (heap+heap_size) == pool->addr)
            pool->addr = heap - CZL_MM_HEAP_MSG_SIZE;
        return heap;
    }

CZL_RESIZE:
    return czl_mm_different_page_resize(gp, heap, new_size,
                                        old_size, pools);
}

static void* czl_mm_same_page_dec
(
    czl_gp *gp,
    char *heap,
    unsigned long new_size,
    unsigned long old_size,
    czl_mm_sp *page,
    czl_mm_sp_pool *pool,
    czl_mm_sp_pool *pools
)
{
	unsigned long heap_size;

    if (0 == new_size)
    {
        czl_mm_free(gp, heap, old_size, pools);
        return NULL;
    }

    heap_size = *((unsigned short*)(heap-4));

    if (CZL_MM_COLOR_WHITE == *(heap+heap_size) &&
        page == pool->at && (heap+heap_size) == pool->addr)
        pool->addr = heap - CZL_MM_HEAP_MSG_SIZE;

    if (heap + heap_size == page->heap + pool->size)
    {
        unsigned long rest = heap_size-new_size;
        if (rest < CZL_MM_HEAP_MIN_SIZE)
            return heap;
        *((unsigned short*)(heap-4)) = new_size;
        *(heap+new_size) = CZL_MM_COLOR_WHITE;
        *((unsigned short*)(heap+new_size+1)) = rest - CZL_MM_HEAP_MSG_SIZE;
        *((unsigned short*)
          (heap+new_size+3)) = heap + new_size - page->heap;
        page->rest += rest;
        pool->rest += rest;
        page->piece += rest;
    }
    else
    {
        if (CZL_MM_COLOR_BLACK == *(heap+heap_size))
        {
			unsigned long size;
            if (heap_size-new_size < CZL_MM_HEAP_MIN_SIZE)
                return heap;
            *((unsigned short*)(heap-4)) = new_size;
            size = heap_size-new_size;
            *(heap+new_size) = CZL_MM_COLOR_WHITE;
            *((unsigned short*)
              (heap+new_size+1)) = size - CZL_MM_HEAP_MSG_SIZE;
            *((unsigned short*)
              (heap+new_size+3)) = heap + new_size - page->heap;
            pool->rest += size;
            page->piece += size;
            *(heap+heap_size) = CZL_MM_COLOR_RED;
            *((unsigned short*)(heap+heap_size-2)) = size;
        }
        else //CZL_MM_COLOR_WHITE
        {
			unsigned long size;
            *((unsigned short*)(heap-4)) = new_size;
            size = heap_size-new_size;
            pool->rest += size;
            page->piece += size;
            //
            *(heap+new_size) = CZL_MM_COLOR_WHITE;
            size += *((unsigned short*)(heap+heap_size+1));
            *((unsigned short*)(heap+new_size+1)) = size;
            *((unsigned short*)
              (heap+new_size+3)) = heap + new_size - page->heap;
            //
            if (heap + new_size + CZL_MM_HEAP_MSG_SIZE + size ==
                page->heap + pool->size)
                page->rest = CZL_MM_HEAP_MSG_SIZE + size;
            else
                *((unsigned short*)
                  (heap+new_size+3+size)) = CZL_MM_HEAP_MSG_SIZE + size;
        }
    }

    if (page->state)
        czl_mm_sp_insert(&pool->Head, page);
    return heap;
}

static void* czl_mm_sys_realloc
(
    czl_gp *gp,
    void *heap,
    czl_ulong new_size,
    czl_ulong old_size
)
{
	void *buf;
    if (new_size-old_size+gp->mm_cnt > gp->mm_limit)
        return NULL;

    if ((buf=realloc(heap, new_size)))
        gp->mm_cnt += (new_size-old_size);
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
	czl_mm_sp *page;
	czl_mm_sp_pool *pool;

    if (!heap || 0 == old_size)
        return czl_mm_malloc(gp, new_size, pools);
    if (new_size == old_size)
        return heap;

    if (new_size > CZL_MM_SP_512B || old_size > CZL_MM_SP_512B)
    {
#ifdef CZL_MM_SLAB
        if (gp->mm_cache && new_size <= CZL_MM_1MB)
            return czl_mm_slab_resize(gp, heap, new_size, old_size, pools);
        else if (old_size > CZL_MM_1MB && new_size > CZL_MM_1MB)
            return czl_mm_sys_realloc(gp, heap, new_size, old_size);
        else
            return czl_mm_different_page_resize(gp, heap, new_size,
                                                old_size, pools);
#else
        if (new_size > CZL_MM_SP_512B && old_size > CZL_MM_SP_512B)
            return czl_mm_sys_realloc(gp, heap, new_size, old_size);
        else
            return czl_mm_different_page_resize(gp, heap, new_size,
                                                old_size, pools);
#endif //#ifdef CZL_MM_SLAB
    }

    
	page = (czl_mm_sp*)(heap-*((unsigned short*)(heap-2)) -
						CZL_MM_HEAP_MSG_SIZE - CZL_MM_SP_HEAD_SIZE);
    pool = pools + page->id;

    switch (page->id)
    {
    case 0:
        if (new_size <= CZL_MM_SP_128B)
        {
            if (new_size > old_size)
                return czl_mm_same_page_add(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
            else
                return czl_mm_same_page_dec(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
        }
        break;
    case 1:
        if (new_size > CZL_MM_SP_128B && new_size <= CZL_MM_SP_256B)
        {
            if (new_size > old_size)
                return czl_mm_same_page_add(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
            else
                return czl_mm_same_page_dec(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
        }
        break;
    default: //2
        if (new_size > CZL_MM_SP_256B && new_size <= CZL_MM_SP_512B)
        {
            if (new_size > old_size)
                return czl_mm_same_page_add(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
            else
                return czl_mm_same_page_dec(gp, heap, new_size,
                                            old_size, page,
                                            pool, pools);
        }
        break;
    }

    return czl_mm_different_page_resize(gp, heap, new_size,
                                        old_size, pools);
}

static void czl_mm_free
(
    czl_gp *gp,
    char *heap,
    czl_ulong size,
    czl_mm_sp_pool *pool
)
{
    czl_mm_sp *page;

    if (0 == pool->len)
    {
        if (0 == size || !heap)
            return;
    #ifdef CZL_MM_SLAB
        if (gp->mm_cache && size > CZL_MM_SP_512B && size <= CZL_MM_1MB)
        {
            czl_mm_slab_free(gp, heap, size);
            return;
        }
        else
    #endif //#ifdef CZL_MM_SLAB
        if (size > CZL_MM_SP_512B)
        {
            free(heap);
            gp->mm_cnt -= size;
            return;
        }
    }

    page = (czl_mm_sp*)(heap-*((unsigned short*)(heap-2)) -
                        CZL_MM_HEAP_MSG_SIZE - CZL_MM_SP_HEAD_SIZE);
    if (CZL_MM_SP_ERROR(page))
    {
        //pool字段用于校验内存池是否匹配，不匹配说明该heap分配和释放使用了不同的池
        assert(0);
    }

    if (pool->len)
    {
        pool->rest += (CZL_MM_HEAP_MSG_SIZE+size);
        page->piece += (CZL_MM_HEAP_MSG_SIZE+size);
    }
    else
    {
        pool += page->id;
        pool->rest += (CZL_MM_HEAP_MSG_SIZE+*((unsigned short*)(heap-4)));
        page->piece += (CZL_MM_HEAP_MSG_SIZE+*((unsigned short*)(heap-4)));
    }

    if (page->piece == pool->size)
    {
        czl_mm_sp_break(pool, page);
        czl_mm_sp_delete(pool, page);
        pool->rest -= pool->size;
        pool->count -= pool->size;
        gp->mm_cnt -= (CZL_MM_SP_HEAD_SIZE+pool->size);
    }
    else
    {
        czl_mm_set_white(heap-4, pool, page);
        if (page->state)
            czl_mm_sp_insert(&pool->Head, page);
    }
}
//////////////////////////////////////////////////////////////////
#define CZL_MM_SP_CHECK(size, pool) \
(size <= CZL_MM_SP_512B && 4*pool.rest > pool.count)

#define CZL_MM_SPS_CHECK(size, pools) \
((size <= CZL_MM_SP_128B && 4*pools[0].rest > pools[0].count) || \
 (size <= CZL_MM_SP_256B && 4*pools[1].rest > pools[1].count) || \
 (size <= CZL_MM_SP_512B && 4*pools[2].rest > pools[2].count))

static void czl_vars_gc(czl_gp*, czl_var*, unsigned long);
static void czl_str_gc(czl_gp*, czl_str*);
static czl_ins* czl_ins_gc(czl_gp*, czl_ins*);
static void czl_tab_gc(czl_gp*, czl_value*);
static void czl_arr_gc(czl_gp*, czl_value*);
static void czl_sql_gc(czl_gp*, czl_value*);

static char czl_val_gc(czl_gp *gp, czl_var *var)
{
    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FILE:
    case CZL_FUN_REF: case CZL_OBJ_REF:
        return 0;
    case CZL_STRING:
        czl_str_gc(gp, &var->val.str);
        return 0;
    case CZL_INSTANCE:
        var->val.obj = czl_ins_gc(gp, (czl_ins*)var->val.obj);
        break;
    case CZL_TABLE:
        czl_tab_gc(gp, &var->val);
        break;
    case CZL_ARRAY:
        czl_arr_gc(gp, &var->val);
        break;
    default: //CZL_STACK/CZL_QUEUE/CZL_LIST
        czl_sql_gc(gp, &var->val);
        break;
    }

    return (CZL_OBJ_IS_LOCK(var));
}

static void czl_str_gc(czl_gp *gp, czl_str *str)
{
	unsigned long size;

    if (str->s->rc > 1)
        return;

    size = CZL_SL(str->size);
    if (size < gp->mm_gc_max_str_size &&
        CZL_MM_SPS_CHECK(size, gp->mmp_str))
    {
        czl_string *buf = CZL_STR_MALLOC(gp, size);
        if (buf)
        {
            memcpy(buf, str->s, size);
            CZL_STR_FREE(gp, str->s, size);
            str->s = buf;
        }
        else if (size < gp->mm_gc_max_str_size)
            gp->mm_gc_max_str_size = size;
    }
}

static czl_ins* czl_ins_gc(czl_gp *gp, czl_ins *ins)
{
    unsigned char flag = 0;
	czl_var *var = CZL_GIV(ins);
	czl_ins **p = ins->parents;
    unsigned long i, j = ins->pclass->parents_count;
    for (i = 0; i < j; i++)
        p[i] = czl_ins_gc(gp, p[i]);
    
    j = ins->pclass->vars_count;
    for (i = 0; i < j; ++i)
        if (czl_val_gc(gp, var++) && 0 == flag)
            flag = 1;

    if (1 == ins->rc && 0 == ins->rf && 0 == flag)
    {
        unsigned long size = CZL_IL(ins->pclass->parents_count,
                                    ins->pclass->vars_count);
        if (CZL_MM_SP_CHECK(size, ins->pclass->pool))
        {
            czl_ins *buf = (czl_ins*)czl_malloc(gp, size, &ins->pclass->pool);
            if (buf)
            {
				czl_var *var;
				unsigned long i, j = ins->pclass->vars_count;
                memcpy(buf, ins, size);
                czl_free(gp, ins, size, &ins->pclass->pool);
                ins = buf;
                var = CZL_GIV(ins);
                for (i = 0; i < j; ++i, ++var)
                    if (CZL_VAR_EXIST_REF(var))
                        czl_ref_obj_update(var);
            }
        }
    }

    return ins;
}

static void czl_tab_gc(czl_gp *gp, czl_value *val)
{
    czl_table *tab = (czl_table*)val->obj;
    unsigned long size;

    if (1 == tab->rc && 0 == tab->rf)
    {
        size = sizeof(czl_table);
        if (CZL_MM_SP_CHECK(size, gp->mmp_tab))
        {
            czl_table *buf = CZL_TAB_MALLOC(gp);
            if (buf)
            {
                memcpy(buf, tab, size);
                CZL_TAB_FREE(gp, tab);
                val->obj = tab = buf;
            }
        }
    }

    size = sizeof(czl_tabkv);
    if (CZL_MM_SP_CHECK(size, tab->pool))
    {
        czl_tabkv *p = tab->eles_head, *q = NULL;
        while (p)
        {
            p->last = q;
            if (czl_val_gc(gp, (czl_var*)p))
            {
                q = p;
                p = p->next;
                continue;
            }
            q = (czl_tabkv*)czl_malloc(gp, size, &tab->pool);
            *q = *p;
            czl_free(gp, p, size, &tab->pool);
            if (p == tab->foreach_inx)
                tab->foreach_inx = q;
            p = q->next;
            if (q->last)
                q->last->next = q;
            else
                tab->eles_head = q;
            if (CZL_VAR_EXIST_REF(q))
                czl_ref_obj_update((czl_var*)q);
        }
        tab->eles_tail = q;
        czl_table_rehash(tab);
    }
}

static void czl_arr_ref_obj_update(czl_array *arr)
{
    czl_var *var = arr->vars;
    unsigned long i, j = arr->cnt;
    for (i = 0; i < j; ++i, ++var)
        if (CZL_VAR_EXIST_REF(var))
        {
            czl_ref_var *p = ((czl_ref_obj*)var->name)->head;
            while (p)
            {
                p->var->val.ref.var = (czl_var*)arr;
                p = p->next;
            }
        }
}

static void czl_arr_gc(czl_gp *gp, czl_value *val)
{
    czl_array *arr = (czl_array*)val->obj;

    if (1 == arr->rc && 0 == arr->rf)
    {
        unsigned long size = sizeof(czl_array);
        if (CZL_MM_SP_CHECK(size, gp->mmp_arr))
        {
            czl_array *buf = CZL_ARR_MALLOC(gp);
            if (buf)
            {
                memcpy(buf, arr, size);
                CZL_ARR_FREE(gp, arr);
                val->obj = arr = buf;
                czl_arr_ref_obj_update(arr);
            }
        }
    }

    czl_vars_gc(gp, arr->vars, arr->cnt);
}

static void czl_sql_gc(czl_gp *gp, czl_value *val)
{
    czl_sql *sql = (czl_sql*)val->obj;
    unsigned long size;

    if (1 == sql->rc && 0 == sql->rf)
    {
        size = sizeof(czl_sql);
        if (CZL_MM_SP_CHECK(size, gp->mmp_sql))
        {
            czl_sql *buf = CZL_SQL_MALLOC(gp);
            if (buf)
            {
                memcpy(buf, sql, size);
                CZL_SQL_FREE(gp, sql);
                val->obj = sql = buf;
            }
        }
    }

    size = sizeof(czl_glo_var);
    if (CZL_MM_SP_CHECK(size, sql->pool))
    {
        czl_glo_var *p = sql->eles_head, *q = NULL, *last = NULL;
        while (p)
        {
            if (czl_val_gc(gp, (czl_var*)p))
            {
                last = p;
                p = p->next;
                continue;
            }
            q = (czl_glo_var*)czl_malloc(gp, size, &sql->pool);
            *q = *p;
            czl_free(gp, p, size, &sql->pool);
            if (p == sql->foreach_inx)
                sql->foreach_inx = q;
            p = q->next;
            if (last)
                last->next = q;
            else
                sql->eles_head = q;
            last = q;
            if (CZL_VAR_EXIST_REF(q))
                czl_ref_obj_update((czl_var*)q);
        }
        sql->eles_tail = q;
    }
}

static void czl_vars_gc(czl_gp *gp, czl_var *vars, unsigned long cnt)
{
    unsigned long i;
    for (i = 0; i < cnt; ++i)
        czl_val_gc(gp, vars+i);
}

static void czl_var_list_gc(czl_gp *gp, czl_glo_var *p)
{
    while (p)
    {
        czl_val_gc(gp, (czl_var*)p);
        p = p->next;
    }
}

static void czl_fun_list_gc(czl_gp *gp, czl_fun *f)
{
    while (f)
    {
        unsigned long i;
        czl_vars_gc(gp, f->vars, f->dynamic_vars_cnt+f->static_vars_cnt);
        for (i = 0; i < f->backup_cnt; ++i)
            czl_vars_gc(gp, f->backup_vars[i], f->dynamic_vars_cnt);
        f = f->next;
    }
}

//全局扫描整理各个内存池的外部碎片，压缩内存池使内存更连续，将多余内存释放给操作系统
void czl_mm_sp_pool_gc(czl_gp *gp)
{
	czl_fun *f;
	czl_class *c;

    gp->mm_gc_max_str_size = CZL_MM_SP_512B + 1;

    czl_var_list_gc(gp, gp->vars_head);

    for (f = gp->funs_head; f; f = f->next)
    {
		unsigned long i;
        if (f->dynamic_vars_cnt < 0)
            continue;
        if (CZL_SYS_FUN == f->type)
            czl_vars_gc(gp, f->vars, f->dynamic_vars_cnt);
        else
            czl_vars_gc(gp, f->vars, f->dynamic_vars_cnt+f->static_vars_cnt);
        for (i = 0; i < f->backup_cnt; ++i)
            czl_vars_gc(gp, f->backup_vars[i], f->dynamic_vars_cnt);
    }

    for (c = gp->class_head; c; c = c->next)
    {
        czl_var_list_gc(gp, (czl_glo_var*)c->vars);
        czl_fun_list_gc(gp, c->funs);
    }
}
//////////////////////////////////////////////////////////////////
static void czl_mm_sp_pool_init
(
    czl_mm_sp_pool *pool,
    unsigned short size,
    unsigned short len
)
{
    pool->head = pool->Head = pool->at = NULL;
    pool->addr = NULL;
    pool->size = size;
    pool->len = len;
    pool->rest = 0;
    pool->count = 0;
}

void czl_mm_pool_init(czl_mm_sp_pool *pool, unsigned short len)
{
    if (len > 0)
    {
        unsigned short size = CZL_MM_HEAP_MSG_SIZE + len;
        if (len <= CZL_MM_SP_128B)
            czl_mm_sp_pool_init(pool, CZL_MM_SP_4KB/size*size, len);
        else if (len <= CZL_MM_SP_256B)
            czl_mm_sp_pool_init(pool, CZL_MM_SP_20KB/size*size, len);
        else //<= CZL_MM_SP_512B
            czl_mm_sp_pool_init(pool, CZL_MM_SP_40KB/size*size, len);
    }
    else
    {
        czl_mm_sp_pool_init(pool, CZL_MM_SP_4KB, 0);
        czl_mm_sp_pool_init(pool+1, CZL_MM_SP_20KB, 0);
        czl_mm_sp_pool_init(pool+2, CZL_MM_SP_40KB, 0);
    }
}

void czl_mm_module_init
(
    czl_gp *gp
#ifdef CZL_MM_SLAB
    , unsigned long cache
#endif
)
{
    czl_mm_pool_init(gp->mmp_tmp, 0);
    czl_mm_pool_init(gp->mmp_rt, 0);
    czl_mm_pool_init(gp->mmp_stack, 0);
    czl_mm_pool_init(gp->mmp_str, 0);

    czl_mm_pool_init(&gp->mmp_tab, sizeof(czl_table));
    czl_mm_pool_init(&gp->mmp_arr, sizeof(czl_array));
    czl_mm_pool_init(&gp->mmp_sql, sizeof(czl_sql));
    czl_mm_pool_init(&gp->mmp_ref, sizeof(czl_ref_var));
    czl_mm_pool_init(&gp->mmp_cor, sizeof(czl_coroutine));
#ifdef CZL_MULT_THREAD
    czl_mm_pool_init(&gp->mmp_thread, sizeof(czl_thread));
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_MM_SLAB
    gp->mm_cache = cache;
    gp->slab_pool.gc.datas = NULL;
    gp->slab_pool.array = NULL;
    gp->slab_pool.count = 0;
#endif //#ifdef CZL_MM_SLAB
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
        czl_mm_sp_pool_gc(gp);
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
        czl_mm_sp_pool_gc(gp);
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
        czl_mm_sp_pool_gc(gp);
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

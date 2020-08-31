#ifndef __STL_ALLOC_H
#define __STL_ALLOC_H

// 第一级配置器 __malloc_alloc_template
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

#ifdef __STL_THREADS
# include <stl_threads.h>
# define __NODE_ALLOCATOR_THREADS true
# ifdef __STL_SGI_THREADS
  // We test whether threads are in use before locking.
  // Perhaps this should be moved into stl_threads.h, but that
  // probably makes it harder to avoid the procedure call when
  // it isn't needed.
    extern "C" {
      extern int __us_rsthread_malloc;
    }
	// The above is copied from malloc.h.  Including <malloc.h>
	// would be cleaner but fails with certain levels of standard
	// conformance.
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_release_lock(); }
# else /* !__STL_SGI_THREADS */
#   define __NODE_ALLOCATOR_LOCK \
        { if (threads) _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK \
        { if (threads) _S_node_allocator_lock._M_release_lock(); }
# endif
#else
//  Thread-unsafe
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS false
#endif

// malloc-based allocator 通常比稍后介绍的 default alloc 速度慢
// 一般而言是 thread-safe, 并且对于空间的运用比较高效
// 以下是第一级配置器
// 注意, 无 "template型别参数", 至于 "非型别参数" inst, 则完全没派上用场
template <int inst>
class __malloc_alloc_template {

private:
    // 以下函数用来处理内存不足的情况
    // oom : out of memory.
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static void (* __malloc_alloc_oom_handler) ();

public:

static void * allocate(size_t n)
{
    void *result = malloc(n);       //  第一级配置器直接使用 malloc()
    // 以下无法满足需求时，改用 oom_malloc()
    if (0 == result) result = oom_malloc(n);
    return result;
}

static void deallocate(void *p, size_t /* n */)
{
    free(p);    // 第一级配置器直接使用 free()
}

static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
{
    void * result = realloc(p, new_sz);     // 第一级配置器直接使用 realloc()
    // 以下无法满足需求时，改用 oom_realloc()
    if (0 == result) result = oom_realloc(p, new_sz);
    return result;
}

// 以下仿真 c++ 的 set_new_handler(). 换句话说, 你可以通过它
// 指定你自己的 out-of-memory handler
static void (* set_malloc_handler(void (*f)())) ()
{
    void (* old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return (old);
}
};

// malloc_alloc out-of-memory handling
// 初值为0. 有待客端设定
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler) () = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void (* my_malloc_handler) ();
    void *result;

    for (;;) {                  // 不断尝试释放, 配置, 再释放, 再配置...
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)(); // 调用处理例程, 企图释放内存
        result = malloc(n);     // 再次尝试配置内存
        if (result) return (result);
    }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    for (;;) {                      // 不断尝试释放, 配置, 再释放, 再配置...
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();     // 调用处理例程, 企图释放内存
        result = realloc(p, n);     // 再次尝试配置内存
        if (result) return (result);
    }
}

// 注意, 以下直接将参数 inst 指定为 0
typedef __malloc_alloc_template<0> malloc_alloc;

/*******************************************************/

enum {__ALIGN = 8};         // 小型区块的上调边界
enum {__MAX_BYTES = 128};   // 小型区块的上限
enum {__NFREELISTS = __MAX_BYTES / __ALIGN};    // free-lists 个数

// 第二级配置器
// 注意, 无 "template型别参数", 且第二参数完全没派上用场
// 第一参数用于多线程环境下. 此处不涉及
template <bool threads, int inst>
class __default_alloc_template {

private:
    // ROUND_UP() 将 bytes 上调至 8 的倍数
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }
private:
    union obj {     // free-lists 的节点构造
        union obj * free_list_link;
        char client_data[1];    // The client sees this.
    };
private:
    // 16 个 free-lists
    static obj * volatile free_list[__NFREELISTS];
    // 以下函数根据区块大小, 决定使用第 n 号 free-list, n 从 0 起算
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }

    // 返回一个大小为 n 的对象, 并可能加入大小为 n 的其他区块到 free-list
    static void *refill(size_t n);
    // 配置一大块空间, 可容纳 nobjs 个大小为 "size" 的区块  
    // 如果配置 nobjs 个区块有所不便, nobjs 可能会降低
    static char *chunk_alloc(size_t size, int &nobjs);

    // Chunk allocation state
    static char *start_free;    // 内存池起始位置, 只在 chunk_alloc() 中变化
    static char *end_free;      // 内存池结束位置, 只在 chunk_alloc() 中变化
    static size_t heap_size;

public:
    static void * allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void * reallocate(void *p, size_t old_sz, size_t new_sz);
};

// 以下是 static data member 的定义与初值设定
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

// n must be > 0
template <bool threads, int inst>
void *__default_alloc_template<threads, inst>::allocate(size_t n)
{
    obj * volatile * my_free_list;
    obj * result;

    // 大于 128 就调用第一级配置器
    if (n > (size_t) __MAX_BYTES)
    {
        return (malloc_alloc::allocate(n));
    }
    // 寻找 16 个 free lists 中适当的一个
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == 0)
    {
        // 没找到可用的 free list, 准备重新填充 free list
        void *r = refill(ROUND_UP(n));
        return r;
    }
    // 调整 free list
    *my_free_list = result->free_list_link;
    return result;
}

// p cannot be 0
template <bool threads, int inst>
void __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
{
    obj *q = (obj *)p;
    obj * volatile * my_free_list;

    // 大于 128 就调用第一级配置器
    if (n > (size_t) __MAX_BYTES)
    {
        malloc_alloc::deallocate(p, n);
        return;
    }
    // 寻找对应的 free list
    my_free_list = free_list + FREELIST_INDEX(n);
    // 调整 free list, 回收区块
    q->free_list_link = *my_free_list;
    *my_free_list = q;
}

// 返回一个大小为 n 的对象, 并且有时候会为适当的 free list 增加节点
// 假设 n 已经适当上调至 8 的倍数
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    // 调用 chunk_alloc(), 尝试取得 nobjs 个区块作为 free list 的新节点
    // 注意参数 nobjs 是 pass by reference
    char * chunk = chunk_alloc(n, nobjs);
    obj * volatile * my_free_list;
    obj * result;
    obj * current_obj, * next_obj;

    // 如果只获得一个区块, 这个区块就分配给调用者用, free list 无新节点
    if (1 == nobjs) return chunk;
    // 否则准备调整 free list, 纳入新节点
    my_free_list = free_list + FREELIST_INDEX(n);

    // 以下在 chunk 空间内建立 free list
    result = (obj *)chunk;      // 这一块准备返回给客端
    // 以下引导 free list 指向新配置的空间(取自内存池)
    *my_free_list = next_obj = (obj *)(chunk + n);
    // 以下将 free list 的各节点串接起来
    for (int i = 1; ; i++) {        // 从 1 开始, 因为第 0 个将返回给客端
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj->free_list_link = 0;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

template <bool threads, int inst>
char *
__default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;      // 内存池剩余空间

    if (bytes_left >= total_bytes) {
        // 内存池剩余空间完全满足需求量
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        // 内存池剩余空间不能完全满足需求量, 但足够供应一个(含)以上的区块
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        // 内存池剩余空间连一个区块的大小都无法提供
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        // 以下试着让内存池重的残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池内还有一些零头, 先配给适当的 free list
            // 首先寻找适当的 free list
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整 free list, 将内存池中的残余空间编入
            ((obj *)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        // 配置 heap 空间, 用来补充内存池
        start_free = (char *)malloc(bytes_to_get);
        if (0 == start_free) {
            // heap 空间不足, malloc() 失败
            obj * volatile * my_free_list, *p;
            // 试着检视我们手上拥有的东西. 这不会造成伤害. 我们不打算尝试配置
            // 较小的区块, 因为那在多进程 (multi-process) 机器上容易导致灾难
            // 以下搜寻适当的 free list
            // 所谓适当是指 "尚有未用区块, 且区块够大" 之 free list
            for (int i = size;  i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (0 != p) {   // free list 内尚有未用区块
                    // 调整 free list 以释出未用区块
                    *my_free_list = p->free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + 1;
                    // 递归调用自己, 为了修正 nobjs
                    return chunk_alloc(size, nobjs);
                    // 注意, 任何残余零头终将被编入适当的 free list 中备用
                }
            }
            end_free = 0;   // 如果出现意外 (山穷水尽, 到处都没内存可用了)
            // 调用第一级配置器, 看看 out-of-memory 机制能否尽点力
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            // 这会导致抛出异常, 或内存不足的情况获得改善
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            // 递归调用自己, 为了修正 nobjs
            return chunk_alloc(size, nobjs);
        }
    }
}

#ifdef __USE_MALLOC
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;     // 令 alloc 为第一级配置器
#else
// 令 alloc 为第二级配置器
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
#endif

template<class T, class Alloc>
class simple_alloc {
public:
    static T *allocate(size_t n)
      { return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof (T)); }
    static T *allocate(void)
      { return (T*) Alloc::allocate(sizeof (T)); }
    static void deallocate(T *p, size_t n)
      { if (0 != n) Alloc::deallocate(p, n * sizeof (T)); }
    static void deallocate(T *p)
      { Alloc::deallocate(p, sizeof (T)); }
};

#endif
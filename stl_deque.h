#ifndef __STL_DEQUE_H
#define __STL_DEQUE_H

#include "stl_config.h"
#include "stl_iterator.h"
#include "stl_alloc.h"

inline size_t __deque_buf_size(size_t n, size_t sz)
{
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {   // 未继承 std::iterator
    typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
    static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

    // 未继承 std::iterator, 所以必须自行撰写五个必要的迭代器相应型别
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Ptr                        pointer;
    typedef Ref                        reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef T**                        map_pointer;

    typedef __deque_iterator self;

    // 保持与容器的联结
    T* cur;             // 此迭代器所指之缓冲区中的现行(current)元素
    T* first;           // 此迭代器所指之缓冲区的头
    T* last;            // 此迭代器所指之缓冲区的尾(含备用空间)
    map_pointer node;   // 指向管控中心

    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }

    reference operator*() const { return *cur; }
    pointer operator->() const { return &(operator*()); }

    difference_type operator-(const self& x) const
    {
        return difference_type(buffer_size()) * (node - x.node - 1) +
            (cur - first) + (x.last - x.cur);
    }

    // 参考 More Effective C++, item6: Distinguish between prefix and
    // postfix forms of increment and decrement operators
    self& operator++()
    {
        ++cur;                  // 切换至下一个元素
        if (cur == last) {      // 如果已达所在缓冲区的尾端
            set_node(node + 1); // 就切换至下一节点(亦即缓冲区)的第一个元素
            cur = first; 
        }
        return *this;
    }

    // 后置式, 标准写法
    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        if (cur == first) {     // 如果已达所在缓冲区的头端
            set_node(node + 1); // 就切换至前一节点(亦即缓冲区)的最后一个元素(的下一位置)
            cur = first; 
        }
        --cur;                  // 切换至前一个元素
        return *this;
    }

    // 后置式, 标准写法
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 以下实现随机存取. 迭代器可以直接跳跃 n 个距离
    self& operator+=(difference_type n)
    {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            // 目标位置在同一缓冲区内
            cur += n;
        } else {
            // 目标位置不在同一缓冲区内
            difference_type node_offset = 
                offset > 0 ? offset / difference_type(buffer_size())
                    : -difference_type((-offset - 1) / buffer_size()) - 1;
            // 切换至正确的节点(亦即缓冲区)
            set_node(node + node_offset);
            // 切换至正确的元素
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    // 参考 More Effective C++, item22: Consider using op= instead of
    // stand-alone op
    self operator+(difference_type n) const
    {
        self tmp = *this;
        return tmp += n;    // 调用 operator+=
    }

    // 利用 operator+= 来完成 operator-=
    self& operator-=(difference_type n) { return *this += -n; }

    // 参考 More Effective C++, item22: Consider using op= instead of
    // stand-alone op
    self operator-(difference_type n) const
    {
        self tmp = *this;
        return tmp -= n;    // 调用 operator-=
    }

    // 以下调用 operator*, operator+实现随机存取, 迭代器可以直接跳跃 n 个距离
    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const
    {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }
};

// BufSize 默认值为 0 的唯一理由是为了闪避某些编译器在处理常数算式时的 bug
template <class T, class Alloc = alloc, size_t BufSiz = 0>
class deque {
public:                         // Basic types
    typedef T           value_type;
    typedef value_type* pointer;
    typedef size_t      size_type;
    typedef value_type& reference;
    typedef ptrdiff_t   difference_type;

public:
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;

public:                         // Basic accessors
    deque(int n, const value_type& value)
        : start(), finish(), map(0), map_size(0)
    {

    }

    iterator begin() { return start; }
    iterator end() { return finish; }

    reference operator[](size_type n)
    {
        return start[difference_type(n)];   // 调用 __deque_iterator<>::operator[]
    }

    reference front() { return *start; }    // 调用 __deque_iterator<>::operator*
    reference back()
    {
        iterator tmp = finish;
        --tmp;          // __deque_iterator<>::operator--
        return *tmp;    // __deque_iterator<>::operator*
        // 以上为何不改为: return *(finish - 1);
        // 因为 __deque_iterator<> 没有为 (finish - 1) 定义运算子
    }

    size_type size() const { return finish - start; }       // 调用__deque_iterator<>::operator-
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return finish == start; }

protected:                      // Internal typedefs
    // 元素的指针的指针
    typedef pointer* map_pointer;

    // 专属之空间配置器, 每次配置一个元素大小
    typedef simple_alloc<value_type, Alloc> data_allocator;
    // 专属之空间配置器, 每次配置一个指针大小
    typedef simple_alloc<pointer, Alloc> map_allocator;

protected:                      // Data members
    iterator start;         // 第一个节点
    iterator finish;        // 最后一个节点
    map_pointer map;        // 指向 map, map 是块连续空间, 其内的每个元素
                            // 都是一个指针(称为节点), 指向一块缓冲区
    size_type map_size;     // map 内可容纳多少指针

protected:                      // Internal construction/destruction
    enum { initial_map_size = 8 };

    void fill_initialize(size_type n, const value_type& value);

    // 负责产生并安排好 deque 的结构:
    void create_map_and_nodes(size_type num_elements);
    
};

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value)
{

}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
    // 需要节点数 = (元素个数 / 每个缓冲区可容纳的元素个数) + 1
    // 如果刚好整除, 会多配一个节点
    size_type num_nodes = num_elements / __deque_buf_size(BufSize, sizeof(T)) + 1;

    // 一个 map 要管理几个节点. 最少 8 个, 最多是 "所需节点数加 2"
    // (前后各预留一个, 扩充时可用)
    map_size = std::max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);
    // 以上配置出一个 "具有 map_size 个节点" 的 map

    // 以下令 nstart 和 nfinish 指向 map 所拥有之全部节点的最中央区段
    // 保持在最中央, 可使头尾两端的扩充能力一样大. 每个节点可对应一个缓冲区
    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    __STL_TRY {
        // 为 map 内的每个现用节点配置缓冲区. 所以缓冲区加起来就是 deque 的
        // 可用空间(最后一个缓冲区可能留有一些余裕)
        for (cur = nstart; cur <= nfinish; ++cur) {
            *cur = allocate_node();
        }
    } catch (...) {
        // "commit or rollback" 语意: 若非全部成功, 就一个都不留
    }

    // 为 deque 内部的两个迭代器 start 和 end 设定正确内容
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;        // first, cur 都是 public
    finish.cur = finish.first + num_elements % __deque_buf_size(BufSize, sizeof(T));
    // 前面说过, 如果刚好整除, 会多配一个节点
    // 此时即令 cur 指向这多配的一个节点(所对映之缓冲区)的起始处
}

#endif
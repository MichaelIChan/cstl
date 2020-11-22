#ifndef __STL_DEQUE_H
#define __STL_DEQUE_H

#include "stl_config.h"
#include "stl_iterator.h"
#include "stl_alloc.h"

namespace cstl
{

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
    typedef reverse_iterator<iterator>          reverse_iterator;

public:                         // Basic accessors

    deque(int n, const value_type& value)
        : start(), finish(), map(0), map_size(0)
    {
        fill_initialize(n, value);
    }

    iterator begin() { return start; }
    iterator end() { return finish; }
    reverse_iterator rbegin() { return reverse_iterator(finish); }
    reverse_iterator rend() { return reverse_iterator(start); }

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

    void push_back(const value_type& t)
    {
        if (finish.cur != finish.last - 1) {
            // 最后缓冲区尚有两个(含)以上的元素备用空间
            construct(finish.cur, t);   // 直接在备用空间上构造元素
            ++finish.cur;               // 调整最后缓冲区的使用状态
        } else {
            // 最后缓冲区只剩一个元素备用空间
            push_back_aux(t);
        }
    }

    void push_front(const value_type& t)
    {
        if (start.cur != start.first) {
            // 第一缓冲区尚有备用空间
            construct(start.cur - 1, t);    // 直接在备用空间上构造元素
            --start.cur;                    // 调整第一缓冲区的使用状态
        } else {
            // 第一缓冲区已无备用空间
            push_back_aux(t);
        }
    }

    void pop_back()
    {
        if (finish.cur != finish.first) {
            // 最后缓冲区有一个(或更多)元素
            --finish.cur;           // 调整指针, 相当于排除了最后元素
            destory(finish.cur);    // 将最后元素析构
        } else {
            // 最后缓冲区没有任何元素
            pop_back_aux();         // 这里将进行缓冲区的释放工作
        }
    }

    void pop_front()
    {
        if (start.cur != start.last - 1) {
            // 第一缓冲区有两个(或更多)元素
            destory(start.cur);     // 将第一元素析构
            ++start.cur;            // 调整指针, 相当于排除了第一元素
        } else {
            // 第一缓冲区仅有一个元素
            pop_front_aux();        // 这里将进行缓冲区的释放工作
        }
    }

    // 清除整个 deque, 注意, 最终需要保留一个缓冲区.
    // 这是 deque 的策略, 也是 deque 的初始状态
    void clear();

    // 清除 pos 所指的元素. pos 为清除点
    iterator erase(iterator pos);

    // 清除 [first, last) 区间内的所有元素
    iterator erase(iterator first, iterator last);

    // 在 position 处插入一个元素, 其值为 x
    iterator insert(iterator position, const value_type& x)
    {
        if (position.cur == start.cur) {            // 如果插入点是 deque 最前端
            push_front(x);  // 交给 push_front 去做
            return start;
        } else if (position.cur == finish.cur) {    // 如果插入点是 deque 最尾端
            push_back(x);   // 交给 push_back 去做
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else {
            return insert_aux(position, x);         // 交给 insert_aux 去做
        }
    }
    

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

    T* allocate_node();
    void deallocate_node(T*);

    void reserve_map_at_back(size_type nodes_to_add = 1)
    {
        if (nodes_to_add + 1 > map_size - (finish.node - map)) {
            // 如果 map 尾端的节点备用空间不足
            // 符合以上条件则必须重换一个 map (配置更大的, 拷贝原来的, 释放原来的)
            reallocate_map(nodes_to_add, false);
        }
    }
    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        if (nodes_to_add > start.node - map) {
            // 如果 map 前端的节点备用空间不足
            // 符合以上条件则必须重换一个 map (配置更大的, 拷贝原来的, 释放原来的)
            reallocate_map(nodes_to_add, true);
        }
    }

    void fill_initialize(size_type n, const value_type& value);

    // 负责产生并安排好 deque 的结构:
    void create_map_and_nodes(size_type num_elements);

    // 只有当 finish.cur == finish.last - 1 时才会被调用
    // 也就是说, 只有当最后一个缓冲区只剩一个备用元素空间时才会被调用
    void push_back_aux(const value_type& t);

    // 只有当 start.cur == start.last 时才会被调用
    // 也就是说, 只有当第一个缓冲区没有任何备用元素时才会被调用
    void push_front_aux(const value_type& t);

    // 只有当 finish.cur == finish.first 时才会被调用
    void pop_back_aux();

    // // 只有当 start.cur == start.last - 1 时才会被调用
    void pop_front_aux();

    iterator insert_aux(iterator pos, const value_type& x);

    void reallocate_map(size_type nodes_to_add, bool add_at_front);
};

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value)
{
    create_map_and_nodes(n);    // 把 deque 的结构都产生并安排好
    map_pointer cur;
    __STL_TRY {
        // 为每个节点缓冲区设定初值
        for (cur = start.node; cur < finish.node; ++cur) {
            uninitialized_fill(*cur, *cur + __deque_buf_size(BufSize, sizeof(T)), value);
        }
        // 最后一个节点的设定稍有不同(因为尾端可能有备用空间, 不必设初值)
        uninitialized_fill(finish.first, finish.cur, value);
    } catch(...) {

    }
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

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t)
{
    value_type t_copy = t;
    reserve_map_at_back();      // 若符合某种条件则必须重换一个 map
    *(finish.node + 1) = allocate_node();   // 配置一个新节点(缓冲区)
    __STL_TRY {
        construct(finish.cur, t_copy);      // 针对标的元素设值
        finish.set_node(finish.node + 1);   // 改变 finish, 令其指向新节点
        finish.cur = finish.first;          // 设定 finish 的状态
    } 
    __STL_UNWIND(deallocate_node(*(finish.node + 1)));
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t)
{
    value_type t_copy = t;
    reserve_map_at_front();     // 若符合某种条件则必须重换一个 map
    *(start.node - 1) = allocate_node();    // 配置一个新节点(缓冲区)
    __STL_TRY {
        start.set_node(start.node - 1);     // 改变 start, 令其指向新节点
        start.cur = start.last - 1;         // 设定 start 的状态
        construct(start.cur, t_copy);       // 针对标的元素设值
    } catch(...) {
        // "commit or rollback" 语意: 若非全部成功, 就一个都不留
        start.set_node(start.node + 1);
        start.cur = start.first;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front)
{
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_nstart;
    if (map_size > 2 * new_num_nodes) {
        new_nstart = map + (map - new_num_nodes / 2)
                         + (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node) {
            copy(start.node, finish.node + 1, new_nstart);
        } else {
            copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
    } else {
        size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
        // 配置一块空间, 准备给新 map 使用
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                             + (add_at_front ? nodes_to_add : 0);
        // 把原 map 内容拷贝过来
        copy(start.node, finish.node + 1, new_nstart);
        // 释放原 map
        map_allocator::deallocate(map, map_size);
        // 设定新 map 的起始地址与大小
        map = new_map;
        map_size = new_map_size;
    }

    // 重新设定迭代器 start 和 finish
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux()
{
    deallocate_node(finish.first);      // 释放最后一个缓冲区
    finish.set_node(finish.node - 1);   // 调整 finish 的状态, 使指向
    finish.cur = finish.last - 1;       // 上一个缓冲区的最后一个元素
    destroy(finish.cur);                // 将该元素析构
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux()
{
    destory(start.cur);                 // 将第一缓冲区的第一个(也是最后一个, 唯一一个)元素析构
    deallocate_node(start.first);       // 释放第一缓冲区
    start.set_node(start.node + 1);     // 调整 start 的状态, 使指向
    start.cur = start.first;            // 下一个缓冲区的第一个元素
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear()
{
    // 以下针对头尾以外的每一个缓冲区
    for (map_pointer node = start.node + 1; node < finish.node; ++node) {
        // 将缓冲区内的所有元素析构. 注意, 调用的是 destory() 第二版本
        destory(*node, *node + __deque_buf_size(BufSize, sizeof(T)));
        // 释放缓冲区内存
        data_allocator::deallocate(*node, __deque_buf_size(BufSize, sizeof(T)));
    }

    if (start.node != finish.node) {    // 至少有头尾两个缓冲区
        destory(start.cur, start.last);     // 将头缓冲区的目前所有元素析构
        destory(finish.first, finish.cur);  // 将尾缓冲区的目前所有元素析构
        // 以下释放尾缓冲区, 头缓冲区保留
        data_allocator::deallocate(finish.first, __deque_buf_size(BufSize, sizeof(T)));
    } else {    // 只有一个缓冲区
        destory(start.cur, finish.cur);     // 将此唯一缓冲区内的所有元素析构
        // 并不释放缓冲区, 保留这唯一的缓冲区
    }

    finish = start; // 调整状态
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator pos)
{
    iterator next = pos;
    ++next;
    difference_type index = pos - start;    // 清除点之前的元素个数
    if (index < (size() >> 1)) {            // 如果清除点之前的元素比较少
        copy_backward(start, pos, next);    // 就移动清除点之前的元素
        pop_front();                        // 移动完毕, 最前一个元素冗余, 去除之
    } else {                        // 清除点之后点元素比较少
        copy(next, finish, pos);    // 就移动清除点之后的元素
        pop_back();                 // 移动完毕, 最后一个元素冗余, 去除之
    }
    return start + index;
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
{
    if (first == start && last == finish) {
        // 如果清除区间就是整个 deque, 直接调用 clear() 即可
        clear();
        return finish;
    } else {
        difference_type n = last - first;               // 清除区间的长度
        difference_type elems_before = first - start;   // 清除区间前方的元素个数
        if (elems_before < (size() - n) / 2) {          // 如果前方的元素比较少
            copy_backword(start, first, last);          // 向后移动前方元素(覆盖清除区间)
            iterator new_start = start + n;             // 标记 deque 的新起点
            destory(start, new_start);                  // 移动完毕, 将冗余的元素析构

            // 以下将冗余的缓冲区释放
            for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
                data_allocator::deallocate(*cur, __deque_buf_size(BufSize, sizeof(T)));
            }
            start = new_start;  // 设定 deque 的新起点
        } else {    // 如果清除区间后方的元素比较少
            copy(last, finish, first);          // 向前移动后方元素(覆盖清除区间)
            iterator new_finish = finish - n;   // 标记 deque 的新尾点
            destory(new_finish, finish);        // 移动完毕, 将冗余的元素析构

            // 以下将冗余的缓冲区释放
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
                data_allocator::deallocate(*cur, __deque_buf_size(BufSize, sizeof(T)));
            }
            finish = new_finish;  // 设定 deque 的新尾点
        }
        return start + elems_before;
    }
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x)
{
    difference_type index = pos - start;    // 插入点之前的元素个数
    value_type x_copy = x;
    if (index < size() / 2) {       // 如果插入点之前的元素个数比较少
        push_front(front());        // 在最前端加入与第一元素同值的元素
        iterator front1 = start;    // 以下标示记号, 然后进行元素移动
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        +pos1;
        copy(front2, pos1, front1); // 元素移动
    } else {                        // 插入点之后的元素个数比较少
        push_back(back());          // 在最尾端加入与最后元素同值的元素
        iterator back1 = finish;    // 以下标示记号, 然后进行元素移动
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + index;
        copy_backward(pos, back2, back1);   // 元素移动
    }
    *pos = x_copy;      // 在插入点上设定新值
    return pos;
}

} // namespace cstl

#endif /* __STL_DEQUE_H */
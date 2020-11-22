#ifndef __STL_LIST_H
#define __STL_LIST_H

#include <algorithm>

#include "stl_iterator.h"
#include "stl_alloc.h"

namespace cstl
{

template <class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer prev;      // 型别为 void*. 其实可设为 __list_node<T>*
    void_pointer next;
    T data;
};

// list 迭代器
template <class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*>   iterator;
    typedef __list_iterator<T, Ref, Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;     // 迭代器内部要有一个普通指针, 指向 list 的节点

    // constructor
    __list_iterator(link_type x) : node(x) { }
    __list_iterator() { }
    __list_iterator(const iterator& x) : node(x.node) { }

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
    // 以下是对迭代器取值, 取的是节点的数据值
    reference operator*() const { return (*node).data; }

    // 以下是迭代器的成员存取运算子的标准做法
    pointer operator->() const { return &(operator*()); }

    // 对迭代器累加 1, 就是前进一个节点
    self& operator++()      // 前置
    {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int)    // 后置
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    // 对迭代器递减 1, 就是后退一个节点
    self& operator--()      // 前置
    {
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int)    // 后置
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template <class T, class Alloc = alloc>
class list {
protected:
    typedef __list_node<T> list_node;
    // 专属之空间配置器, 每次配置一个节点大小
    typedef simple_alloc<list_node, Alloc> list_node_allocator;
public:
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef reverse_iterator<iterator>             reverse_iterator;

    typedef T                 value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef list_node*        link_type;

protected:
    link_type node;     // 只要一个指针, 便可表示整个环状双向链表

public:
    list() { empty_initialize(); }

    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    bool empty() const { return node->next == node; }
    size_type size() const
    {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }
    // 取头节点的内容(元素值)
    reference front() { return *begin(); }
    reference back() { return *(--end()); }

    void push_front(const T& x) { insert(begin(), x); }     // 插入头节点
    void push_back(const T& x) { insert(end(), x); }        // 插入尾节点

    // 移除迭代器 position 所指节点
    iterator erase(iterator position)
    {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }

    // 移除头节点
    void pop_front() { erase(begin()); }
    // 移除尾节点
    void pop_back()
    {
        iterator tmp = end();
        erase(--tmp);
    }

    // 将 x 接合于 position 所指位置之前. x 必须不同于 *this
    void splice(iterator position, list& x)
    {
        if (!x.empty()) {
            transfer(position, x.begin(), x.end());
        }
    }

    // 将 i 所指元素接合于 position 所指位置之前. position 和 i 可指向同一个 list
    void splice(iterator position, list&, iterator i)
    {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }

    // 将 [first, last) 内的所有元素接合于 position 所指位置之前
    // position 和 [first, last) 可指向同一个 list,
    // 但 position 不能位于 [first, last) 之内
    void splice(iterator position, list&, iterator first, iterator last)
    {
        if (first != last) {
            transfer(position, first, last);
        }
    }

    void swap(list<T, Alloc>& x) { std::swap(node, x.node); }

    void clear();                   // 清除所有节点
    void remove(const T& value);    // 将数值为 value 之所有元素移除
    void unique();                  // 移除数值相同的连续元素
    void merge(list<T, Alloc>& x);  // 将 x 合并到 *this 身上. 两个 list 必须是递增的
    void reverse();                 // 将 *this 的内容逆向重置
    void sort();                    // list 不能使用 STL 算法 sort(), 必须使用自己的 sort()
                                    // 因为 STL 算法 sort() 只接受 RamdonAccessIterator
                                    // 本函数采用 quick sort


protected:
    // 配置一个节点并传回
    link_type get_node() { return list_node_allocator::allocate(); }
    // 释放一个节点
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    // 产生(配置并构造)一个节点, 带有元素值
    link_type create_node(const T& x)
    {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }
    // 销毁(析构并释放)一个节点
    void destroy_node(link_type p)
    {
        destory(&p->data);
        put_node(p);
    }

    void empty_initialize()
    {
        node = get_node();  // 配置一个节点空间, 令 node 指向他
        node->next = node;  // 令 node 头尾都指向自己, 不设元素值
        node->prev = node;
    }

    iterator insert(iterator position, const T& x)
    {
        link_type tmp = create_node(x);
        // 调整双向指针, 插入 tmp
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }

    // 将 [first, last) 内的所有元素移动到 position 之前
    void transfer(iterator position, iterator first, iterator last)
    {
        if (position != last) {
            (*(link_type((*last.node).prev))).next = position.node;
            (*(link_type((*first.node).prev))).next = last.node;
            (*(link_type((*position.node).prev))).next = first.node;
            link_type tmp = link_type((*position.node).prev);
            (*position.node).prev = (*last.node).prev;
            (*last.node).prev = (*first.node).prev;
            (*first.node).prev = tmp;
        }
    }
};

template <class T, class Alloc>
void list<T, Alloc>::clear()
{
    link_type cur = (link_type)node->next;
    while (cur != node) {   // 遍历每一个节点
        link_type tmp = cur;
        cur = (link_type)cur->next;
        destroy_node(tmp);
    }
    // 恢复 node 原始状态
    node->next = node;
    node->prev = node;
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value)
{
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::unique()
{
    iterator first = begin();
    iterator last = end();
    if (first == last) return;      // 空链表, 什么都不做
    iterator next = first;
    while (++next != last)
    {
        if (*first == *next) erase(next);
        else first = next;
        next = first;
    } 
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x)
{
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();

    // 注意, 两个 list 都已经经过递增排序
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        } else ++first1;
    }
    if (first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc>
void list<T, Alloc>::reverse()
{
    // 以下判断, 如果是空链表, 或仅有一个元素, 就不进行任何操作
    // 使用 size() == 0 || size() == 1 来判断虽然也可以, 但是比较慢
    if (node->next == node || link_type(node->next)->next == node) return;

    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        transfer(begin(), old, first);
    }
}

template <class T, class Alloc>
void list<T, Alloc>::sort()
{
    if (node->next == node || link_type(node->next)->next == node) return;

    // 一些新的 list, 作为中介数据存放区
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }

    for (int i = 1; i < fill; ++i) {
        counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
}

} // namespace cstl

#endif /* __STL_LIST_H */
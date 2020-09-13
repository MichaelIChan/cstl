#ifndef __STL_PRIORITY_QUEUE_H
#define __STL_PRIORITY_QUEUE_H

#include <functional>
#include <algorithm>

#include "../src/stl_vector.h"
// #include "../src/stl_heap.h"

template <class T, class Sequence = vector<T>,
          class Compare = std::less<typename Sequence::value_type> >
class priority_queue {
public:
    typedef typename Sequence::value_type      value_type;
    typedef typename Sequence::size_type       size_type;
    typedef typename Sequence::reference       reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c;     // 底层容器
    Compare comp;   // 元素大小比较标准
public:
    priority_queue() : c() { }
    explicit priority_queue(const Compare& x) : c(), comp(x) { }

    // 以下用到的 make_heap(), push_heap(), pop_heap() 都是泛型算法
    // 注意, 任何一个构造函数都立刻于底层容器内产生一个 implicit representation heap
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x)
        : c(first, last), comp(x) { std::make_heap(c.begin(), c.end(), comp); }
    
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last)
        : c(first, last) { std::make_heap(c.begin(), c.end(), comp); }
    
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    const_reference top() const { return c.front(); }
    void push(const value_type& x)
    {
        __STL_TRY {
            // push_heap 是泛型算法, 先利用底层容器的 push_back() 将新元素推入末端, 再重排 heap
            c.push_back(x);
            std::push_heap(c.begin(), c.end(), comp);
        }
        __STL_UNWIND(c.clear());
    }
    void pop()
    {
        __STL_TRY {
            // pop_heap 是泛型算法, 从 heap 内取出一个元素
            // 它并不是真正将元素弹出, 而是重排 heap,
            // 然后再以底层容器的 pop_back() 取得被弹出的元素
            std::pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }
        __STL_UNWIND(c.clear());
    }
};

#endif
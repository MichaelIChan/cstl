#ifndef __STL_SET_H
#define __STL_SET_H

#include <utility>

#include "../src/stl_function.h"
#include "../src/stl_alloc.h"
#include "../src/stl_tree.h"

template <class Key, class Compare = std::less<Key>, class Alloc = alloc>
class set {
public:     // typedefs
    typedef Key key_type;
    typedef Key value_type;
    // 注意, 以下 key_compare 和 value_compare 使用同一个比较函数
    typedef Compare key_compare;
    typedef Compare value_compare;
private:
    // 以下的 identity 定义于 <stl_function.h>, 见第七章, 其定义为:
    /*
        template <class T>
        struct identity : public unary_function<T, T> {
            const T& operator()(const T& x) const { return x; }
        };
    */
   typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
   rep_type t;      //  采用红黑树(RB-tree)来表现 set
public:
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;
    // 注意上一行, iterator 定义为 RB-tree 的 const_iterator, 这表示 set 的迭代器无法执行写入操作.
    // 这是因为 set 的元素有一定次序安排, 不允许用户在任意处进行写入
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::const_reverse_iterator reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    // allocation/deallocation
    // 注意, set 一定使用 RB-tree 的 insert_unique() 而非 insert_equal()
    // multiset 才使用 RB-tree 的 insert_equal()
    // 因为 set 不允许相同键值存在, multiset 才允许相同键值存在
    set() : t(Compare()) { }
    explicit set(const Compare& comp) : t(comp) { }

    // insert_unique(iterator&, iterator&) has not been finished
    template <class InputIterator>
    set(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_unique(first, last); }
    
    template <class InputIterator>
    set(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_unique(first, last); }
    
    set(const set<Key, Compare, Alloc>& x) : t(x.t) { }
    set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x)
    {
        t = x.t;
        return *this;
    }

    // this is not been finished
};

#endif
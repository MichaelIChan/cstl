#ifndef __STL_MAP_H
#define __STL_MAP_H

#include <algorithm>

#include "../src/stl_function.h"
#include "../src/stl_pair.h"
#include "../src/stl_tree.h"

template <class Key, class T, class Compare = std::less<Key>, class Alloc = alloc>
class map {
public:
    // typedefs:
    typedef Key key_type;          // 键值型别
    typedef T data_type;            // 数据(实值)型别
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;  // 元素型别(键值/实值)
    typedef Compare key_compare;    // 键值比较函数

    // 以下定义一个 functor, 其作用就是调用"元素比较函数"
    class value_compare : public binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare, Alloc>;
    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) { }
    public:
        bool operator() (const value_type& x, const value_type& y) const
        {
            return comp(x.first, y.first);
        }
    };

private:
    // 以下定义表述型别(representation type). 以 map 元素型别(一个pair)的第一型别
    // 作为 RB-tree 节点的键值型别
    typedef rb_tree<key_type, value_type,
                    select1st<value_type>, key_compare, Alloc> rep_type;
    rep_type t;

public:
    typedef typename rep_type::pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::iterator iterator;
    // 注意上一行, map 并不像 set 一样将 iterator 定义为 RB-tree 的 const_iterator.
    // 因为它允许用户通过其迭代器修改元素的实值(value)
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::reverse_iterator reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    // allocation/deallocation
    // 注意, map 一定使用底层 RB-tree 的 insert_unique() 而非 insert_equal()
    // multimap 才使用 insert_equal()
    // 因为 map 不允许相同键值存在, multimap 才允许相同键值存在

    map() : t(Compare()) { }
    explicit map(const Compare& comp) : t(comp) { }

    template <class InputIterator>
    map(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_unique(first, last); }
    
    template <class InputIterator>
    map(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_unique(first, last); }

    map(const map<Key, T, Compare, Alloc>& x) : t(x.t) { }
    map<Key, T, Compare, Alloc>& operator=(const map<Key, T, Compare, Alloc>& x)
    {
        t = x.t;
        return *this;
    }

    // accessors:
    // 以下所以的 map 操作行为, RB-tree 都已提供, map 只要转调用即可

    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return value_compare(t.key_comp()); }
    iterator begin() { return t.begin(); }
    const_iterator begin() const { return t.begin(); }
    iterator end() { return t.end(); }
    const_iterator end() const { return t.end(); }
    reverse_iterator rbegin() { return t.rbegin(); }
    const_reverse_iterator rbegin() const { return t.rbegin(); }
    reverse_iterator rend() { return t.rend(); }
    const_reverse_iterator rend() const { return t.rend(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    T& operator[](const key_type& k)
    {
        iterator i = lower_bound(k);
        // i->first is greater than or equivalent to __k.
        if (i == end() || key_comp()(k, (*i).first)) {
            i = insert(i, value_type(k, T()));
        }
        return (*i).second;
    }

    // insert/erase

    pair<iterator, bool> insert(const value_type& x)
    {
        return t.insert_unique(x);
    }
    iterator insert(iterator position, const value_type& x)
    {
        return t.insert_unique(position, x);
    }
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        t.insert_unique(first, last);
    }

    void erase(iterator position) { t.erase(position); }
    size_type erase(const key_type& x) { return t.erase(x); }
    void erase(iterator first, iterator last) { t.erase(first, last); }
    void clear() { t.clear(); }

    // map operations:
    // TODO
};

#endif
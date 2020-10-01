#ifndef __STL_MULTISET_H
#define __STL_MULTISET_H

#include "../src/stl_function.h"
#include "../src/stl_alloc.h"
#include "../src/stl_tree.h"

template <class Key, class Compare = less<Key>, class Alloc = alloc>
class multiset {
public:
    // typedefs:
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
   rep_type t;      //  采用红黑树(RB-tree)来表现 multiset
public:
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;
    // 注意上一行, iterator 定义为 RB-tree 的 const_iterator, 这表示 multiset 的迭代器无法执行写入操作.
    // 这是因为 multiset 的元素有一定次序安排, 不允许用户在任意处进行写入
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::const_reverse_iterator reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    // allocation/deallocation
    // 注意, multiset 一定使用 insert_equal() 而不使用 insert_unique()
    // set 才使用 insert_unique()

    multiset() : t(Compare()) { }
    explicit multiset(const Compare& comp) : t(comp) { }

    template <class InputIterator>
    multiset(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_equal(first, last); }
    template <class InputIterator>
    multiset(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_equal(first, last); }

    multiset(const multiset<Key, Compare, Alloc>& x) : t(x.t) { }
    multiset<Key, Compare, Alloc>& operator=(const multiset<Key, Compare, Alloc>& x)
    {
        t = x.t;
        return *this;
    }

    // 以下所有的 multiset 操作行为, RB-tree 都已提供, 所以 multiset 只要传递调用即可

    // accessors;
    key_compare key_comp() const { return t.key_comp(); }
    // 以下注意, multiset 的 value_comp() 事实上为 RB-tree 的 key_comp()
    value_compare value_comp() const { return t.key_comp(); }
    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    reverse_iterator rbegin() const { return t.rbegin(); }
    reverse_iterator rend() const { return t.rend(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }
    void swap(multiset<Key, Compare, Alloc>& x) { t.swap(x.t); }

    // insert/erase
    iterator insert(const value_type& x)
    {
        return t.insert_equal(x);
    }
    iterator insert(iterator position, const value_type& x)
    {
        typedef typename rep_type::iterator rep_iterator;
        return t.insert_equal((rep_iterator&)position, x);
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        t.insert_equal(first, last);
    }

    void erase(iterator position)
    {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator&)position);
    }
    size_type erase(const key_type& x)
    {
        return t.erase(x);
    }
    void erase(iterator first, iterator last)
    {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator&)first, (rep_iterator&)last);
    }
    void clear() { t.clear(); }

    // multiset operations
    iterator find(const key_type& x) const { return t.find(x); }
    size_type count(const key_type& x) const { return t.count(x); }
    iterator lower_bound(const key_type& x) const
    {
        return t.lower_bound(x);
    }
    iterator upper_bound(const key_type& x) const
    {
        return t.upper_bound(x);
    }
    pair<iterator, iterator> equal_range(const key_type& x) const
    {
        return t.equal_range(x);
    }
    // 以下的 __STL_NULL_TMPL_ARGS 被定义为 <>, 详见 1.9.1 节
    friend bool operator== __STL_NULL_TMPL_ARGS (const multiset&, const multiset&);
    friend bool operator< __STL_NULL_TMPL_ARGS (const multiset&, const multiset&);
};

template <class Key, class Compare, class Alloc>
inline bool operator==(const multiset<Key, Compare, Alloc>& x,
                       const multiset<Key, Compare, Alloc>& y)
{
    return x.t == y.t;
}

template <class Key, class Compare, class Alloc>
inline bool operator<(const multiset<Key, Compare, Alloc>& x,
                      const multiset<Key, Compare, Alloc>& y)
{
    return x.t < y.t;
}

#endif
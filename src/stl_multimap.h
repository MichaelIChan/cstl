#ifndef __STL_MULTIMAP_H
#define __STL_MULTIMAP_H

#include "../src/stl_function.h"
#include "../src/stl_alloc.h"
#include "../src/stl_tree.h"

template <class Key, class T, class Compare = less<Key>, class Alloc = alloc>
class multimap {
public:
    // typedefs
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
   rep_type t;      //  采用红黑树(RB-tree)来表现 multimap
public:
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;
    // 注意上一行, iterator 定义为 RB-tree 的 const_iterator, 这表示 multimap 的迭代器无法执行写入操作.
    // 这是因为 multimap 的元素有一定次序安排, 不允许用户在任意处进行写入
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::const_reverse_iterator reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    // allocation/deallocation
    // 注意, multimap 一定使用底层 RB-tree 的 insert_equal() 而非 insert_unique()
    // map 才使用 insert_unique()

    multimap() : t(Compare()) { }
    explicit multimap(const Compare& comp) : t(comp) { }

    template <class InputIterator>
    multimap(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_equal(first, last); }

    template <class InputIterator>
    multimap(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_equal(first, last); }
    
    multimap(const multimap<Key, T, Compare, Alloc>& x) : t(x.t) { }
    multimap<Key, T, Compare, Alloc>& operator=(const multimap<Key, T, Compare, Alloc>& x)
    {
        t = x.t;
        return *this;
    }

    // accessors:
    // 以下所以的 multimap 操作行为, RB-tree 都已提供, multimap 只要转调用即可

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

    iterator insert(const value_type& x) { return t.insert_equal(x); }
    iterator insert(iterator position, const value_type& x)
    {
        return t.insert_equal(position, x);
    }
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        t.insert_equal(first, last);
    }

    void erase(iterator position) { t.erase(position); }
    size_type erase(const key_type& x) { return t.erase(x); }
    void erase(iterator first, iterator last) { t.erase(first, last); }
    void clear() { t.clear(); }

    // multimap operations:
    
    iterator find(const key_type& x) { return t.find(x); }
    const_iterator find(const key_type& x) const { return t.find(x); }
    size_type count(const key_type& x) const { return t.count(x); }
    iterator lower_bound(const key_type& x) { return t.lower_bound(x); }
    const_iterator lower_bound(const key_type& x) const
    {
        return t.lower_bound(x);
    }

    pair<iterator, iterator> equal_range(const key_type& x)
    {
        return t.equal_range(x);
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& x) const
    {
        return t.equal_range(x);
    }
    friend bool operator== __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
    friend bool operator< __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
};

template <class Key, class T, class Compare, class Alloc>
inline bool operator==(const multimap<Key, T, Compare, Alloc>& x,
                       const multimap<Key, T, Compare, Alloc>& y)
{
    return x.t == y.t;
}

template <class Key, class T, class Compare, class Alloc>
inline bool operator<(const multimap<Key, T, Compare, Alloc>& x,
                      const multimap<Key, T, Compare, Alloc>& y)
{
    return x.t < y.t;
}

#endif
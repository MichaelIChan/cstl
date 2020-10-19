#ifndef __STL_HASH_SET_H
#define __STL_HASH_SET_H

#include "../src/stl_hashtable.h"
#include "../src/stl_hash_fun.h"
#include "../src/stl_function.h"
#include "../src/stl_alloc.h"

template <class Value,
          class HashFcn = hash<Value>,
          class EqualKey = equal_to<Value>,
          class Alloc = alloc>
class hash_set {
private:
    // 以下使用的 identity<> 定义于 <stl_function.h> 中
    typedef hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef typename ht::key_type key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;

    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;

    typedef typename ht::const_iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    // 缺省使用大小为 100 的表格. 将被hash table 调整为最接近且较大的质数
    hash_set() : rep(100, hasher(), key_equal()) { }
    explicit hash_set(size_type n) : rep(n ,hasher(), key_equal()) { }
    hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) { }
    hash_set(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) { }

    // 以下, 插入操作全部使用 insert_unique(), 不允许键值重复
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l)
        : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n)
        : ht(n, hasher(), key_equal()) { ht.insert_unique(f, l); }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n, const hasher& hf)
        : ht(n, hf, key_equal()) { ht.insert_unique(f, l); }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n,
             const hasher& hf, const key_equal& eql,)
        : ht(n, hf, eql, a) { ht.insert_unique(f, l); }

public:
    size_type size() const { return ht.size(); }
    size_type max_size() const { return ht.max_size(); }
    bool empty() const { return ht.empty(); }
    void swap(hash_set& hs) { ht.swap(hs.ht); }
    friend bool operator== __STL_NULL_TMPL_ARGS (const hash_set&, const hash_set&);

    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.end(); }

public:
    pair<iterator, bool> insert(const value_type& obj)
    {
        pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
        retrun pair<iterator, bool>(p.first, p.second);
    }
    template <class InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_unique(f, l); }
    pair<iterator, bool> insert_noresize(const value_type& obj)
    {
        pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type& key) const { return rep.find(key); }

    size_type count(const key_type& key) const { return rep.count(key); }

    pair<iterator, iterator> equal_range(const key_type& key) const
    {
        return rep.equal_range(key);
    }

    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }

public:
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const
    {
        return rep.elems_in_bucket(n);
    }
};

template <class Value, class HashFcn, class EqualKey, class Alloc>
inline bool
operator==(const hash_set<class Value, class HashFcn, class EqualKey, class Alloc>& hs1,
           const hash_set<class Value, class HashFcn, class EqualKey, class Alloc>& hs2)
{
    return hs1.rep == hs2.rep;
}

#endif
#ifndef __STL_HASHTABLE_H
#define __STL_HASHTABLE_H

#include <algorithm>

#include "../src/stl_iterator.h"
#include "../src/stl_alloc.h"
#include "../src/stl_vector.h"

template <class Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value val;
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;

template <class Value, class _Key, class _HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
        hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
        iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn,
                                       ExtractKey, EqualKey, Alloc>
        const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;      // 迭代器目前所指的节点
    hashtable* ht;  // 保持对容器的连结关系(因为可能需要从 bucket 跳到 bucket)

    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) { }
    __hashtable_iterator() { }
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++();
    iterator operator++(int);
    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++()
{
    const node* old = cur;
    cur = cur->next;        // 如果存在, 就是它. 否则进入以下 if 流程
    if (!cur) {
        // 根据元素值, 定位出下一个 bucket. 其起头处就是我们的目的地
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size()) {
            cur = ht->buckets[bucket];
        }
    }
    return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
inline __hashtable_iterator<V, K, HF, ExK, EqK, A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
    iterator tmp = *this;
    ++*this;    // 调用 operator++()
    return tmp;
}

// hashtable 的模版参数
// Value: 节点的实值型别
// Key: 节点的键值型别
// HashFcn: hash function 的函数型别
// ExtractKey: 从节点中取出键值的方法(函数或仿函数)
// EqualKey: 判断键值相同与否的方法(函数或仿函数)
// Alloc: 空间配置器, 缺省使用 alloc
template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    typedef HashFcn  hasher;        // 为 template 型别参数重新定义一个别称
    typedef EqualKey key_equal;     // 为 template 型别参数重新定义一个别称
    typedef size_t   size_type;

private:
    // 以下三者都是 function objects.
    // <stl_hash_fun.h> 中定义有数个标准型别的 hasher
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    vector<node*, Alloc> buckets;
    size_type num_elements;

public:
    // bucket 个数即 buckets vector 的大小
    size_type bucket_count() const { return buckets.size(); }

    // 总共可以有多少 buckets
    size_type max_bucket_count() const
        { return __stl_prime_list[__stl_num_primes - 1]; }
};

// 注意: 假设 long 至少有 32 bits
static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
    1610612741ul, 3221225473ul, 4294967291ul
};

// 以下找出上述 28 个质数之中, 最接近并大于或等于 n 的那个质数
inline unsigned long __stl_next_prime(unsigned long n)
{
  const unsigned long* first = __stl_prime_list;
  const unsigned long* last = __stl_prime_list + (int)__stl_num_primes;
  const unsigned long* pos = std::lower_bound(first, last, n);
  // 使用 lower_bound(), 序列需先排序
  return pos == last ? *(last - 1) : *pos;
}

#endif
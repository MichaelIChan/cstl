#ifndef __STL_HASHTABLE_H
#define __STL_HASHTABLE_H

#include <algorithm>

#include "../src/stl_hash_fun.h"
#include "../src/stl_iterator.h"
#include "../src/stl_alloc.h"
#include "../src/stl_vector.h"
#include "../src/stl_pair.h"

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

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator {
  typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
          hashtable;
  typedef __hashtable_iterator<Value, Key, HashFcn, 
                               ExtractKey, EqualKey, Alloc>
          iterator;
  typedef __hashtable_const_iterator<Value, Key, HashFcn, 
                                     ExtractKey, EqualKey, Alloc>
          const_iterator;
  typedef __hashtable_node<Value> node;

  typedef forward_iterator_tag iterator_category;
  typedef Value value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef const Value& reference;
  typedef const Value* pointer;

  const node* cur;
  const hashtable* ht;

  __hashtable_const_iterator(const node* n, const hashtable* tab)
    : cur(n), ht(tab) {}
  __hashtable_const_iterator() {}
  __hashtable_const_iterator(const iterator& it) 
    : cur(it.cur), ht(it.ht) {}
  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(operator*()); }

  const_iterator& operator++();
  const_iterator operator++(int);
  bool operator==(const const_iterator& it) const { return cur == it.cur; }
  bool operator!=(const const_iterator& it) const { return cur != it.cur; }
};

template <class Value, class Key, class HF, class ExK, class EqK, class A>
__hashtable_const_iterator<Value, Key, HF, ExK, EqK, A>&
__hashtable_const_iterator<Value, Key, HF, ExK, EqK, A>::operator++()
{
  const _Node* old = cur;
  cur = cur->next;
  if (!cur) {
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size())
      cur = ht->buckets[bucket];
  }
  return *this;
}

template <class Value, class Key, class HF, class ExK, class EqK, class A>
inline __hashtable_const_iterator<Value, Key, HF, ExK, EqK, A>
__hashtable_const_iterator<Value, Key, HF, ExK, EqK, A>::operator++(int)
{
  const_iterator tmp = *this;
  ++*this;
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
  typedef __hashtable_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc>
          iterator;
  typedef __hashtable_const_iterator<Val, Key, HashFcn, ExtractKey, EqualKey,
                                     Alloc>
          const_iterator;

public:
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
    {
        initialize_buckets(n);
    }

public:
    // bucket 个数即 buckets vector 的大小
    size_type bucket_count() const { return buckets.size(); }

    // 总共可以有多少 buckets
    size_type max_bucket_count() const
        { return __stl_prime_list[__stl_num_primes - 1]; }

    // 在不需重建表格的情况下插入新节点. 键值不允许重复
    pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    
    // 插入元素, 不允许重复
    pair<iterator, bool> insert_unique(const value_type& obj)
    {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    // 判断是否需要重建表格. 如果不需要, 立即返回. 如果需要, 则进一步处理
    void resize(size_type num_elements_hint);

private:
    size_type bkt_num_key(const key_type& key) const
    {
        return bkt_num_key(key, buckets.size());
    }

    size_type bkt_num(const value_type& obj) const
    {
        return bkt_num_key(get_key(obj));
    }

    size_type bkt_num_key(const key_type& key, size_t n) const
    {
        return hash(key) % n;
    }

    size_type bkt_num(const value_type& obj, size_t n) const
    {
        return bkt_num_key(get_key(obj), n);
    }

    node* new_node(const value_type& obj)
    {
        node* n = node_allocator::allocate();
        n->next = 0;
        __STL_TRY {
            construct(&n->val, obj);
            return n;
        }
        __STL_UNWIND(node_allocator::deallocate(n));
    }

    void delete_node(node* n)
    {
        destory(&n->val);
        node_allocator::deallocate(n);
    }

    void initialize_buckets(size_type n)
    {
        const size_type n_buckets = next_size(n);
        // 举例: 传入 50, 返回 53. 以下首先保留 53 个元素空间, 然后将其全部填 0
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)0);
        num_elements = 0;
    }

    // 返回最接近 n 并大于或等于 n 的质数
    size_type next_size(size_type n) const { return __stl_next_prime(n); }
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

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint)
{
    // 以下, "表格重建与否" 的原则, 是拿元素个数(把新增元素计入后)和 bucket vector 的大小来比.
    // 如果前者大于后者, 就重建表格
    // 由此可知, 每个 bucket(list) 的最大容量和 buckets vector 的大小相同
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) {    // 确定真的需要重新配置
        const size_type n = next_size(num_elements_hint);   // 找出下一个质数
        if (n > old_n) {
            vector<node*, A> tmp(n, (node*)0);  // 设立新的 buckets
            __STL_TRY {
                // 以下处理每一个旧的 bucket
                for (size_type bucket = 0; bucket < old_n; ++bucket) {
                    node* first = buckets[bucket];  // 指向节点所对应的串行的起始节点
                    // 以下处理每一个旧 bucket 所含(串行)的每一个节点
                    while (first) {     // 串行还没结束时
                        // 以下找出节点落在哪一个新 bucket 内
                        size_type new_bucket = bkt_num(first->val, n);
                        // (1) 令旧 bucket 指向其所对应的串行的下一个节点(以便迭代处理)
                        buckets[bucket] = first->next;
                        // (2) (3)将当前节点插入到新 bucket 内, 成为其对应串行的第一个节点
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        // (4) 回到旧 bucket 所指的待处理串行, 准备处理下一个节点
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);  // vector::swap 新旧两个 buckets 对调
                // 注意, 对调两方如果大小不同, 大的会变小, 小的会变大
                // 离开时释放 local tmp的内存
            }
        }
    }
}

template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj)
{
    const size_type n = bkt_num(obj);   // 决定 obj 应位于 #n bucket
    node* first = buckets[n];           // 令 first 指向 bucket 对应的串行头部

    // 如果 buckets[n] 已被占用, 此时 first 将不为 0, 于是进入以下循环,
    // 走过 bucket 所对应的整个链表
    for (node* cur = first; cur; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            // 如果发现与链表中的某键值相同, 就不插入, 立刻返回
            return pair<iterator, bool>(iterator(cur, this), false);
        }
    }
    // 离开以上循环(或根本未进入循环)时, first 指向 bucket 所指链表的头部节点
    node* tmp = new_node(obj);      // 产生新节点
    tmp->next = first;
    buckets[n] = tmp;               // 令新节点成为链表的第一个节点
    ++num_elements;                 // 节点个数累加 1
    return pair<iterator, bool>(iterator(tmp, this), true);
}

#endif
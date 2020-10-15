// file: 5hashtable-test.cpp

#include <iostream>

#include "../src/stl_hashtable.h"
#include "../src/stl_hash_fun.h"
#include "../src/stl_function.h"
#include "../src/stl_alloc.h"

int main(void)
{
    // note: hash-table has no default ctor
    hashtable<int, int, hash<int>, identity<int>, equal_to<int>, alloc>
        iht(50, hash<int>(), equal_to<int>());
    
    std::cout << iht.size() << std::endl;               // 0
    std::cout << iht.bucket_count() << std::endl;       // 53
    std::cout << iht.max_bucket_count() << std::endl;   // 429496729

    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique(55);
    std::cout << iht.size() << std::endl;   // 6

    hashtable<int, int, hash<int>, identity<int>, equal_to<int>, alloc>::
        iterator ite = iht.begin();
    
    // 以迭代器遍历 hashtable, 将所以节点的值打印出来
    for (int i = 0; i < iht.size(); ++i, ++ite) {
        std::cout << *ite << ' ';
    }
    std::cout << std::endl;

    // 遍历所有 buckets, 如果其节点个数不为 0, 就打印出节点个数
    for (int i = 0; i < iht.bucket_count(); ++i) {
        int n = iht.elems_in_bucket(i);
        if (n != 0) {
            std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
        }
    }
    // bucket[0] has 1 elems.
    // bucket[2] has 3 elems.
    // bucket[6] has 1 elems.
    // bucket[10] has 1 elems.

    // 为了验证 "bucket(list) 的容量就是 buckets vector 的大小" (这是从 hashtable<T>::resize() 得知的结果),
    // 现在将元素增加到 54 个, 看看是否会发生表格重建(re-hashing)
    for (int i = 0; i <= 47; ++i) {
        iht.insert_equal(i);
    }
    std::cout << iht.size() << std::endl;           // 54. 元素(节点)个数
    std::cout << iht.bucket_count() <<std::endl;    // 96. buckets 个数
    // 遍历所有 buckets, 如果其节点个数不为 0, 就打印出节点个数
    for (int i = 0; i < iht.bucket_count(); ++i) {
        int n = iht.elems_in_bucket(i);
        if (n != 0) {
            std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
        }
    }
    // 打印结果: bucket[2] 和 bucket[11] 的节点个数为 2,
    // 其余的 bucket[0]~bucket[47] 的节点个数均为 1
    // 此外, bucket[53], [55], [63]的节点个数均为 1

    // 以迭代器遍历 hashtable, 将所有的节点的值打印出来
    ite = iht.begin();
    for (int i = 0; i < iht.size(); ++i, ++ite) {
        std::cout << *ite << ' ';
    }
    std::cout << std::endl;
    // 0 1 2 2 3 4 5 6 7 8 9 10 11 108 12 13 14 15 16 17 18 19 20 21
    // 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42
    // 43 44 45 46 47 53 55 59 63

    std::cout << *(iht.find(2)) << std::endl;   // 2
    std::cout << iht.count(2) << std::endl;     // 2

    return 0;
}
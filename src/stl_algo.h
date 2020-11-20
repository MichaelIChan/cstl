#ifndef __STL_ALGO_H
#define __STL_ALGO_H

#include <stdlib.h>

#include "stl_tempbuf.h"
#include "stl_algobase.h"
#include "stl_heap.h"
#include "stl_pair.h"

namespace cstl
{

// 算法 set_union 可构造 S1, S2 的并集

// 求存在于 [first1, last1) 或存在于 [first2, last2) 的所有元素
// 注意, set 是一种 sorted range. 这是以下算法的前提
// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result)
{
    // 当两个区间都尚未达到尾端时, 执行以下操作
    while (first1 != last1 && first2 != last2) {
        // 在两区间内分别移动迭代器. 首先将元素值较小者(假设为 A 区)记录于目标区,
        // 然后移动 A 区迭代器使之前进; 同时间的另一个区迭代器不动. 然后进行新一次
        // 的比大小、记录小值、迭代器移动... 直到两区中有一区到达尾端. 如果元素相等,
        // 取 S1 者记录于目标区, 并同时移动两个迭代器
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        } else {    // *first2 == *first1
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }

    // 只要两区之中有一区达到尾端, 就结束上述的 while 循环
    // 以下将尚未达到尾端的区间的所以剩余元素拷贝到目的端
    // 此刻的 [first1, last1) 和 [first2, last2) 之中至少有一个是空白区间
    return copy(first2, last2, copy(first1, last1, result));
}

// 版本二
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {    // *first2 == *first1
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }

    return copy(first2, last2, copy(first1, last1, result));
}

// 算法 set_intersection 可构造 S1, S2 的交集

// 求存在于 [first1, last1) 且存在于 [first2, last2) 的所有元素
// 注意, set 是一种 sorted range. 这是以下算法的前提
// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result)
{
    // 当两个区间都尚未达到尾端时, 执行以下操作
    while (first1 != last1 && first2 != last2) {
        // 在两区间内分别移动迭代器, 直到遇有元素值相同, 暂停, 将该值记录于目标区,
        // 再继续移动迭代器... 直到两区之中有一区到达尾端
        if (*first1 < *first2) {
            ++first1;
        } else if (*first2 < *first1) {
            ++first2;
        } else {    // *first2 == *first1
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }

    return result;
}

// 版本二
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else if (comp(*first2, *first1)) {
            ++first2;
        } else {    // *first2 == *first1
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }

    return result;
}

// 算法 set_difference 可构造 S1, S2 的差集

// 求存在于 [first1, last1) 且不存在于 [first2, last2) 的所有元素
// 注意, set 是一种 sorted range. 这是以下算法的前提
// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result)
{
    // 当两个区间都尚未达到尾端时, 执行以下操作
    while (first1 != last1 && first2 != last2) {
        // 在两区间内分别移动迭代器. 当第一区间的元素等于第二区间的元素(表示此值
        // 同时存在于两区间), 就让两区间同时前进; 当第一个区间的元素大于第二个区间的元素,
        // 就让第二区间前进; 有了这两种处理, 就保证当第一区间的元素小于第二区间的元素时,
        // 第一区间的元素只存在于第一区间中, 不存在于第二区间, 于是将它记录于目标区
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (*first2 < *first1) {
            ++first2;
        } else {    // *first2 == *first1
            ++first1;
            ++first2;
        }
    }

    return copy(first1, last1, result);
}

// 版本二
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (comp(*first2, *first1)) {
            ++first2;
        } else {    // *first2 == *first1
            ++first1;
            ++first2;
        }
    }

    return copy(first1, last1, result);
}

// 算法 set_symmetric_difference 可构造 S1, S2 的对称差集, 即 (S1-S2) U (S2-S1)

// 求存在于 [first1, last1) 且不存在于 [first2, last2) 的所有元素,
// 以及存在于 [first2, last2) 且不存在于 [first1, last1) 的所有元素
// 注意, 上述定义只有在 "元素值独一无二" 的情况下才成立. 如果将 set 一般化,
// 允许出现重复元素, 那么 set-symmetric-difference 的定义应该是:
// 如果某值在 [first, last1) 出现 n 次, 在 [first2, last2) 出现 m 次,
// 那么它在 result range 中应该出现 abs(n-m) 次
// 注意, set 是一种 sorted range. 这是以下算法的前提
// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result)
{
    // 当两个区间都尚未达到尾端时, 执行以下操作
    while (first1 != last1 && first2 != last2) {
        // 在两区间内分别移动迭代器. 当两区间内的元素相等, 就让两区间同时前进;
        // 当两区间内的元素不等, 就记录较小值于目标区, 并令较小值所在区间前进
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
            ++result;
        } else {    // *first2 == *first1
            ++first1;
            ++first2;
        }
    }

    return copy(first2, last2, copy(first1, last1, result));
}

// 版本二
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result, Compare comp)
{
    // 当两个区间都尚未达到尾端时, 执行以下操作
    while (first1 != last1 && first2 != last2) {
        // 在两区间内分别移动迭代器. 当两区间内的元素相等, 就让两区间同时前进;
        // 当两区间内的元素不等, 就记录较小值于目标区, 并令较小值所在区间前进
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
            ++result;
        } else {    // *first2 == *first1
            ++first1;
            ++first2;
        }
    }

    return copy(first2, last2, copy(first1, last1, result));
}

// adjacent_find() 找出第一组满足条件的相邻元素

// 查找相邻的重复元素. 版本一
template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
{
    if (first == last) return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (*first == *next) return first;
        first = next;
    }

    return last;
}

// 查找相邻的重复元素. 版本二
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
                              BinaryPredicate binary_pred)
{
    if (first == last) return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (binary_pred(*first, *next)) return first;
        first = next;
    }

    return last;
}

// count() 运用 equality 操作符, 将 [first, last) 区间内的每一个元素拿来和指定值 value 比较,
// 并返回与 value 相等的元素个数
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for ( ; first != last; ++first) {
        if (*first == value) {
            ++n;
        }
    }

    return n;
}

// count_if() 将指定操作(一个仿函数) pred 实施于 [first, last) 区间内的每一个元素,
// 并将 "造成 pred 的计算结果为 true" 的所有元素个数返回
template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for ( ; first != last; ++first) {
        if (pred(*first)) {
            ++n;
        }
    }

    return n;
}

// search() 在序列一 [first1, last1) 所涵盖的区间中, 查找序列二 [first2, last2) 的首次出现的.
// 如果序列一内不存在与序列二完全匹配的子序列, 便返回迭代器 last1

template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
                          ForwardIterator2 first2, ForwardIterator2 last2,
                          Distance1*, Distance2*)
{
    Distance1 d1 = 0;
    distance(first1, last1, d1);
    Distance1 d2 = 0;
    distance(first2, last2, d2);

    if (d1 < d2) return last1; // 如果第二序列大于第一序列, 不可能成为其子序列

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;

    while (current2 != last2) {
        if (*current1 == *current2) {
            ++current1;
            ++current2;
        } else {
            if (d1 == d2) {             // 如果两序列一样长, 表示不可能成功两
                return last1;
            } else {                    // 如果两序列不一样长(至此肯定是序列一大于序列二)
                current1 = ++first1;    // 调整第一序列的标兵
                current2 = first2;      // 准备在新起点上再找一次
                --d1;                   // 已经排除了序列一的一个元素, 所以序列一的长度要减 1
            }
        }
    }

    return first1;
}

template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2,
          class BinaryPredicate>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
                          ForwardIterator2 first2, ForwardIterator2 last2,
                          Distance1*, Distance2*, BinaryPredicate binary_pred)
{
    Distance1 d1 = 0;
    distance(first1, last1, d1);
    Distance1 d2 = 0;
    distance(first2, last2, d2);

    if (d1 < d2) return last1; // 如果第二序列大于第一序列, 不可能成为其子序列

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;

    while (current2 != last2) {
        if (binary_pred(*current1, *current2)) {
            ++current1;
            ++current2;
        } else {
            if (d1 == d2) {             // 如果两序列一样长, 表示不可能成功两
                return last1;
            } else {                    // 如果两序列不一样长(至此肯定是序列一大于序列二)
                current1 = ++first1;    // 调整第一序列的标兵
                current2 = first2;      // 准备在新起点上再找一次
                --d1;                   // 已经排除了序列一的一个元素, 所以序列一的长度要减 1
            }
        }
    }

    return first1;
}

// 查找子序列首次出现地点
// 版本一
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2)
{
    return __search(first1, last1, first2, last2,
                    distance_type(first1), distance_type(first2));
}

// 版本二
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2,
                               BinaryPredicate binary_pred)
{
    return __search(first1, last1, first2, last2,
                    distance_type(first1), distance_type(first2), binary_pred);
}

// find() 根据 equality 操作符, 循序查找 [first, last) 内的所有元素,
// 找出第一个匹配 "equality条件" 者
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while (first != last && *first != value) ++first;
    return first;
}

// find_if() 根据指定的 pred 运算符(以仿函数表示), 循序查找 [first, last) 内的所有元素,
// 找出第一个令 pred 运算结果为 true 者
template <class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred)
{
    while (first != last && !pred(*first)) ++first;
    return first;
}

// find_end() 在序列一 [first1, last1) 所涵盖的区间中, 查找序列二
// [first2, last2) 的最后一次出现点
// 如果序列一之内不存在 "完全匹配序列二" 的自序列, 便返回迭代器 last1

// 以下是 forward iterators 版
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 __find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            forward_iterator_tag, forward_iterator_tag)
{
    if (first2 == last2) {      // 如果查找目标是空的
        return last1;           // 返回 last1, 表示该 "空子序列" 的最后出现点
    } else {
        ForwardIterator1 result = last1;
        while (1) {
            // 以下利用 search() 查找某个子序列的首次出现点. 找不到的话返回 last1
            ForwardIterator1 new_result = search(first1, last1, first2, last2);
            if (new_result == last1) {  // 没找到
                return result;
            } else {
                result = new_result;    // 调动一下标兵, 准备下一个查找行动
                first1 = new_result;
                ++first1;
            }
        }
    }
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
ForwardIterator1 __find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            forward_iterator_tag, forward_iterator_tag,
                            BinaryPredicate binary_pred)
{
    if (first2 == last2) {      // 如果查找目标是空的
        return last1;           // 返回 last1, 表示该 "空子序列" 的最后出现点
    } else {
        ForwardIterator1 result = last1;
        while (1) {
            // 以下利用 search() 查找某个子序列的首次出现点. 找不到的话返回 last1
            ForwardIterator1 new_result = search(first1, last1, first2, last2, binary_pred);
            if (new_result == last1) {  // 没找到
                return result;
            } else {
                result = new_result;    // 调动一下标兵, 准备下一个查找行动
                first1 = new_result;
                ++first1;
            }
        }
    }
}

// 以下是 bidirectional iterators 版 (可以逆向查找)
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                  BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                  bidirectional_iterator_tag, bidirectional_iterator_tag)
{
    // 由于查找的是 "最后出现地点", 因此反向查找比较快. 利用 reverse_iterator
    typedef reverse_iterator<BidirectionalIterator1> reviter1;
    typedef reverse_iterator<BidirectionalIterator2> reviter2;

    reviter1 rlast1(first1);
    reviter2 rlast2(first2);

    // 查找时, 将序列一和序列二统统逆转方向
    reviter1 rresult = search(reviter1(last1), rlast1, reviter2(last2), rlast2);

    if (rresult == rlast1) {    // 没找到
        return last1;
    } else {
        BidirectionalIterator1 result = rresult.base();     // 转回正常(非逆向)迭代器
        advance(result, -distance(first2, last2));          // 调整回到子序列的起头处
        return result;
    }
}

template <class BidirectionalIterator1, class BidirectionalIterator2, class BinaryPredicate>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                  BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                  bidirectional_iterator_tag, bidirectional_iterator_tag,
                                  BinaryPredicate binary_pred)
{
    // 由于查找的是 "最后出现地点", 因此反向查找比较快. 利用 reverse_iterator
    typedef reverse_iterator<BidirectionalIterator1> reviter1;
    typedef reverse_iterator<BidirectionalIterator2> reviter2;

    reviter1 rlast1(first1);
    reviter2 rlast2(first2);

    // 查找时, 将序列一和序列二统统逆转方向
    reviter1 rresult = search(reviter1(last1), rlast1, reviter2(last2), rlast2, binary_pred);

    if (rresult == rlast1) {    // 没找到
        return last1;
    } else {
        BidirectionalIterator1 result = rresult.base();     // 转回正常(非逆向)迭代器
        advance(result, -distance(first2, last2));          // 调整回到子序列的起头处
        return result;
    }
}

// 版本一
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2)
{
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category2;

    // 以下根据两个区间的类属, 调用不同的下层函数
    return __find_end(first1, last1, first2, last2, category1(), category2());
}

// 版本二
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate binary_pred)
{
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category2;

    // 以下根据两个区间的类属, 调用不同的下层函数
    return __find_end(first1, last1, first2, last2, category1(), category2(), binary_pred);
}

// find_first_of() 算法以 [first2, last2) 区间内的某些元素作为查找目标,
// 寻找它们在 [first1, last1) 区间内的第一次出现地点

// 版本一
template <class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2)
{
    for ( ; first1 != last1; ++first1) {
        for (ForwardIterator iter = first2; iter != last2; ++iter) {
            if (*first1 == *iter) {
                return first1;
            }
        }
    }
    return last1;
}

// 版本二
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2,
                            BinaryPredicate binary_pre)
{
    for ( ; first1 != last1; ++first1) {
        for (ForwardIterator iter = first2; iter != last2; ++iter) {
            if (binary_pre(*first1, *iter)) {
                return first1;
            }
        }
    }
    return last1;
}

// for_each() 将仿函数 非施行于 [first, last) 区间内的每一个元素身上
template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
    for ( ; first != last; ++first) {
        f(*first);
    }
    return f;
}

// generate() 将仿函数 gen 的运算结果填写在 [first, last) 区间内的所有元素身上
template <class ForwardIterator, class Generator>
void generator(ForwardIterator first, ForwardIterator last, Generator gen)
{
    for ( ; first != last; ++first) {
        *first = gen();
    }
}

// generate_n() 将仿函数 gen 的运算结果填写在从迭代器 first 开始的 n 个元素身上
template <class OutputIterator, class Size, class Generator>
OutputIterator generator_n(OutputIterator first, Size n, Generator gen)
{
    for ( ; n > 0; --n, ++first) {
        *first = gen();
    }
    return first;
}

// includes() 判断序列二 S2 是否 "涵盖于" 序列一
// S1 和 S2 都必须是有序集合, 其中的元素都可重复

// 版本一. 判断区间二的每个元素是否都存在于区间一
// 前提: 区间一和区间二都是 sorted ranges
template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            return false;
        } else if (*first1 < *first2) {
            ++first1;
        } else {
            ++first1, ++first2;
        }
    }
    return first2 == last2;
}

// 版本二. 判断序列一内是否有个子序列, 其与序列二的每个对应元素都满足二元运算 comp
// 前提: 区间一和区间二都是 sorted ranges
template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) {
            return false;
        } else if (comp(*first1, *first2)) {
            ++first1;
        } else {
            ++first1, ++first2;
        }
    }
    return first2 == last2;
}

// max_element() 返回一个迭代器, 指向序列之中数值最大的元素
// 版本一
template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
{
    if (first == last) return first;
    ForwardIterator result = first;
    while (++first != last) {
        if (*result < *first) result = first;
    }
    return result;
}

// 版本一
template <class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
    if (first == last) return first;
    ForwardIterator result = first;
    while (++first != last) {
        if (comp(*result, *first)) result = first;
    }
    return result;
}

// merge() 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间
// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result)
{
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    // 最后剩余元素以 copy 复制到目的端. 以下两个序列一定至少有一个为空
    return copy(first2, last2, copy(first1, last1, result));
}

// 版本一
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    // 最后剩余元素以 copy 复制到目的端. 以下两个序列一定至少有一个为空
    return copy(first2, last2, copy(first1, last1, result));
}

// min_element() 返回一个迭代器, 指向序列之中数值最小的元素
// 版本一
template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
{
    if (first == last) return first;
    ForwardIterator result = first;
    while (++first != last) {
        if (*first < *result) result = first;
    }
    return result;
}

// 版本二
template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
    if (first == last) return first;
    ForwardIterator result = first;
    while (++first != last) {
        if (comp(*first, *result)) result = first;
    }
    return result;
}

// partition() 将区间 [first, last) 中的元素重新排列
// 所有被 pred 判定为 true 的元素, 都被放到前段
// 被 pred 判定为 false 的元素, 都被放到后段
// 不保证保留原相对位置
template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last,
                                Predicate pred)
{
    while (true) {
        while (true) {
            if (first == last) {
                return first;
            } else if (pred(*first)) {
                ++first;
            } else {
                break;
            }
        }
        --last;
        while (true) {
            if (first == last) {
                return first;
            } else if (!pred(*last)) {
                --last;
            } else {
                break;
            }
        }
        iter_swap(first, last);
        ++first;
    }
}

// remove() 移除(不删除) [first, last) 之中所有与 value 相等的元素
// 将每一个与 value 相等的元素轮番复制给 first 之后的空间
template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value)
{
    first = find(first, last, value);       // 找出第一个相等元素
    ForwardIterator next = first;
    // 以下利用 "remove_copy() 允许新旧容器重叠" 的性质, 进行移除操作
    // 并将结果指定于原容器中
    return first == last ? first : remove_copy(++next, last, first, value);
}

// remove_copy() 移除 [first, last) 区间内所有与 value 相等的元素
// 并不是真正从容器中删除那些元素, 而是将结果复制到一个以 result 标示其实位置的容器上
template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last,
                           OutputIterator result, const T& value)
{
    for ( ; first != last; ++first) {
        if (*first != value) {
            *result = *first;
            ++result;
        }
    }
    return result;
}

// remove_if() 移除(不删除) [first, last) 区间内所有被仿函数 pred 核定为 true 的元素
template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred)
{
    first = find_if(first, last, pred);       // 找出第一个相等元素
    ForwardIterator next = first;
    // 以下利用 "remove_copy_if() 允许新旧容器重叠" 的性质, 进行移除操作
    // 并将结果指定于原容器中
    return first == last ? first : remove_copy_if(++next, last, first, pred);
}

// remove_copy_if() 移除 [first, last) 区间内所有被仿函数 pred 评估为 true 的元素
template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last,
                              OutputIterator result, Predicate pred)
{
    for ( ; first != last; ++first) {
        if (!pred(*first)) {
            *result = *first;
            ++result;
        }
    }
    return result;
}

// replace() 将 [first, last) 区间内所有 old_value 和 new_value 取代
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last,
             const T& old_value, const T& new_value)
{
    for ( ; first != last; ++first) {
        if (*first == old_value) *first = new_value;
    }
}

// replace_copy() 与 replace()类似, 唯一不同的是新序列会被复制到 result 所指的容器中
template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last,
                            OutputIterator result,
                            const T& old_value, const T& new_value)
{
    for ( ; first != last; ++first, ++result) {
        *result = *first == old_value ? new_value : *first;
    }
    return result;
}

// replace_if() 将 [first, last) 区间内所有 "被 pred 评估为 true" 的元素, 都以 new_value 取而代之
template <class ForwardIterator, class Predicate, class T>
void replace(ForwardIterator first, ForwardIterator last,
             Predicate pred, const T& new_value)
{
    for ( ; first != last; ++first) {
        if (pred(*first)) *first = new_value;
    }
}

// replace_copy_if() 与 replace_if()类似, 但是新序列会被复制到 result 所指的区间内
template <class InputIterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if(InputIterator first, InputIterator last,
                               OutputIterator result, Predicate pred,
                               const T& new_value)
{
    for ( ; first != last; ++first, ++result) {
        *result = pred(*first) ? new_value : *first;
    }
    return result;
}

// reverse() 将序列 [first, last) 但元素在原容器中颠倒重排

// reverse 的 bidirectional iterator 版
template <class BidirectionalIterator>
void __reverse(BidirectionalIterator first, BidirectionalIterator last,
               bidirectional_iterator_tag)
{
    while (true) {
        if (first == last || first == --last) {
            return;
        } else {
            iter_swap(first++, last);
        }
    }
}

// reverse 的 random access iterator 版
template <class RandomAccessIterator>
void __reverse(RandomAccessIterator first, RandomAccessIterator last,
               random_access_iterator_tag)
{
    // 以下, 头尾两两互换, 然后头部累进一个位置, 尾部累退一个位置. 两者交错时即停止
    // 注意, 只有 random iterators 才能做以下的 first < last 判断
    while (first < last) iter_swap(first++, --last);
}

// 分派函数
template <class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last)
{
    __reverse(first, last, iterator_category(first));
}

// reverse_copy() 与 reverse()类似, 但产生出来的新序列会被置于以 result 指出的容器中
template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last,
                            OutputIterator result)
{
    while (first != last) {
        --last;
        *result = *last;
        ++result;
    }
    return result;
}

// rotate() 将 [first, middle) 内的元素和 [middle, last) 内的元素互换

// rotate 的 forward iterator 版
template <class ForwardIterator, class Distance>
void __rotate(ForwardIterator first, ForwardIterator middle,
              ForwardIterator last, Distance*, forward_iterator_tag)
{
    for (ForwardIterator i = middle; ; ) {
        iter_swap(first, i);    // 前段、后段的元素一一交换
        ++first;
        ++i;
        // 以下判断是前段 [first, middle) 先结束还是后段 [middle, last) 先结束
        if (first == middle) {          // 前段结束了
            if (i == last) return;      // 如果后段同时也结束了, 整个就结束了
            middle = i;                 // 否则调整, 对新的前, 后段再作交换
        } else if (i == last) {         // 后段结束了
            i = middle;                 // 调整, 准备对新的前, 后段再作交换
        }
    }
}

// rotate 的 bidirectional iterator 版
template <class BidirectionalIterator, class Distance>
void __rotate(BidirectionalIterator first, BidirectionalIterator middle,
              BidirectionalIterator last, Distance*, bidirectional_iterator_tag)
{
    reverse(first, middle);
    reverse(middle, last);
    reverse(first, last);
}

// 最大共因子, 利用辗转相除法
// __gcd() 应用于 __rotate() 的 random access iterator 版
template <class EuclideanRingElement>
EuclideanRingElement __gcd(EuclideanRingElement m, EuclideanRingElement n)
{
    while (n != 0) {
        EuclideanRingElement t = m % n;
        m = n;
        n = t;
    }
    return m;
}

template <class RandomAccessIterator, class Distance, class T>
void __rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
                    RandomAccessIterator initial, Distance shift, T*)
{
    T value = *initial;
    RandomAccessIterator ptr1 = initial;
    RandomAccessIterator ptr2 = ptr1 + shift;
    while (ptr2 != initial) {
        *ptr1 = *ptr2;
        ptr1 = ptr2;
        if (last - ptr2 > shift) {
            ptr2 += shift;
        } else {
            ptr2 = first + (shift - (last - ptr2));
        }
    }
    *ptr1 = value;
}

// rotate 的 random access iterator 版
template <class RandomAccessIterator, class Distance>
void __rotate(RandomAccessIterator first, RandomAccessIterator middle,
              RandomAccessIterator last, Distance*, random_access_iterator_tag)
{
    // 以下迭代器的相减操作, 只适用于 random access iterator
    // 取全长和前段长度的最大公因子
    Distance n = __gcd(last - first, middle - first);
    while (n--) {
        __rotate_cycle(first, last, first + n, middle - first, value_type(first));
    }
}

// 分派函数
template <class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
{
    if (first == middle || middle == last) return;
    __rotate(first, middle, last, distance_type(first), iterator_category(first));
}

// rotate_copy() 行为类似 rotate(), 但产生出来的新序列会被置于 result 所指出的容器中
template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle,
                           ForwardIterator last, OutputIterator result)
{
    return copy(first, middle, copy(middle, last, result));
}

// search_n() 在序列 [first, last) 所涵盖的区间中, 查找 "连续 count 个符合条件的元素" 所形成的子序列,
// 并返回一个迭代器指向该子序列起始处
// 版本一
template <class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                         Integer count, const T& value)
{
    if (count <= 0) {
        return first;
    } else {
        first = find(first, last, value);       // 首先找出 value 第一次出现点
        while (first != last) {                 // 继续查找余下元素
            Integer n = count - 1;              // value 还应出现 n 次
            ForwardIterator i = first;          // 从上次出现点接下去查找
            ++i;
            while (i != last && n != 0 && *i == value) {    // 找到下一个 value
                ++i;
                --n;
            }
            if (n == 0) {   // n == 0 表示确实找到了 "元素值出现 n 次" 的子序列
                return first;
            } else {
                first = find(i, last, value);   // 找 value 的下一个出现点
            }
        }
        return last;
    }
}

// 版本二
// 查找 "连续 count 个元素皆满足指定条件" 所形成的那个子序列的起点, 返回其发生位置
template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count,
                         const T& value, BinaryPredicate binary_pred)
{
    if (count <= 0) {
        return first;
    } else {
        while (first != last) {
            if (binary_pred(*first, value)) break;  // 找出第一个符合条件的元素
            ++first;
        }
        while (first != last) {
            Integer n = count - 1;
            ForwardIterator i = first;
            ++i;
            // 以下循环确定接下来 count - 1 个元素是否都符合条件
            while (i != last && n != 0 && binary_pred(*i, value)) {
                ++i;
                --n;
            }
            if (n == 0) {
                return first;
            } else {
                while (i != last) {
                    if (binary_pred(*i, value)) break;
                    ++i;
                }
                first = i;
            }
        }
        return last;
    }
}

// swap_ranges() 将 [first1, last1) 区间内的元素与 "从 first2 开始, 个数相同" 的元素互相交换
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1,
                             ForwardIterator2 first2)
{
    for ( ; first1 != last1; ++first1, ++first2) {
        iter_swap(first1, first2);
    }
    return first2;
}

// transform() 的第一个版本以仿函数 op 作用于 [first, last) 中的每一个元素身上,
// 并以其结果产生出一个新序列. 第二个版本以仿函数 binary_op 作用于一双元素身上(其中
// 一个元素来自 [first, last), 另一个元素来自 "从 first2 开始的序列"), 
// 并以其结果产生出一个新序列
// 版本一
template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last,
                         OutputIterator result, UnaryOperation op)
{
    for ( ; first != last; ++first, ++result) {
        *result = op(*first);
    }
    return result;
}

// 版本二
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                         OutputIterator result, BinaryOperation binary_op)
{
    for ( ; first1 != last1; ++first1, ++first2, ++result) {
        *result = binary_op(*first1, *first2);
    }
    return result;
}

// unique_copy() 可从 [first, last) 中将元素复制到以 result 开头的区间上;
// 如果面对相邻重复元素群, 只会复制其中第一个元素

// 版本一辅助函数, forward_iterator_tag 版
template <class InputIterator, class ForwardIterator>
ForwardIterator __unique_copy(InputIterator first, InputIterator last,
                              ForwardIterator result, forward_iterator_tag)
{
    *result = *first;           // 记录第一个元素
    while (++first != last) {
        // 以下, 元素不同就记录
        if (*result != *first) *++result = *first;
    }
    return ++result;
}

// 由于 output iterator 为 write only, 无法像 forward iterator 那般可以读取
// 所以不能有类似 *result != *first 这样的判断操作, 所以才需要设计这一版本
// 例如 ostream_iterator 就是一个 output iterator
template <class InputIterator, class OutputIterator, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last,
                             OutputIterator result, T*)
{
    // T 为 output iterator 的 value type
    T value = *first;
    *result = value;
    while (++first != last) {
        if (value != *first) {
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

template <class InputIterator, class OutputIterator, class T, class BinaryPredicate>
OutputIterator __unique_copy(InputIterator first, InputIterator last,
                             OutputIterator result, T*, BinaryPredicate binary_pred)
{
    // T 为 output iterator 的 value type
    T value = *first;
    *result = value;
    while (++first != last) {
        if (binary_pred(value, *first)) {
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

// 版本一辅助函数, output_iterator_tag 版
template <class InputIterator, class OutputIterator>
inline OutputIterator __unique_copy(InputIterator first, InputIterator last,
                                    OutputIterator result, output_iterator_tag)
{
    // 以下, output iterator 有一些功能限制, 所以必须先知道其 value type
    return __unique_copy(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator __unique_copy(InputIterator first, InputIterator last,
                                    OutputIterator result, output_iterator_tag,
                                    BinaryPredicate binary_pred)
{
    // 以下, output iterator 有一些功能限制, 所以必须先知道其 value type
    return __unique_copy(first, last, result, value_type(first), binary_pred);
}

// 版本一
template <class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last,
                                  OutputIterator result)
{
    if (first == last) return result;
    // 以下, 根据 result 的 iterator category 做不同的处理
    return __unique_copy(first, last, result, iterator_category(result));
}

// 版本二
template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(InputIterator first, InputIterator last,
                                  OutputIterator result, BinaryPredicate binary_pred)
{
    if (first == last) return result;
    // 以下, 根据 result 的 iterator category 做不同的处理
    return __unique_copy(first, last, result, iterator_category(result), binary_pred);
}

// unique() 能够移除重复的元素
// 注意 unique 只能移除相邻的重复元素
// 版本一
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last)
{
    first = adjacent_find(first, last);     // 首先找到相邻重复元素的起点
    return unique_copy(first, last, first);
}

// lower_bound() 是二分查找的一种版本, 试图在已排序的 [first, last) 中寻找元素 value.
// 如果 [first, last) 具有与 value 相等的元素, 便返回一个迭代器, 指向其中第一个元素.
// 如果没有这样的元素存在, 便返回 "假设这样的元素存在时应该出现的位置"

// 版本一辅助函数的 forward_iterator 版本
template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                              Distance*, forward_iterator_tag)
{
    Distance len = 0;
    distance(first, last, len);     // 求整个区间的长度 len
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                              Compare comp, Distance*, forward_iterator_tag)
{
    Distance len = 0;
    distance(first, last, len);     // 求整个区间的长度 len
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (comp(*middle, value)) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

// 版本一辅助函数的 random_access_iterator 版本
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Distance*, random_access_iterator_tag)
{
    Distance len = last - first;        // 求整个区间的长度 len
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;;
        if (*middle < value) {
            first = middle + 1;;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Compare comp, Distance*,
                                   random_access_iterator_tag)
{
    Distance len = last - first;        // 求整个区间的长度 len
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;;
        if (comp(*middle, value)) {
            first = middle + 1;;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

// lower_bound() 版本一
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value)
{
    return __lower_bound(first, last, value,
                         distance_type(first), iterator_category(first));
}

// lower_bound() 版本二
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp)
{
    return __lower_bound(first, last, value, comp,
                         distance_type(first), iterator_category(first));
}

// upper_bound() 是二分查找法的一个版本. 它试图在已排序的 [first, last) 中寻找 value.
// 返回 "在不破坏顺序的情况下, 可插入 value 的最后一个合适位置"

// 版本一辅助函数的 forward_iterator 版本
template <class ForwardIterator, class T, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last,
                              const T& value, Distance*, forward_iterator_tag)
{
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (value < *middle) {
            len = half;
        } else {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T& value,
                              Compare comp, Distance*, forward_iterator_tag)
{
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (comp(value, *middle)) {
            len = half;
        } else {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

// 版本一辅助函数的 random_access_iterator 版本
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Distance*, random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (value < *middle) {
            len = half;
        } else {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Compare comp, Distance*,
                                   random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (comp(value, *middle)) {
            len = half;
        } else {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

// upper_bound() 版本一
template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value)
{
    return __upper_bound(first, last, value,
                         distance_type(first), iterator_category(first));
}

// upper_bound() 版本二
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp)
{
    return __upper_bound(first, last, value, comp,
                         distance_type(first), iterator_category(first));
}

// 算法 binary_search() 是一种二分查找法, 试图在已排序的 [first ,last) 中寻找元素 value
// 版本一
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value)
{
    ForwardIterator i = lower_bound(first, last, value);
    return i != last && !(value < *i);
}

// 版本二
template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T& value, Compare comp)
{
    ForwardIterator i = lower_bound(first, last, value, comp);
    return i != last && !comp(value, *i);
}

// next_permutation() 会取得 [first, last) 所标示的序列的下一个排列组合
// 版本一
template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last)
{
    if (first == last) return false;    // 空区间
    BidirectionalIterator i = first;
    ++i;
    if (i == last) return false;        // 只有一个元素
    i = last;
    --i;

    for (;;) {
        BidirectionalIterator ii = i;
        --i;
        // 以上, 锁定一组(两个)相邻元素
        if (*i < *ii) {     // 前一个元素小于后一个元素
            BidirectionalIterator j = last;
            while (!(*i < *--j));   // 由尾端往前找, 直到遇上比 *i 大的元素
            iter_swap(i, j);
            reverse(ii, last);      // 将 ii 之后的元素全部逆向重排
            return true;
        }
        if (i == first) {           // 进行至最前面了
            reverse(first, last);   // 全部逆向重排
            return false;
        }
    }
}

// 版本二
template <class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last,
                      Compare comp)
{
    if (first == last) return false;    // 空区间
    BidirectionalIterator i = first;
    ++i;
    if (i == last) return false;        // 只有一个元素
    i = last;
    --i;

    for (;;) {
        BidirectionalIterator ii = i;
        --i;
        // 以上, 锁定一组(两个)相邻元素
        if (comp(*i, *ii)) {            // 前一个元素小于后一个元素
            BidirectionalIterator j = last;
            while (!comp(*i, *--j));    // 由尾端往前找, 直到遇上比 *i 大的元素
            iter_swap(i, j);
            reverse(ii, last);          // 将 ii 之后的元素全部逆向重排
            return true;
        }
        if (i == first) {               // 进行至最前面了
            reverse(first, last);       // 全部逆向重排
            return false;
        }
    }
}

// prev_permutation() 版本一
template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last)
{
    if (first == last) return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last) return false;
    i = last;
    --i;

    for (;;) {
        BidirectionalIterator ii = i;
        --i;
        // 以上, 锁定一组(两个)相邻元素
        if (*ii < *i) {
            BidirectionalIterator j = last;
            while (!(*--j < *i));   // 由尾端往前找, 直到遇上比 *i 小的元素
            iter_swap(i, j);
            reverse(ii, last);      // 将 ii 之后的元素全部逆向重排
            return true;
        }
        if (i == first) {           // 进行至最前面了
            reverse(first, last);   // 全部逆向重排
            return false;
        }
    }
}

// prev_permutation() 版本二
template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last,
                      Compare comp)
{
    if (first == last) return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last) return false;
    i = last;
    --i;

    for (;;) {
        BidirectionalIterator ii = i;
        --i;
        // 以上, 锁定一组(两个)相邻元素
        if (comp(*ii, *i)) {
            BidirectionalIterator j = last;
            while (!comp(*--j, *i));    // 由尾端往前找, 直到遇上比 *i 小的元素
            iter_swap(i, j);
            reverse(ii, last);          // 将 ii 之后的元素全部逆向重排
            return true;
        }
        if (i == first) {               // 进行至最前面了
            reverse(first, last);       // 全部逆向重排
            return false;
        }
    }
}

// random_shuffle() 将 [first, last) 的元素次序随机重排.
// 在 N! 种可能的元素排列顺序中随机选出一种, 此处 N 为 last - first
// random_shuffle 有两个版本. 版本一使用内部随机数产生器, 版本二使用一个会产生随机数的仿函数

template <class RandomAccessIterator, class Distance>
void __random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance*)
{
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
#ifdef __STL_NO_DRAND48
        iter_swap(i, first + Distance(rand() % ((i - first) + 1)));
#else
        iter_swap(i, first + Distance(lrand48() % ((i - first) + 1)));
#endif /* __STL_NO_DRAND48 */
    }
}

// SGI 版本一
template <class RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    __random_shuffle(first, last, distance_type(first));
}

// SGI 版本二
template <class RandomAccessIterator, class RandomNumberGenerator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last,
                    RandomNumberGenerator& rand)    // 注意, by reference
{
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
        iter_swap(i, first + rand((i - first) + 1));
    }
}

// partial_sort() 接受一个 middle 迭代器, 然后重新安排 [first, last),
// 使序列中的 middle-first 个最小元素, 以递增顺序排序, 置于 [first, middle) 内.
// 其余 last-middle 个元素安置于 [middle, last) 中, 不保证有任何特定顺序

template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                    RandomAccessIterator last, T*)
{
    make_heap(first, middle);
    // 以下的 i < last 判断操作, 只适用于 random iterator
    for (RandomAccessIterator i = middle; i < last; ++i) {
        if (*i < *first) {
            __pop_heap(first, middle, i, T(*i), distance_type(first));
        }
    }
    sort_heap(first, middle);
}

template <class RandomAccessIterator, class T, class Compare>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                    RandomAccessIterator last, T*, Compare comp)
{
    make_heap(first, middle);
    // 以下的 i < last 判断操作, 只适用于 random iterator
    for (RandomAccessIterator i = middle; i < last; ++i) {
        if (comp(*i, *first)) {
            __pop_heap(first, middle, i, T(*i), distance_type(first));
        }
    }
    sort_heap(first, middle);
}

// 版本一
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                         RandomAccessIterator last)
{
    __partial_sort(first, middle, last, value_type(first));
}

// 版本二
template <class RandomAccessIterator, class Compare>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                         RandomAccessIterator last, Compare comp)
{
    __partial_sort(first, middle, last, value_type(first), comp);
}

// partial_sort() 和 partial_sort_copy() 两者行为逻辑完全相同,
// 后者将 last-first 个最小元素(或最大元素, 视 comp 而定) 排序后的
// 所得结果置于 [result_first, result_last)

template <class InputIterator, class RandomAccessIterator, class Distance, class T>
RandomAccessIterator __partial_sort_copy(InputIterator first, InputIterator last,
                                         RandomAccessIterator result_first,
                                         RandomAccessIterator result_last, 
                                         Distance*, T*) {
    if (result_first == result_last) return result_last;
    RandomAccessIterator result_real_last = result_first;
    while(first != last && result_real_last != result_last) {
        *result_real_last = *first;
        ++result_real_last;
        ++first;
    }
    make_heap(result_first, result_real_last);
    while (first != last) {
        if (*first < *result_first) {
            __adjust_heap(result_first, Distance(0),
                          Distance(result_real_last - result_first), T(*first));
        }
        ++first;
    }
    sort_heap(result_first, result_real_last);
    return result_real_last;
}

// 版本一
template <class InputIterator, class RandomAccessIterator>
inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
                                              RandomAccessIterator result_first,
                                              RandomAccessIterator result_last) {
    return __partial_sort_copy(first, last, result_first, result_last, 
                               distance_type(result_first),
                               value_type(first));
}

template <class InputIterator, class RandomAccessIterator, class Compare, class Distance, class T>
RandomAccessIterator __partial_sort_copy(InputIterator first, InputIterator last,
                                         RandomAccessIterator result_first,
                                         RandomAccessIterator result_last,
                                         Compare comp, Distance*, T*) {
    if (result_first == result_last) return result_last;
    RandomAccessIterator result_real_last = result_first;
    while(first != last && result_real_last != result_last) {
        *result_real_last = *first;
        ++result_real_last;
        ++first;
    }
    make_heap(result_first, result_real_last, comp);
    while (first != last) {
        if (comp(*first, *result_first)) {
            __adjust_heap(result_first, Distance(0), Distance(result_real_last - result_first),
                          T(*first), comp);
        }
        ++first;
    }
    sort_heap(result_first, result_real_last, comp);
    return result_real_last;
}

// 版本二
template <class InputIterator, class RandomAccessIterator, class Compare>
inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
                                              RandomAccessIterator result_first,
                                              RandomAccessIterator result_last, Compare comp) { 
  return __partial_sort_copy(first, last, result_first, result_last, comp,
                             distance_type(result_first), value_type(first));
}

// insertion sort

// 版本一辅助函数 __unguarded_linear_insert()
template <class RandomAccessIterator, class T>
void __unguarded_linear_insert(RandomAccessIterator last, T value)
{
    RandomAccessIterator next = last;
    --next;
    // insertion sort 的内循环
    // 注意, 一旦不再出现逆转对 (inversion), 循环就可以结束了
    while (value < *next) {     // 逆转对 (inversion) 存在
        *last = *next;          // 调整
        last = next;            // 调整迭代器
        --next;                 // 左移一个位置
    }
    *last = value;              // value 的正确落脚处
}

template <class RandomAccessIterator, class T, class Compare>
void __unguarded_linear_insert(RandomAccessIterator last, T value, Compare comp)
{
    RandomAccessIterator next = last;
    --next;
    // insertion sort 的内循环
    // 注意, 一旦不再出现逆转对 (inversion), 循环就可以结束了
    while (comp(value, *next)) {    // 逆转对 (inversion) 存在
        *last = *next;              // 调整
        last = next;                // 调整迭代器
        --next;                     // 左移一个位置
    }
    *last = value;                  // value 的正确落脚处
}

// 版本一辅助函数 __linear_insert()
template <class RandomAccessIterator, class T>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*)
{
    T value = *last;
    if (comp(value, *first)) {                           // 尾元素比头元素小
        copy_backward(first, last, last + 1);       // 将整个区间向右递移一个位置
        *first = value;                             // 令头元素等于原先的尾元素值
    } else {
        __unguarded_linear_insert(last, value);
    }
}

template <class RandomAccessIterator, class T, class Compare>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last,
                            T*, Compare comp)
{
    T value = *last;
    if (value < *first) {                           // 尾元素比头元素小
        copy_backward(first, last, last + 1);       // 将整个区间向右递移一个位置
        *first = value;                             // 令头元素等于原先的尾元素值
    } else {
        __unguarded_linear_insert(last, value, comp);
    }
}

// __insertion_sort() 版本一
template <class RandomAccessIterator>
void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {  // 外循环
        __linear_insert(first, i, value_type(first));
        // 以上, [first, i) 形成一个子区间
    }
}

// __insertion_sort() 版本二
template <class RandomAccessIterator, class Compare>
void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {  // 外循环
        __linear_insert(first, i, value_type(first), comp);
        // 以上, [first, i) 形成一个子区间
    }
}

// quick sort

// 返回 a, b, c 之居中者
template <class T>
inline const T& __median(const T& a, const T& b, const T& c)
{
    if (a < b) {
        if (b < c) {        // a < b < c
            return b;
        } else if (a < c) { // a < b, b >= c, a < c
            return c;
        } else {
            return a;
        }
    } else if (a < c) {     // c > a >= b
        return a;
    } else if (b < c) {     // a >= b, a >= c, b < c
        return c;
    } else {
        return b;
    }
}

// 分割函数, 其返回值是分割后的右段第一个位置
template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first,
                                           RandomAccessIterator last, T pivot)
{
    while (true) {
        while (*first < pivot) ++first;         // first 找到 >= pivot 的元素就停下来
        --last;                                 // 调整
        while (pivot < *last) --last;           // last 找到 <= pivot 的元素就停下来
        // 注意, 以下 first < last 判断操作, 只适用于 random iterator
        if (!(first < last)) return first;      // 交错, 结束循环
        iter_swap(first, last);                 // 大小值交换
        ++first;                                // 调整
    }
}

// SGI STL sort

// __lg() 用来控制分割恶化的情况
// 找出 2 ^ k <= n 的最大值 k
template <class Size>
inline Size __lg(Size n)
{
    Size k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

const int __stl_threshold = 16;

// 当元素个数为 40 时, __introsort_loop() 的最后一个参数将是 5 * 2, 即最多允许分割 10 层
// 注意, 本函数内的许多迭代器运算操作, 都只适用于 RandomAccess Iterator
template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last,
                      T*, Size depth_limit)
{
    while (last - first > __stl_threshold) {
        if (depth_limit == 0) {                 // 至此, 分割恶化
            partial_sort(first, last, last);    // 改用 heapsort
            return;
        }
        --depth_limit;
        // 以下是 median-of-3 partition, 选择一个够好的枢轴并决定分割点
        // 分割点将落在迭代器 cut 身上
        RandomAccessIterator cut = __unguarded_partition(first, last,
                        T(__median(*first, *(first + (last - first) / 2, *(last - 1)))));
        // 对右半段递归进行 sort
        __introsort_loop(cut, last, value_type(first), depth_limit);
        last = cut;
        // 现在回到 while 循环, 准备对左半段递归进行 sort
    }
}

template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (last - first > __stl_threshold) {
        __insertion_sort(first, first + __stl_threshold);
        __unguarded_insertion_sort(first + __stl_threshold, last);
    } else {
        __insertion_sort(first, last);
    }
}

template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    __unguarded_insertion_sort_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last,
                                       T*)
{
    for (RandomAccessIterator i = first; i != last; ++i) {
        __unguarded_linear_insert(i, T(*i));
    }
}

// sort() 只适用于 RandomAccessIterator
template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first != last) {
        __introsort_loop(first, last, value_type(first), __lg(last - first) * 2);
        __final_insertion_sort(first, last);
    }
}

// equal_range() 是二分查找法的一个版本, 试图在已排序的 [first, last) 中寻找 value
// 返回一对迭代器 i 和 j, 其中 i 是在不破坏次序的前提下, value 可插入的第一个位置,
// j 是在不破坏次序的前提下, value 可插入的最后一个位置.

// 版本一的 random_access_iterator 版本
template <class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last,
              const T& value, Distance*, random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle, left, right;

    while (len > 0) {
        half = len >> 1;                // 找出中央位置
        middle = first + half;          // 设定中央迭代器
        if (*middle < value) {
            first = middle + 1;         // 将运作区间缩小(移至后半段), 以提高效率
            len = len - half - 1;
        } else if (value < *middle) {
            len = half;                 // 将运作区间缩小(移至前半段), 以提高效率
        } else {    // 中央元素等于指定值
            // 在前半段找 lower_bound
            left = lower_bound(first, middle, value);
            // 在后半段找 upper_bound
            right = upper_bound(++middle, first + len, value);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }
    // 整个区间内都没有匹配的值, 那么应该返回一对迭代器, 指向第一个大于 value 的元素
    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template <class RandomAccessIterator, class T, class Distance, class Compare>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last,
              const T& value, Distance*, random_access_iterator_tag, Compare comp)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle, left, right;

    while (len > 0) {
        half = len >> 1;                // 找出中央位置
        middle = first + half;          // 设定中央迭代器
        if (comp(*middle, value)) {
            first = middle + 1;         // 将运作区间缩小(移至后半段), 以提高效率
            len = len - half - 1;
        } else if (comp(value, *middle)) {
            len = half;                 // 将运作区间缩小(移至前半段), 以提高效率
        } else {    // 中央元素等于指定值
            // 在前半段找 lower_bound
            left = lower_bound(first, middle, value);
            // 在后半段找 upper_bound
            right = upper_bound(++middle, first + len, value);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }
    // 整个区间内都没有匹配的值, 那么应该返回一对迭代器, 指向第一个大于 value 的元素
    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

// 版本一的 forward_iterator 版本
template <class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last,
              const T& value, Distance*, forward_iterator_tag)
{
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle, left, right;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else if (value < *middle) {
            len = half;
        } else {
            left = lower_bound(first, middle, value);
            advance(first, len);
            right = upper_bound(++middle, first, value);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    return pair<ForwardIterator, ForwardIterator>(first, first);
}

template <class ForwardIterator, class T, class Distance, class Compare>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last,
              const T& value, Distance*, forward_iterator_tag, Compare comp)
{
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle, left, right;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (comp(*middle, value)) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else if (comp(value, *middle)) {
            len = half;
        } else {
            left = lower_bound(first, middle, value);
            advance(first, len);
            right = upper_bound(++middle, first, value);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    return pair<ForwardIterator, ForwardIterator>(first, first);
}

// 版本一
template <class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value)
{
    return __equal_range(first, last, value,
                         distance_type(first), iterator_category(first));
}

// 版本二
template <class ForwardIterator, class T, class Compare>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare comp)
{
    return __equal_range(first, last, value,
                         distance_type(first), iterator_category(first), comp);
}

// inplace_merge
// 如果两个连接在一起的序列 [first, middle) 和 [middle, last) 都已排序,
// 那么 inplace_merge 可将它们结合成单一一个序列, 并仍保有序性

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            +first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class BidirectionalIterator, class Distance>
void __merge_without_buffer(BidirectionalIterator first, BidirectionalIterator middle,
                            BidirectionalIterator last, Distance len1, Distance len2)
{
    if (len1 == 0 || len2 == 0) {
        return;
    }
    if (len1 + len2 == 2) {
        if (*middle < *first) {
            iter_swap(first, middle);
        }
        return;
    }
    BidirectionalIterator first_cut = first;
    BidirectionalIterator second_cut = middle;
    Distance len11 = 0;
    Distance len22 = 0;
    if (len1 > len2) {
        len11 = len1 / 2;
        advance(first_cut, len11);
        second_cut = lower_bound(middle, last, *first_cut);
        distance(middle, second_cut, len22);
    } else {
        len22 = len2 / 2;
        advance(second_cut, len22);
        first_cut = upper_bound(first, middle, *second_cut);
        distance(first, first_cut, len11);
    }
    BidirectionalIterator new_middle = rotate(first_cut, middle, second_cut);
    __merge_without_buffer(first, first_cut, new_middle, len11, len22);
    __merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22);
}

template <class BidirectionalIterator, class Distance, class Compare>
void __merge_without_buffer(BidirectionalIterator first, BidirectionalIterator middle,
                            BidirectionalIterator last, Distance len1, Distance len2,
                            Compare comp) {
    if (len1 == 0 || len2 == 0) {
        return;
    }
    if (len1 + len2 == 2) {
        if (comp(*middle, *first)) {
            iter_swap(first, middle);
        }
        return;
    }
    BidirectionalIterator first_cut = first;
    BidirectionalIterator second_cut = middle;
    Distance len11 = 0;
    Distance len22 = 0;
    if (len1 > len2) {
        len11 = len1 / 2;
        advance(first_cut, len11);
        second_cut = lower_bound(middle, last, *first_cut, comp);
        distance(middle, second_cut, len22);
    } else {
        len22 = len2 / 2;
        advance(second_cut, len22);
        first_cut = upper_bound(first, middle, *second_cut, comp);
        distance(first, first_cut, len11);
    }
    BidirectionalIterator new_middle = rotate(first_cut, middle, second_cut);
    __merge_without_buffer(first, first_cut, new_middle, len11, len22, comp);
    __merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
}

template <class BidirectionalIterator1, class BidirectionalIterator2,
          class BidirectionalIterator3>
BidirectionalIterator3 __merge_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                        BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                        BidirectionalIterator3 result) {
    if (first1 == last1) {
        return copy_backward(first2, last2, result);
    }
    if (first2 == last2) {
        return copy_backward(first1, last1, result);
    }
    --last1;
    --last2;
    while (true) {
        if (*last2 < *last1) {
             *--result = *last1;
            if (first1 == last1) {
                return copy_backward(first2, ++last2, result);
            }
            --last1;
        } else {
            *--result = *last2;
            if (first2 == last2) {
                return copy_backward(first1, ++last1, result);
            }
            --last2;
        }
    }
}

template <class BidirectionalIterator1, class BidirectionalIterator2,
          class BidirectionalIterator3, class Compare>
BidirectionalIterator3 __merge_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                        BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                        BidirectionalIterator3 result, Compare comp) {
    if (first1 == last1) {
        return copy_backward(first2, last2, result);
    }
    if (first2 == last2) {
        return copy_backward(first1, last1, result);
    }
    --last1;
    --last2;
    while (true) {
        if (comp(*last2, *last1)) {
            *--result = *last1;
            if (first1 == last1) {
                eturn copy_backward(first2, ++last2, result);
            }
            --last1;
        } else {
            *--result = *last2;
            if (first2 == last2) {
                return copy_backward(first1, ++last1, result);
            }
            --last2;
        }
    }
}

};  // namespace cstl

#endif /* __STL_ALGO_H */
#ifndef __STL_ALGO_H
#define __STL_ALGO_H

#include "stl_algobase.h"

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
InputIterator find(InputIterator first, InputIterator last, Predicate pred)
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

};  // namespace cstl

#endif /* __STL_ALGO_H */
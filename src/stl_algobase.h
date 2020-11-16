#ifndef __STL_ALGOBASE_H
#define __STL_ALGOBASE_H

#include <cstring>

#include "../src/stl_iterator.h"
#include "../src/stl_pair.h"

namespace cstl
{

// 如果两个序列在 [first, last) 区间内相等, equal() 返回 true

template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
    for ( ; first1 != last1; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2, BinaryPredicate binary_pred)
{
    for ( ; first1 != last1; ++first1, ++first2) {
        if (!binary_pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

// fill() 将 [first, last) 内的所有元素改填新值

template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
    for ( ; first != last; ++first) {
        *first = value;
    }
}

// fill_n() 将 [first, last) 内的前 n 个元素改填新值, 返回的迭代器指向被填入的最后一个元素的下一个位置
// 由于每次迭代进行的是 assignment 操作, 是一种覆写 (overwrite) 操作, 所以一旦操作区间
// 超越了容器大小, 就会造成不可预期的结果.
// 解决办法之一是, 利用 inserter() 产生一个具有插入 (insert) 而非覆写 (overwrite) 能力的迭代器,
// inserter() 可产生一个用来修饰迭代器的配接器 (iterator adapter)
// 用法如下:
// fill_n(inserter(iv, iv.begin()), 5, 7);

template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
    for ( ; n > 0; --n, ++first) {
        *first = value;
    }
    return first;
}

// iter_swap() 将两个 ForwardIterator 所指的对象对调

template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
    __iter_swap(a, b, value_type(a));
}

template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

template <class T>
inline const T& max(const T& a, const T& b)
{
    return a < b ? b : a;
}

template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp)
{
    return comp(a, b) ? b : a;
}

template <class T>
inline const T& min(const T& a, const T& b)
{
    return b < a ? b : a;
}

template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp)
{
    return comp(b, a) ? b : a;
}

// lexicographical_compare() 以 "字典排列方式" 对两个序列 [first1, last1) 和 [first2, last2) 进行比较

template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2)
{
    for ( ; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return true;
        }
        if (*first2 < *first1) {
            return false;
        }
    }
    // 如果,第一序列到达尾端而第二序列尚有余额, 那么第一序列小于第二序列
    return first1 == last1 && first2 != last2;
}

template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2, Compare comp)
{
    for ( ; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (comp(*first1, *first2)) {
            return true;
        }
        if (comp(*first2, *first1)) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

inline bool
lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
                        const unsigned char* first2, const unsigned char* last2)
{
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    // 先比较相同长度的一段. memcmp() 速度较快
    const int result = std::memcmp(first1, first2, min(len1, len2));
    // 如果不相上下, 则长度较长者被视为比较大
    return result != 0 ? result < 0 : len1 < len2;
}

// mismatch() 用来平行比较两个序列, 指出两者之间的第一个不匹配点.
// 返回一对迭代器, 分别指向两序列中的不匹配点,
// 如果两序列的所有对应元素都匹配, 返回两序列各自的 last 迭代器

template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                              InputIterator2 first2)
{
    // 如果序列一走完, 就结束
    // 如果序列一和序列二的对应元素相等, 就结束
    // 显然, 序列一个的元素个数必须多过序列二的元素个数, 否则结果无可预期
    while (first1 != last1 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                              InputIterator2 first2, BinaryPredicate binary_pred)
{
    while (first1 != last1 && binary_pred(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}

template <class T>
inline void swap(T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

};  // namespace cstl

#endif /* __STL_ALGOBASE_H */
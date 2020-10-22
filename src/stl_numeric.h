#ifndef __STL_NUMERIC_H
#define __STL_NUMERIC_H

#include "../src/stl_iterator.h"
#include "../src/stl_function.h"

// 算法 accumulate 用来计算 init 和 [first, last) 内所有元素的总和

template <class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init)
{
    for ( ; first != last; ++first) {
        init = init + *first;
    }
    return init;
}

template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init,
             BinaryOperation binary_op)
{
    for ( ; first != last; ++first) {
        init = binary_op(init, *first);
    }
    return init;
}

// 算法 adjacent_difference 用来计算 [first, last) 中相邻元素的差额

template <class InputIterator, class OutputIterator>
OutputIterator
adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
{
    if (first == last) return result;
    *result = *first;
    return __adjacent_difference(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator 
__adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, T*)
{
    T value = *first;
    while (++first != last) {
        T tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator 
adjacent_difference(InputIterator first, InputIterator last, OutputIterator result,
                    BinaryOperation binary_op)
{
    if (first == last) return result;
    *result = *first;
    return __adjacent_difference(first, last, result, value_type(first), binary_op);
}

template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator
__adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, T*,
                      BinaryOperation binary_op)
{
    T value = *first;
    while (++first != last) {
        T tmp = *first;
        *++result = binary_op(tmp, value);
        value = tmp;
    }
    return ++result;
}

// 算法 inner_product 能够计算 [first, last) 和 [first2, first2 + (last1 - first1))
// 的一般内积 (generalized inner product)

template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init)
{
    for ( ; first1 != last1; ++first1, ++first2) {
        init = init + (*first1 * *first2);
    }
    return init;
}

template <class InputIterator1, class InputIterator2, class T,
          class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2, T init, 
                  BinaryOperation1 binary_op1,
                  BinaryOperation2 binary_op2)
{
    for ( ; first1 != last1; ++first1, ++first2) {
        init = binary_op1(init, binary_op2(*first1, *first2));
    }
    return init;
}

// 算法 partial_sum 用来计算局部总和

template <class InputIterator, class OutputIterator>
OutputIterator 
partial_sum(InputIterator first, InputIterator last,
            OutputIterator result)
{
    if (first == last) return result;
    *result = *first;
    return __partial_sum(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator 
__partial_sum(InputIterator first, InputIterator last,
              OutputIterator result, T*)
{
    T value = *first;
    while (++first != last) {
        value = value + *first;
        *++result = value;
    }
    return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator 
partial_sum(InputIterator first, InputIterator last,
            OutputIterator result, BinaryOperation binary_op)
{
    if (first == last) return result;
    *result = *first;
    return __partial_sum(first, last, result, value_type(first), binary_op);
}

template <class InputIterator, class OutputIterator, class T,
          class BinaryOperation>
OutputIterator 
__partial_sum(InputIterator first, InputIterator last, 
              OutputIterator result, T*, BinaryOperation binary_op)
{
    T value = *first;
    while (++first != last) {
        value = binary_op(value, *first);
        *++result = value;
    }
    return ++result;
}

// Alias for the internal name power.  Note that power is an extension,
// not part of the C++ standard.

template <class T, class Integer>
inline T power(T x, Integer n)
{
    return power(x, n, multiplies<T>());    // 指定运算型式为乘法
}

// 如果指定为乘法运算, 则当 n >= 0 时返回 x^n
// 注意, "MonoidOperation" 必须满足结合律(associative), 但不需满足交换律(commutative)
template <class T, class Integer, class MonoidOperation>
T power(T x, Integer n, MonoidOperation opr)
{
    if (n == 0) {
        return identity_element(opr);
    } else {
        while ((n & 1) == 0) {
            n >>= 1;
            x = opr(x, x);
        }

        T result = x;
        n >>= 1;
        while (n != 0) {
            x = opr(x, x);
            if ((n & 1) != 0) {
                result = opr(result, x);
            }
            n >>= 1;
        }
        return result;
    }
}

// iota is not part of the C++ standard.  It is an extension.
// 在 [first, last) 区间内填入 value, value + 1, value + 2...
template <class ForwardIter, class T>
void iota(ForwardIter first, ForwardIter last, T value)
{
    while (first != last) *first++ = value++;
}

#endif
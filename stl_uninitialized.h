#include <algorithm>

#include "stl_construct.h"
#include "type_traits.h"

// uninitialized_fill_n() 函数接受三个参数
// - 迭代器 first 指向欲初始化空间的起始处
// - n 表示欲初始化空间的大小
// - x表示初值
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
                                            Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

// 这个函数的进行逻辑是, 首先萃取出迭代器 first 的 value type, 
// 然后判断该型别是否为 POD 型别:
template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first,
                                              Size n, const T& x, T1*)
{
    // __type_traits<> 详见 3.7 节
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果 copy construction 等同于 assignment, 而且
// destructor 是 trivial, 以下就有效
// 如果是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class ForwardInterator, class Size, class T>
inline ForwardInterator
__uninitialized_fill_n_aux(ForwardInterator first, Size n,
                                                   const T& x, __true_type)
{
    return fill_n(first, n, x);     // 交由高阶函数执行. 见 6.4.2 节
}

// 如果不是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class ForwardIterator, class Size, class T>
ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n,
                           const T& x, __false_type)
{
    ForwardIterator cur = first;
    // 为求阅读顺畅, 以下将原本该有的异常处理省略
    for (; n > 0; --n, ++cur) {
        construct(&*cur, x);
    }
    return cur;
}

// uninitialized_copy() 函数接受三个参数
// - 迭代器 first 指向输入端的起始位置
// - 迭代器 last 指向输入端的结束位置(前闭后开区间)
// - 迭代器 result 指向输出端(欲初始化空间)的起始处
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last,
                   ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
    // 以上, 利用 value_type() 取出 first 的 value type
}

// 这个函数的进行逻辑是, 首先萃取出迭代器 result 的 value type, 
// 然后判断该型别是否为 POD 型别:
template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last,
                     ForwardIterator result, T*)
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
    // 以上, 企图利用 is_POD() 所获得的结果, 让编译器做参数推导
}

// 如果 copy construction 等同于 assignment, 而且
// destructor 是 trivial, 以下就有效
// 如果是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __true_type)
{
    return std::copy(first, last, result);      // 调用 STL 算法 copy()
}

// 如果不是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class InputIterator, class ForwardIterator>
ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __false_type)
{
    ForwardIterator cur = result;
    // 为求阅读顺畅, 以下将原本该有的异常处理省略
    for (; first != last; ++first, ++cur) {
        construct(&*cur, *first);       // 必须一个一个元素地构造, 无法批量进行
    }
    return cur;
}

// 以下是针对 const char* 的特化版本
inline char* uninitialized_copy(const char* first, const char* last,
                                char* result)
{
    std::memmove(result, first, last - first);
    return result + (last - first);
}

// 以下是针对 const wchar_t* 的特化版本
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                   wchar_t* result)
{
    std::memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

// uninitialized_fill() 函数接受三个参数
// - 迭代器 first 指向输出端(欲初始化空间)的起始处
// - 迭代器 last 指向输出端(欲初始化空间)的结束处(前闭后开区间)
// - x 表示初值
template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                               const T& x)
{
    __uninitialized_fill(first, last, x, value_type(first));
}

// 这个函数的进行逻辑是, 首先萃取出迭代器 first 的 value type, 
// 然后判断该型别是否为 POD 型别:
template <class Forwarditerator, class T, class T1>
inline void __uninitialized_fill(Forwarditerator first, Forwarditerator last,
                                 const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_aux(first, last, x, is_POD());
}

// 如果 copy construction 等同于 assignment, 而且
// destructor 是 trivial, 以下就有效
// 如果是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class ForwardIterator, class T>
inline void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                         const T& x, __true_type)
{
    std::fill(first, last, x);      // 调用 STL 算法 fill()
}

// 如果不是 POD 型别, 执行流程就会转进到以下函数. 这是藉由 function template
// 的参数推导机制而得
template <class ForwardIterator, class T>
void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                         const T& x, __false_type)
{
    ForwardIterator cur = first;
    // 为求阅读顺畅, 以下将原本该有的异常处理省略
    for (; cur != last; ++cur) {
        construct(&*cur, x);        // 必须一个一个元素地构造, 无法批量进行
    }
}
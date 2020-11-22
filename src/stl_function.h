/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996-1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_FUNCTION_H
#define __SGI_STL_INTERNAL_FUNCTION_H

#include <cstddef>

#include "../src/stl_config.h"

namespace cstl
{

template <class Arg, class Result>
struct unary_function {
  typedef Arg argument_type;
  typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function {
  typedef Arg1 first_argument_type;
  typedef Arg2 second_argument_type;
  typedef Result result_type;
};      

template <class T>
struct plus : public binary_function<T,T,T> {
  T operator()(const T& x, const T& y) const { return x + y; }
};

template <class T>
struct minus : public binary_function<T,T,T> {
  T operator()(const T& x, const T& y) const { return x - y; }
};

template <class T>
struct multiplies : public binary_function<T,T,T> {
  T operator()(const T& x, const T& y) const { return x * y; }
};

template <class T>
struct divides : public binary_function<T,T,T> {
  T operator()(const T& x, const T& y) const { return x / y; }
};

// identity_element (not part of the C++ standard).

template <class T> inline T identity_element(plus<T>) {
  return T(0);
}
template <class T> inline T identity_element(multiplies<T>) {
  return T(1);
}

template <class T>
struct modulus : public binary_function<T,T,T> 
{
  T operator()(const T& x, const T& y) const { return x % y; }
};

template <class T>
struct negate : public unary_function<T,T> 
{
  T operator()(const T& x) const { return -x; }
};

template <class T>
struct equal_to : public binary_function<T,T,bool> 
{
  bool operator()(const T& x, const T& y) const { return x == y; }
};

template <class T>
struct not_equal_to : public binary_function<T,T,bool> 
{
  bool operator()(const T& x, const T& y) const { return x != y; }
};

template <class T>
struct greater : public binary_function<T,T,bool> 
{
  bool operator()(const T& x, const T& y) const { return x > y; }
};

template <class T>
struct less : public binary_function<T,T,bool> 
{
  bool operator()(const T& x, const T& y) const { return x < y; }
};

template <class T>
struct greater_equal : public binary_function<T,T,bool>
{
  bool operator()(const T& x, const T& y) const { return x >= y; }
};

template <class T>
struct less_equal : public binary_function<T,T,bool> 
{
  bool operator()(const T& x, const T& y) const { return x <= y; }
};

template <class T>
struct logical_and : public binary_function<T,T,bool>
{
  bool operator()(const T& x, const T& y) const { return x && y; }
};

template <class T>
struct logical_or : public binary_function<T,T,bool>
{
  bool operator()(const T& x, const T& y) const { return x || y; }
};

template <class T>
struct logical_not : public unary_function<T,bool>
{
  bool operator()(const T& x) const { return !x; }
};

// 以下配接器用来表示某个 Adaptable Predicate 的逻辑负值
template <class Predicate>
class unary_negate : public unary_function<typename Predicate::argument_type, bool> {
protected:
    Predicate pred;
public:
    explicit unary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::argument_type& x) const
    {
        return !pred(x);    // 将 pred 的运算结果加上否定运算
    }
};


// 辅助函数, 使我们得以方便使用 unary_negate<Pred>
template <class Predicate>
inline unary_negate<Predicate> not1(const Predicate& pred)
{
    return unary_negate<Predicate>(pred);
}

// 以下配接器用来表示某个 Adaptable Binary Predicate 的逻辑负值
template <class Predicate> 
class binary_negate : public binary_function<typename Predicate::first_argument_type,
                                             typename Predicate::second_argument_type, bool> {
protected:
    Predicate pred;
public:
    explicit binary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::first_argument_type& x, 
                    const typename Predicate::second_argument_type& y) const
    {
        return !pred(x, y); 
    }
};

template <class Predicate>
inline binary_negate<Predicate> not2(const Predicate& pred)
{
    return binary_negate<Predicate>(pred);
}

// 以下配接器用来将某个 Adaptable Binary function 转换为 Unary Function
template <class Operation> 
class binder1st : public unary_function<typename Operation::second_argument_type,
                                        typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::first_argument_type value;
public:
    binder1st(const Operation& x, const typename Operation::first_argument_type& y)
        : op(x), value(y) { }
    typename Operation::result_type
        operator()(const typename Operation::second_argument_type& x) const
    {
        return op(value, x);    // 实际调用表达式, 并将 value 绑定为第一参数
    }
};

// 辅助函数, 让我们得以方便使用 binder1st<Op>
template <class Operation, class T> inline binder1st<Operation> 
bind1st(const Operation& fn, const T& x) 
{
    typedef typename Operation::first_argument_type arg1_type;
    return binder1st<Operation>(fn, arg1_type(x));
    // 以上, 先把 x 转型为 op 的第一参数型别
}

template <class Operation> 
class binder2nd : public unary_function<typename Operation::first_argument_type,
                                        typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::second_argument_type value;
public:
    binder2nd(const Operation& x, const typename Operation::second_argument_type& y) 
        : op(x), value(y) { }
    typename Operation::result_type
    operator()(const typename Operation::first_argument_type& x) const
    {
        return op(x, value);    // 实际调用表达式, 并将 value 绑定为第二参数
    }
};

template <class Operation, class T>
inline binder2nd<Operation> bind2nd(const Operation& fn, const T& x) 
{
    typedef typename Operation::second_argument_type arg2_type;
    return binder2nd<Operation>(fn, arg2_type(x));
    // 以上, 先把 x 转型为 op 的第二参数型别

}

// unary_compose and binary_compose (extensions, not part of the standard).

// 已知两个 Adaptable Unary Functions f(), g(), 以下配接器用来产生一个 h(),
// 使 h(x) = f(g(x))
template <class Operation1, class Operation2>
class unary_compose : public unary_function<typename Operation2::argument_type,
                                            typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation2& y) : op1(x), op2(y) { }
    typename Operation1::result_type
        operator()(const typename Operation2::argument_type& x) const
    {
        return op1(op2(x));
    }
};
 
// 辅助函数, 让我们得以方便使用 unary_compose<Op1, Op2>
template <class Operation1, class Operation2>
inline unary_compose<Operation1,Operation2>
compose1(const Operation1& op1, const Operation2& op2)
{
    return unary_compose<Operation1,Operation2>(op1, op2);
}

// 已知一个 Adaptable Binary Function f 和两个 Adaptable Unary Function g1, g2.
// 以下配接器用来产生一个 h, 使 h(x) = f(g1(x), g2(x))
template <class Operation1, class Operation2, class Operation3>
class binary_compose : public unary_function<typename Operation2::argument_type,
                                             typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, const Operation3& z) 
        : op1(x), op2(y), op3(z) { }
    typename Operation1::result_type
    operator()(const typename Operation2::argument_type& x) const
    {
        return op1(op2(x), op3(x));
    }
};

template <class Operation1, class Operation2, class Operation3>
inline binary_compose<Operation1, Operation2, Operation3> 
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3)
{
    return binary_compose<Operation1,Operation2,Operation3>(op1, op2, op3);
}

// 以下配接器其实就是把一个一元函数指针包起来;
// 当仿函数被使用时, 就调用该函数指针
template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result> {
protected:
    Result (*ptr)(Arg);     // 一个函数指针
public:
    pointer_to_unary_function() { }
    // 以下 ctor 将函数指针记录于内部成员之中
    explicit pointer_to_unary_function(Result (*x)(Arg)) : ptr(x) { }

    // 以下, 通过函数指针执行函数
    Result operator()(Arg x) const { return ptr(x); }
};


// 辅助函数, 让我们得以方便运用 pointer_to_unary_function
template <class Arg, class Result>
inline pointer_to_unary_function<Arg, Result> ptr_fun(Result (*x)(Arg))
{
    return pointer_to_unary_function<Arg, Result>(x);
}

// 以下配接器其实就是把一个二元函数指针包起来;
// 当仿函数被使用时, 就调用该函数指针
template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1,Arg2,Result> {
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    pointer_to_binary_function() {}
    explicit pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) { }
    Result operator()(Arg1 x, Arg2 y) const { return ptr(x, y); }
};

template <class Arg1, class Arg2, class Result>
inline pointer_to_binary_function<Arg1,Arg2,Result> ptr_fun(Result (*x)(Arg1, Arg2))
{
    return pointer_to_binary_function<Arg1,Arg2,Result>(x);
}

// identity is an extensions: it is not part of the standard.
template <class T>
struct _Identity : public unary_function<T,T> {
  const T& operator()(const T& x) const { return x; }
};

template <class T> struct identity : public _Identity<T> {};

// select1st and select2nd are extensions: they are not part of the standard.
template <class _Pair>
struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> {
  const typename _Pair::first_type& operator()(const _Pair& x) const {
    return x.first;
  }
};

template <class _Pair>
struct _Select2nd : public unary_function<_Pair, typename _Pair::second_type>
{
  const typename _Pair::second_type& operator()(const _Pair& x) const {
    return x.second;
  }
};

template <class _Pair> struct select1st : public _Select1st<_Pair> {};
template <class _Pair> struct select2nd : public _Select2nd<_Pair> {};

// project1st and project2nd are extensions: they are not part of the standard
template <class Arg1, class Arg2>
struct _Project1st : public binary_function<Arg1, Arg2, Arg1> {
  Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

template <class Arg1, class Arg2>
struct _Project2nd : public binary_function<Arg1, Arg2, Arg2> {
  Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};

template <class Arg1, class Arg2> 
struct project1st : public _Project1st<Arg1, Arg2> {};

template <class Arg1, class Arg2>
struct project2nd : public _Project2nd<Arg1, Arg2> {};

// constant_void_fun, constant_unary_fun, and constant_binary_fun are
// extensions: they are not part of the standard.  (The same, of course,
// is true of the helper functions constant0, constant1, and constant2.)

template <class Result>
struct _Constant_void_fun {
  typedef Result result_type;
  result_type val;

  _Constant_void_fun(const result_type& v) : val(v) {}
  const result_type& operator()() const { return val; }
};  

template <class Result, class _Argument>
struct _Constant_unary_fun {
  typedef _Argument argument_type;
  typedef  Result  result_type;
  result_type val;

  _Constant_unary_fun(const result_type& v) : val(v) {}
  const result_type& operator()(const _Argument&) const { return val; }
};

template <class Result, class Arg1, class Arg2>
struct _Constant_binary_fun {
  typedef  Arg1   first_argument_type;
  typedef  Arg2   second_argument_type;
  typedef  Result result_type;
  Result val;

  _Constant_binary_fun(const Result& v) : val(v) {}
  const result_type& operator()(const Arg1&, const Arg2&) const {
    return val;
  }
};

template <class Result>
struct constant_void_fun : public _Constant_void_fun<Result> {
  constant_void_fun(const Result& v) : _Constant_void_fun<Result>(v) {}
};  


template <class Result,
          class _Argument __STL_DEPENDENT_DEFAULT_TMPL(Result)>
struct constant_unary_fun : public _Constant_unary_fun<Result, _Argument>
{
  constant_unary_fun(const Result& v)
    : _Constant_unary_fun<Result, _Argument>(v) {}
};


template <class Result,
          class Arg1 __STL_DEPENDENT_DEFAULT_TMPL(Result),
          class Arg2 __STL_DEPENDENT_DEFAULT_TMPL(Arg1)>
struct constant_binary_fun
  : public _Constant_binary_fun<Result, Arg1, Arg2>
{
  constant_binary_fun(const Result& v)
    : _Constant_binary_fun<Result, Arg1, Arg2>(v) {}
};

template <class Result>
inline constant_void_fun<Result> constant0(const Result& __val)
{
  return constant_void_fun<Result>(__val);
}

template <class Result>
inline constant_unary_fun<Result,Result> constant1(const Result& __val)
{
  return constant_unary_fun<Result,Result>(__val);
}

template <class Result>
inline constant_binary_fun<Result,Result,Result> 
constant2(const Result& __val)
{
  return constant_binary_fun<Result,Result,Result>(__val);
}

// subtractive_rng is an extension: it is not part of the standard.
// Note: this code assumes that int is 32 bits.
class subtractive_rng : public unary_function<unsigned int, unsigned int> {
private:
  unsigned int _M_table[55];
  size_t _M_index1;
  size_t _M_index2;
public:
  unsigned int operator()(unsigned int __limit) {
    _M_index1 = (_M_index1 + 1) % 55;
    _M_index2 = (_M_index2 + 1) % 55;
    _M_table[_M_index1] = _M_table[_M_index1] - _M_table[_M_index2];
    return _M_table[_M_index1] % __limit;
  }

  void _M_initialize(unsigned int __seed)
  {
    unsigned int __k = 1;
    _M_table[54] = __seed;
    size_t __i;
    for (__i = 0; __i < 54; __i++) {
        size_t __ii = (21 * (__i + 1) % 55) - 1;
        _M_table[__ii] = __k;
        __k = __seed - __k;
        __seed = _M_table[__ii];
    }
    for (int __loop = 0; __loop < 4; __loop++) {
        for (__i = 0; __i < 55; __i++)
            _M_table[__i] = _M_table[__i] - _M_table[(1 + __i + 30) % 55];
    }
    _M_index1 = 0;
    _M_index2 = 31;
  }

  subtractive_rng(unsigned int __seed) { _M_initialize(__seed); }
  subtractive_rng() { _M_initialize(161803398u); }
};


// Adaptor function objects: pointers to member functions.

// There are a total of 16 = 2^4 function objects in this family.
//  (1) Member functions taking no arguments vs member functions taking
//       one argument.
//  (2) Call through pointer vs call through reference.
//  (3) Member function with void return type vs member function with
//      non-void return type.
//  (4) Const vs non-const member function.

// Note that choice (3) is nothing more than a workaround: according
//  to the draft, compilers should handle void and non-void the same way.
//  This feature is not yet widely implemented, though.  You can only use
//  member functions returning void if your compiler supports partial
//  specialization.

// All of this complexity is in the function objects themselves.  You can
//  ignore it by using the helper function mem_fun and mem_fun_ref,
//  which create whichever type of adaptor is appropriate.
//  (mem_fun1 and mem_fun1_ref are no longer part of the C++ standard,
//  but they are provided for backward compatibility.)

// "无任何参数", "通过 pointer 调用", "non-const 成员函数"
template <class Ret, class T>
class mem_fun_t : public unary_function<T*,Ret> {
public:
    explicit mem_fun_t(Ret (T::*pf)()) : f(pf) { }
    Ret operator()(T* p) const { return (p->*f)(); }
private:
    Ret (T::*f)();
};

// "无任何参数", "通过 pointer 调用", "const 成员函数"
template <class Ret, class T>
class const_mem_fun_t : public unary_function<const T*,Ret> {
public:
    explicit const_mem_fun_t(Ret (T::*pf)() const) : f(pf) { }
    Ret operator()(const T* p) const { return (p->*f)(); }
private:
    Ret (T::*f)() const;
};


// "无任何参数", "通过 reference 调用", "non-const 成员函数"
template <class Ret, class T>
class mem_fun_ref_t : public unary_function<T,Ret> {
public:
    explicit mem_fun_ref_t(Ret (T::*pf)()) : f(pf) { }
    Ret operator()(T& r) const { return (r.*f)(); }
private:
    Ret (T::*f)();
};

// "无任何参数", "通过 reference 调用", "const 成员函数"
template <class Ret, class T>
class const_mem_fun_ref_t : public unary_function<T, Ret> {
public:
    explicit const_mem_fun_ref_t(Ret (T::*pf)() const) : f(pf) { }
    Ret operator()(const T& r) const { return (r.*f)(); }
private:
    Ret (T::*f)() const;
};

// "有一个参数", "通过 pointer 调用", "non-const 成员函数"
template <class Ret, class T, class Arg>
class mem_fun1_t : public binary_function<T*,Arg, Ret> {
public:
    explicit mem_fun1_t(Ret (T::*pf)(Arg)) : f(pf) { }
    Ret operator()(T* p, Arg x) const { return (p->*f)(x); }
private:
    Ret (T::*f)(Arg);
};

// "有一个参数", "通过 pointer 调用", "const 成员函数"
template <class Ret, class T, class Arg>
class const_mem_fun1_t : public binary_function<const T*,Arg, Ret> {
public:
    explicit const_mem_fun1_t(Ret (T::*pf)(Arg) const) : f(pf) { }
    Ret operator()(const T* p, Arg x) const { return (p->*f)(x); }
private:
    Ret (T::*f)(Arg) const;
};

// "有一个参数", "通过 reference 调用", "non-const 成员函数"
template <class Ret, class T, class Arg>
class mem_fun1_ref_t : public binary_function<T,Arg, Ret> {
public:
    explicit mem_fun1_ref_t(Ret (T::*pf)(Arg)) : f(pf) { }
    Ret operator()(T& r, Arg x) const { return (r.*f)(x); }
private:
    Ret (T::*f)(Arg);
};

// "有一个参数", "通过 reference 调用", "const 成员函数"
template <class Ret, class T, class Arg>
class const_mem_fun1_ref_t : public binary_function<T,Arg, Ret> {
public:
    explicit const_mem_fun1_ref_t(Ret (T::*pf)(Arg) const) : f(pf) { }
    Ret operator()(const T& r, Arg x) const { return (r.*f)(x); }
private:
    Ret (T::*f)(Arg) const;
};

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class T>
class mem_fun_t<void, T> : public unary_function<T*,void> {
public:
  explicit mem_fun_t(void (T::*pf)()) : f(pf) {}
  void operator()(T* p) const { (p->*f)(); }
private:
  void (T::*f)();
};

template <class T>
class const_mem_fun_t<void, T> : public unary_function<const T*,void> {
public:
  explicit const_mem_fun_t(void (T::*pf)() const) : f(pf) {}
  void operator()(const T* p) const { (p->*f)(); }
private:
  void (T::*f)() const;
};

template <class T>
class mem_fun_ref_t<void, T> : public unary_function<T,void> {
public:
  explicit mem_fun_ref_t(void (T::*pf)()) : f(pf) {}
  void operator()(T& r) const { (r.*f)(); }
private:
  void (T::*f)();
};

template <class T>
class const_mem_fun_ref_t<void, T> : public unary_function<T,void> {
public:
  explicit const_mem_fun_ref_t(void (T::*pf)() const) : f(pf) {}
  void operator()(const T& r) const { (r.*f)(); }
private:
  void (T::*f)() const;
};

template <class T, class Arg>
class mem_fun1_t<void, T, Arg> : public binary_function<T*,Arg,void> {
public:
  explicit mem_fun1_t(void (T::*pf)(Arg)) : f(pf) {}
  void operator()(T* p, Arg x) const { (p->*f)(x); }
private:
  void (T::*f)(Arg);
};

template <class T, class Arg>
class const_mem_fun1_t<void, T, Arg> 
  : public binary_function<const T*,Arg,void> {
public:
  explicit const_mem_fun1_t(void (T::*pf)(Arg) const) : f(pf) {}
  void operator()(const T* p, Arg x) const { (p->*f)(x); }
private:
  void (T::*f)(Arg) const;
};

template <class T, class Arg>
class mem_fun1_ref_t<void, T, Arg>
  : public binary_function<T,Arg,void> {
public:
  explicit mem_fun1_ref_t(void (T::*pf)(Arg)) : f(pf) {}
  void operator()(T& r, Arg x) const { (r.*f)(x); }
private:
  void (T::*f)(Arg);
};

template <class T, class Arg>
class const_mem_fun1_ref_t<void, T, Arg>
  : public binary_function<T,Arg,void> {
public:
  explicit const_mem_fun1_ref_t(void (T::*pf)(Arg) const) : f(pf) {}
  void operator()(const T& r, Arg x) const { (r.*f)(x); }
private:
  void (T::*f)(Arg) const;
};

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Mem_fun adaptor helper functions.  There are only two:
//  mem_fun and mem_fun_ref.  (mem_fun1 and mem_fun1_ref 
//  are provided for backward compatibility, but they are no longer
//  part of the C++ standard.)

template <class Ret, class T>
inline mem_fun_t<Ret,T> mem_fun(Ret (T::*f)())
  { return mem_fun_t<Ret,T>(f); }

template <class Ret, class T>
inline const_mem_fun_t<Ret,T> mem_fun(Ret (T::*f)() const)
  { return const_mem_fun_t<Ret,T>(f); }

template <class Ret, class T>
inline mem_fun_ref_t<Ret,T> mem_fun_ref(Ret (T::*f)()) 
  { return mem_fun_ref_t<Ret,T>(f); }

template <class Ret, class T>
inline const_mem_fun_ref_t<Ret,T> mem_fun_ref(Ret (T::*f)() const)
  { return const_mem_fun_ref_t<Ret,T>(f); }

template <class Ret, class T, class Arg>
inline mem_fun1_t<Ret,T,Arg> mem_fun(Ret (T::*f)(Arg))
  { return mem_fun1_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline const_mem_fun1_t<Ret,T,Arg> mem_fun(Ret (T::*f)(Arg) const)
  { return const_mem_fun1_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline mem_fun1_ref_t<Ret,T,Arg> mem_fun_ref(Ret (T::*f)(Arg))
  { return mem_fun1_ref_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline const_mem_fun1_ref_t<Ret,T,Arg>
mem_fun_ref(Ret (T::*f)(Arg) const)
  { return const_mem_fun1_ref_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline mem_fun1_t<Ret,T,Arg> mem_fun1(Ret (T::*f)(Arg))
  { return mem_fun1_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline const_mem_fun1_t<Ret,T,Arg> mem_fun1(Ret (T::*f)(Arg) const)
  { return const_mem_fun1_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline mem_fun1_ref_t<Ret,T,Arg> mem_fun1_ref(Ret (T::*f)(Arg))
  { return mem_fun1_ref_t<Ret,T,Arg>(f); }

template <class Ret, class T, class Arg>
inline const_mem_fun1_ref_t<Ret,T,Arg>
mem_fun1_ref(Ret (T::*f)(Arg) const)
  { return const_mem_fun1_ref_t<Ret,T,Arg>(f); }

} // namespace cstl

#endif /* __SGI_STL_INTERNAL_FUNCTION_H */

// Local Variables:
// mode:C++
// End:

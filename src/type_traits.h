#ifndef __TYPE_TRAITS_H
#define __TYPE_TRAITS_H

#include "stl_config.h"

struct __true_type { };
struct __false_type { };

template <class type>
struct __type_traits {
    typedef __true_type this_dummy_member_must_be_first;
    /*
    不要移除这个成员. 它通知"有能力自动将 __type_traits 特化"的编译器说, 我们现在所看到
    的这个 __type_traits template 是特殊的. 这是为了确保万一编译器也使用一个名为
    __type__traits 而其实与此处定义并无任何关联的 template 时, 所有事情都仍将顺利运作
    */

   /*
   以下条件应被遵守, 因为编译器有可能自动为各型别产生专属的 __type_traits特化版本:
        - 你可以重新排列以下的成员次序
        - 你可以移除以下任何成员
        - 绝对不可以将以下成员重新命名而却没有改变编译器中的对应名称
        - 新加入的成员会被视为一般成员, 除非你在编译器中加上适当支持
   */

  typedef __false_type has_trivial_default_constructor;
  typedef __false_type has_trivial_copy_constructor;
  typedef __false_type has_trivial_assignment_operator;
  typedef __false_type has_trivial_destructor;
  typedef __false_type is_POD_type;
};

/*
以下针对 C++ 基本型别 char, signed char, unsigned char, short, unsigned
short, int, unsigned int, long, unsigned long, float, double, long
double 提供特化版本. 注意, 每一个成员的值都是 __true_type, 表示这些型别都可
采用最快速方式(例如 memcpy)来进行拷贝或赋值操作
*/

// 注意, SGI STL <stl_config.h> 将以下出现的 __STL_TEMPLATE_NULL
// 定义为 template<>, 见 1.9.1 节, 是所谓的
// class template explicit specialization

__STL_TEMPLATE_NULL struct __type_traits<char> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<signed char> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<unsigned char> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<short> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<unsigned short> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<int> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<unsigned int> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<long> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<unsigned long> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<float> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<double> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

__STL_TEMPLATE_NULL struct __type_traits<long double> {
   typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

// 注意, 以下针对原生指针设计 __type_traits 偏特化版本
// 原生指针亦被视为一种标量型别
template <class T>
struct __type__traits<T*> {
    typedef __true_type    has_trivial_default_constructor;
   typedef __true_type    has_trivial_copy_constructor;
   typedef __true_type    has_trivial_assignment_operator;
   typedef __true_type    has_trivial_destructor;
   typedef __true_type    is_POD_type;
};

#endif
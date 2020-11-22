#ifndef __STL_ITERATOR_H
#define __STL_ITERATOR_H

#include <iostream>
#include <cstddef>      // for ptrdiff_t

#include "stl_config.h"

namespace cstl
{

// 五种迭代器类型
struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };

// 为避免写代码时挂一漏万, 自行开发的迭代器最好继承自下面这个 std::iterator
template <class Category, class T, class Distance = ptrdiff_t,
          class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category  iterator_category;
    typedef T         value_type;
    typedef Distance  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
};

// "榨汁机" traits
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
};

// 针对原生指针(native pointer)而设计的 traits 偏特化版
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

// 针对原生之 pointer-to-const 而设计的 traits 偏特化版
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef const T*                   pointer;
    typedef const T&                   reference;
};

// 这个函数可以很方便的决定某个迭代器的类型(category)
template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// 这个函数可以很方便的决定某个迭代器的 distance type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 这个函数可以很方便的决定某个迭代器的 value type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 以下是整组 distance 函数
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first; ++n;
    }
    return n;
}

template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

// 以下是整组 advance 函数
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, bidirectional_iterator_tag)
{
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_category(i));
}

#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class BidirectionalIterator, class T, class Reference = T&, 
          class Distance = ptrdiff_t> 
#else
template <class BidirectionalIterator, class T, class Reference, 
          class Distance> 
#endif
class reverse_bidirectional_iterator {
    typedef reverse_bidirectional_iterator<BidirectionalIterator, T, Reference, Distance> self;
protected:
    BidirectionalIterator current;
public:
    typedef bidirectional_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Reference                  reference;

    reverse_bidirectional_iterator() {}
    explicit reverse_bidirectional_iterator(BidirectionalIterator x)
        : current(x) { }
    BidirectionalIterator base() const { return current; }
    Reference operator*() const
    {
        BidirectionalIterator tmp = current;
        return *--tmp;
    }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
    self& operator++()
    {
        --current;
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        --current;
        return tmp;
    }
    self& operator--()
    {
        ++current;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }
};

// 这是一个迭代器配接器(iterator adapter), 用来将某个迭代器逆反前进方向,
// 使前进为后退, 后退为前进
template <class Iterator>
class reverse_iterator {
protected:
    Iterator current;       // 记录对应的正向迭代器
public:
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type        value_type;
    typedef typename iterator_traits<Iterator>::difference_type   difference_type;
    typedef typename iterator_traits<Iterator>::pointer           pointer;
    typedef typename iterator_traits<Iterator>::reference         reference;

    typedef Iterator iterator_type;             // 代表正向迭代器
    typedef reverse_iterator<Iterator> self;    // 代表逆向迭代器

public:
    reverse_iterator() {}
    // 下面这个 ctor 将 reverse_iterator 与某个迭代器 x 系结起来
    explicit reverse_iterator(iterator_type x) : current(x) {}

    reverse_iterator(const self& x) : current(x.current) {}
#ifdef __STL_MEMBER_TEMPLATES
    template <class _Iter>
    reverse_iterator(const reverse_iterator<_Iter>& x) : current(x.base()) {}
#endif /* __STL_MEMBER_TEMPLATES */
    
    iterator_type base() const { return current; }
    reference operator*() const
    {
        Iterator tmp = current;
        return *--tmp;
        // 以上为关键所在. 对逆向迭代器取值, 就是将 "对应的正迭代器" 后退一格而后取值
    }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }   // 意义同上
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

    self& operator++()
    {
        --current;
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        --current;
        return tmp;
    }
    self& operator--()
    {
        ++current;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self operator+(difference_type n) const
    {
        return self(current - n);
    }
    self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }
    self operator-(difference_type n) const
    {
        return self(current + n);
    }
    self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }
    // 注意, 下面第一个 * 和唯一一个 + 都会调用本类的 operator* 和 operator+, 第二个 * 则不会
    reference operator[](difference_type n) const { return *(*this + n); }  
}; 
 
template <class Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x, 
                       const reverse_iterator<Iterator>& y)
{
    return x.base() == y.base();
}

template <class Iterator>
inline bool operator<(const reverse_iterator<Iterator>& x, 
                      const reverse_iterator<Iterator>& y)
{
    return y.base() < x.base();
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class Iterator>
inline bool operator!=(const reverse_iterator<Iterator>& x, 
                       const reverse_iterator<Iterator>& y)
{
    return !(x == y);
}

template <class Iterator>
inline bool operator>(const reverse_iterator<Iterator>& x, 
                      const reverse_iterator<Iterator>& y)
{
    return y < x;
}

template <class Iterator>
inline bool operator<=(const reverse_iterator<Iterator>& x, 
                       const reverse_iterator<Iterator>& y)
{
    return !(y < x);
}

template <class Iterator>
inline bool operator>=(const reverse_iterator<Iterator>& x,
                       const reverse_iterator<Iterator>& y)
{
    return !(x < y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

template <class Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
    return y.base() - x.base();
}

template <class Iterator>
inline reverse_iterator<Iterator> 
operator+(typename reverse_iterator<Iterator>::difference_type n, 
          const reverse_iterator<Iterator>& x)
{
    return reverse_iterator<Iterator>(x.base() - n);
}

// 这是一个迭代器配接器(iterator adapter), 用来将某个迭代器的赋值(assign)
// 操作修改为插入(insert)操作 -- 从容器的尾端插入进去
template <class Container>
class back_insert_iterator {
protected:
    Container* container;
public:
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    // 下面这个 ctor 使 back_insert_iterator 与容器绑定起来
    explicit back_insert_iterator(Container& x) : container(&x) { }
    back_insert_iterator<Container>& operator=(const typename Container::value_type& value)
    {
        container->push_back(value);
        return *this;
    }

    // 以下三个操作符对 back_insert_iterator 不起作用 (关闭功能)
    // 三个操作符返回的都是 back_insert_iterator 自己
    back_insert_iterator<Container>& operator*() { return *this; }
    back_insert_iterator<Container>& operator++() { return *this; }
    back_insert_iterator<Container>& operator++(int) { return *this; }
};

// 这是一个辅助函数, 帮助我们方便使用 back_insert_iterator
template <class Container>
inline back_insert_iterator<Container> back_inserter(Container& x)
{
    return back_insert_iterator<Container>(x);
}

// 这是一个迭代器配接器(iterator adapter), 用来将某个迭代器的赋值(assign)
// 操作修改为插入(insert)操作 -- 从容器的头端插入进去
// 注意, 该迭代器不适用于 vector, 因为 vector 没有提供 push_front 函数
template <class Container>
class front_insert_iterator {
protected:
    Container* container;
public:
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    explicit front_insert_iterator(Container& x) : container(&x) { }
    front_insert_iterator<Container>& operator=(const typename Container::value_type& value)
    {
        container->push_front(value);
        return *this;
    }

    // 以下三个操作符对 front_insert_iterator 不起作用 (关闭功能)
    // 三个操作符返回的都是 front_insert_iterator 自己
    front_insert_iterator<Container>& operator*() { return *this; }
    front_insert_iterator<Container>& operator++() { return *this; }
    front_insert_iterator<Container>& operator++(int) { return *this; }
};

// 这是一个辅助函数, 帮助我们方便使用 front_insert_iterator
template <class Container>
inline front_insert_iterator<Container> front_inserter(Container& x)
{
    return front_insert_iterator<Container>(x);
}

// 这是一个迭代器配接器(iterator adapter), 用来将某个迭代器的赋值(assign)
// 操作修改为插入(insert)操作, 在指定的位置上进行, 并将迭代器右移一个位置,
// 如此便可很方便地连续执行 "表面上是赋值(覆写)而实际上是插入" 的操作
template <class Container>
class insert_iterator {
protected:
    Container* container;
    typename Container::iterator iter;
public:
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    insert_iterator(Container& x, typename Container::iterator i) : container(&x), iter(i) { }
    insert_iterator<Container>& operator=(const typename Container::value_type& value)
    {
        iter = container->insert(iter, value);  // 这里是关键, 转而调用 insert()
        ++iter;                                 // 使 insert iterator 永远随其目标贴身移动
        return *this;
    }

    // 以下三个操作符对 insert_iterator 不起作用 (关闭功能)
    // 三个操作符返回的都是 insert_iterator 自己
    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }
};

// 这是一个辅助函数, 帮助我们方便使用 insert_iterator
template <class Container, class Iterator>
inline insert_iterator<Container> inserter(Container& x, Iterator i)
{
    typedef typename Container::iterator iter;
    return insert_iterator<Container>(x, iter(i));
}

// 这是一个 input iterator, 能够为 "来自某一 basic_istream" 的对象执行格式化输入操作.
template <class T, class Distance = ptrdiff_t>
class istream_iterator {
    friend bool operator== __STL_NULL_TMPL_ARGS (const istream_iterator<T, Distance>& x,
                                                 const istream_iterator<T, Distance>& y);
protected:
    std::istream* stream;
    T value;
    bool end_market;
    void read()
    {
        end_market = (*stream) ? true : false;
        if (end_market) *stream >> value;
        // 以上, 输入之后, stream 的状态可能改变, 所以下面再判断一次以决定 end_marker
        // 当读到 eof 或读到型别不符的资料, stream 即处于 false 状态
        end_market = (*stream) ? true : false;
    }
public:
    typedef input_iterator_tag iterator_category;
    typedef T                  value_type;
    typedef Distance           difference_type;
    typedef const T*           pointer;
    typedef const T&           reference;
    // 以上, 因身为 input iterator, 所以采用 const 比较保险

    istream_iterator() : stream(&std::cin), end_market(false) { }
    istream_iterator(std::istream& s) : stream(&s) { read(); }

    reference operator*() const { return value; }
    pointer operator->() const { return &(operator*()); }

    istream_iterator<T, Distance>& operator++()
    {
        read();
        return *this;
    }

    istream_iterator<T, Distance>& operator++(int)
    {
        istream_iterator<T, Distance> tmp = *this;
        read();
        return tmp;
    }
}; /* istream_iterator */

// 这是一个 output iterator, 能够将对象格式化输出到某个 basic_istream 上
template <class T>
class ostream_iterator {
protected:
    std::ostream* stream;
    const char* string;     // 每次输出后的间符号

public:
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    ostream_iterator(std::ostream& s) : stream(&s), string(0) { }
    ostream_iterator(std::ostream& s, const char* c) : stream(&s), string(c) { }

    // 对迭代器做赋值操作, 就代表要输出一笔资料
    ostream_iterator<T>& operator=(const T& value)
    {
        *stream << value;               // 输出数据
        if (string) *stream << string;  // 如果间隔符号不为空, 输出间隔符号
        return *this;
    }

    ostream_iterator<T>& operator*() { return *this; }
    ostream_iterator<T>& operator++() { return *this; }
    ostream_iterator<T>& operator++(int) { return *this; }
}; /* ostream_iterator */

} // namespace cstl

#endif /* __STL_ITERATOR_H */
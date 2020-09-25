#ifndef __STL_ITERATOR_H
#define __STL_ITERATOR_H

#include <cstddef>      // for ptrdiff_t

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
template <class _BidirectionalIterator, class _Tp, class _Reference = _Tp&, 
          class _Distance = ptrdiff_t> 
#else
template <class _BidirectionalIterator, class _Tp, class _Reference, 
          class _Distance> 
#endif
class reverse_bidirectional_iterator {
    typedef reverse_bidirectional_iterator<_BidirectionalIterator, _Tp, 
                                         _Reference, _Distance>  _Self;
protected:
    _BidirectionalIterator current;
public:
    typedef bidirectional_iterator_tag iterator_category;
    typedef _Tp                        value_type;
    typedef _Distance                  difference_type;
    typedef _Tp*                       pointer;
    typedef _Reference                 reference;

    reverse_bidirectional_iterator() {}
    explicit reverse_bidirectional_iterator(_BidirectionalIterator __x)
        : current(__x) {}
    _BidirectionalIterator base() const { return current; }
    _Reference operator*() const
    {
        _BidirectionalIterator __tmp = current;
        return *--__tmp;
    }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
    _Self& operator++()
    {
        --current;
        return *this;
    }
    _Self operator++(int)
    {
        _Self __tmp = *this;
        --current;
        return __tmp;
    }
    _Self& operator--()
    {
        ++current;
        return *this;
    }
    _Self operator--(int)
    {
        _Self __tmp = *this;
        ++current;
        return __tmp;
    }
};

#endif
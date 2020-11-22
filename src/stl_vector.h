#ifndef __STL_VECTOR_H
#define __STL_VECTOR_H

#include <utility>

#include "stl_uninitialized.h"
#include "stl_iterator.h"
#include "stl_alloc.h"

namespace cstl
{

template <class T, class Alloc = alloc>
class vector {
public:
    // vector 的嵌套型别定义
    typedef T                           value_type;
    typedef value_type*                 pointer;
    typedef value_type*                 iterator;       // vector 的迭代器是普通指针
    typedef const value_type*           const_iterator;
    typedef value_type&                 reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    typedef reverse_iterator<iterator> reverse_iterator;

protected:
    // 以下, simple_alloc 是 SGI STL 的空间配置器
    typedef simple_alloc<value_type, Alloc> data_allocator;
    
    iterator start;             // 表示目前使用空间的头
    iterator finish;            // 表示目前使用空间的尾
    iterator end_of_storage;    // 表示目前可用空间的尾

    void insert_aux(iterator position, const T& x);
    void insert(iterator position, size_type n, const T& x);

    void deallocate()
    {
        if (start) {
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }

    void fill_initialize(size_type n, const T& value)
    {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin() { return start;  }
    iterator end()   { return finish; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    size_type size() const { return size_type(end() - begin()); }
    size_type capacity() const 
    {
        return size_type(end_of_storage - begin());
    }
    bool empty() const { return begin() == end(); }
    reference operator[] (size_type n) { return *(begin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) { }
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const T& value) { fill_initialize(n, value); }
    vector(long n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }

    ~vector()
    {
        destory(start, finish);
        deallocate();
    }
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    void push_back(const T& x)
    {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else {
            insert_aux(end(), x);
        }
    }

    void pop_back()
    {
        --finish;
        destory(finish);
    }

    // 清除 [first, last) 中的所有元素
    iterator erase(iterator first, iterator last)
    {
        iterator i = copy(last, finish, first);     // 见第6章
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }

    // 清除某位置上的元素
    iterator erase(iterator position)
    {
        if (position + 1 != end()) {
            copy(position + 1, finish, position);   // 后续元素往前移动
        }
        --finish;
        destory(finish);
        return position;
    }

    void resize(size_type new_size, const T& x)
    {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), x);
        }
    }

    void reserve(size_type n)
    {
        if (capacity() < n) {
            const size_type old_size = size();
            iterator tmp = allocate_and_copy(n, start, finish);
            destroy(start, finish);
            data_allocator::deallocate(start, end_of_storage - start);
            start = tmp;
            finish = tmp + old_size;
            end_of_storage = start + n;
        }
    }

    void swap(vector<T, Alloc>& x) {
        std::swap(start, x.start);
        std::swap(finish, x.finish);
        std::swap(end_of_storage, x.end_of_storage);
    }

    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }

protected:
    // 配置空间并填满内容
    iterator allocate_and_fill(size_type n , const T& x)
    {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }

    iterator allocate_and_copy(size_type n, const_iterator first,
                                            const_iterator last)
    {
        iterator result = data_allocator::allocate(n);
        __STL_TRY {
            uninitialized_copy(first, last, result);
            return result;
        }
        __STL_UNWIND(data_allocator::deallocate(result, n));
    }
};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
    if (finish != end_of_storage) {     // 还有备用空间
        // 在备用空间起始处构造一个元素, 并以 vector 最后一个元素值为其初值
        construct(finish, *(finish - 1));
        // 调整水位
        ++finish;
        T x_copy = x;
        copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    } else {    // 以无备用空间
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        // 以上配置原则: 如果原大小为 0, 则配置 1 (个元素大小);
        // 如果原大小不为 0, 则配置原大小的两倍,
        // 前半段用来放置原数据, 后半段准备用来放置新数据

        iterator new_start = data_allocator::allocate(len); // 实际配置
        iterator new_finish = new_start;
        try {
            // 将原 vector 的内容拷贝到新 vector
            new_finish = uninitialized_copy(start, position, new_start);
            // 为新元素设定初值 x
            construct(new_finish, x);
            // 调整水位
            ++new_finish;
            // 将安插点的原内容也拷贝过来(提示: 本函数也可能被 insert(p, x) 调用)
            new_finish = uninitialized_copy(position, finish, new_finish);
        } catch (...) {
            // "commit or rollback" semantics.
            destory(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        // 析构并释放原 vector
        destroy(begin(), end());
        deallocate();

        // 调整迭代器, 指向新 vector
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

// 从 position 开始, 插入 n 个元素, 元素初值为 x
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
    if (n != 0) {   // 当 n != 0 才进行以下所有操作
        if (size_type(end_of_storage - finish) >= n) {
            // 备用空间大于等于 "新增元素个数"
            T x_copy = x;
            // 以下计算插入点之后的现有元素个数
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                // "插入点之后的现有元素个数" 大于 "新增元素个数"
                uninitialized_copy(finish - n, finish, finish);
                finish += n;    // 将 vector 尾端标记后移
                copy_backward(position, old_finish - n, old_finish);
                fill(position, position + n, x_copy);   // 从插入点开始填入新值
            } else {
                // "插入点之后的现有元素个数" 小于等于 "新增元素个数"
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                fill(position, old_finish, x_copy);
            }
        } else {
            // 备用空间小于 "新增元素个数" (必须配置额外的空间)
            // 首先决定新长度: 旧长度的两倍, 或旧长度 + 新增元素个数
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            // 以下配置新的 vector 空间
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            __STL_TRY {
                // 以下首先将旧 vector 的插入点之前的元素复制到新空间
                new_finish = uninitialized_copy(start, position, new_start);
                // 以下再将新增元素(初值皆为 x )填入新空间
                new_finish = uninitialized_fill_n(new_finish, n, x);
                // 以下再将旧 vector 的插入点之后的元素复制到新空间
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
#ifdef __STL_USE_EXCEPTIONS
            catch(...) {
                // 如有异常发生, 实现"commit or rollbach" semantics
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
#endif /* __STL_USE_EXCEPTIONS */
            // 以下清除并释放旧的 vector
            destroy(start, finish);
            deallocate();
            // 以下调整水位标记
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

} // namespace cstl

#endif /* __STL_VECTOR_H */
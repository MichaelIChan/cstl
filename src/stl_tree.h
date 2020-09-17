#ifndef __STL_TREE_H
#define __STL_TREE_H

#include <utility>

#include "../src/stl_config.h"
#include "../src/stl_alloc.h"
#include "../src/stl_iterator.h"

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;   // 红色为 0
const __rb_tree_color_type __rb_tree_black = true;  // 黑色为 1

struct __rb_tree_node_base {
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;       // 节点颜色, 非红即黑
    base_ptr parent;        // RB 树的许多操作, 必须知道父节点
    base_ptr left;          // 指向坐节点
    base_ptr right;         // 指向右节点

    static base_ptr minimum(base_ptr x)
    {
        while (x->left != 0) x = x->left;       // 一直向左走, 就会找到最小值
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while (x->right != 0) x = x->right;     // 一直向右走, 就会找到最大值
        return x;
    }
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base {
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;  // 节点值
};

// 基层迭代器
struct __rb_tree_base_iterator {
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node;  // 它用来与容器之间产生一个连结关系

    // 以下其实可实现于 operator++ 内, 因为再无他处会调用此函数了
    void increment()
    {
        if (node->right != 0) {         // 如果有右子节点. 状况(1)
            node = node->right;         // 就向右走
            while (node->left != 0) {   // 然后一直往左子树走到底
                node = node->left;      // 即是解答
            }
        } else {                        // 如果没有右子节点. 状况(2)
            base_ptr y = node->parent;  // 找出父节点
            while (node == y->right) {  // 如果现行节点本身是个右子节点
                node = y;               // 就一直上溯, 直到不是右子节点为止
                y = y->parent;
            }
            if (node->right != y) {     // 若此时的右子节点不等于此时的父节点
                node = y;               // 此时的父节点即为解答. 状况(3)
            }
                                        // 否则此时的 node 为解答. 状况(4)
        }
        // 注意, 以上判断"若此时的右子节点不等于此时的父节点", 是为了应付一种特殊情况:
        // 我们欲寻找根节点下一节点, 而恰好根节点无右子节点
        // 当然, 以上特殊做法必须配合 RB-tree 根节点与特殊之 header 之间的特殊关系
    }

    // 以下其实可实现于 operator-- 内, 因为再无他处会调用此函数了
    void decrement()
    {
        if (node->color == __rb_tree_red && node->parent->parent == node) {
            // 如果是红节点, 且父节点的父节点等于自己
            node = node->right;     // 右子节点即为解答. 状况(1)

            // 以上情况发生于 node 为 header 时(亦即 node 为 end() 时)
            // 注意, header 之右子节点即 mostright, 指向整棵树的 max 节点
        } else if (node->left != 0) {       // 如果有左子节点. 状况(2)
            base_ptr y = node->left;        // 令 y 指向左子节点
            while (y->right != 0) {         // 当 y 有右子节点时
                y = y->right;               // 一直往右子节点走到底
            }
            node = y;                       // 最后即为答案
        } else {                            // 即非根节点, 亦无左子节点. 状况(3)
            base_ptr y = node->parent;      // 找出父节点
            while (node == y->left) {       // 当现行节点身为左子节点
                node = y;                   // 一直交替往上走, 直到现行节点不为左子节点
                y = y->parent;
            }
            node = y;                       // 此时之父节点即为答案
        }
    }
};

// RB-tree 的正规迭代器
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator {
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
    typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value>* link_type;

    __rb_tree_iterator() { }
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator& it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

    self& operator++() { increment(); return *this; }
    self operator++(int) { self tmp = *this; increment(); return tmp; }

    self& operator--() { decrement(); return *this; }
    self operator--(int) { self tmp = *this; decrement(); return tmp; }
};

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
protected:
    typedef void* void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef __rb_tree_color_type color_type;
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type& x)
    {
        link_type tmp = get_node();             // 配置空间
        __STL_TRY {
            construct(&tmp->value_field, x);    // 构造内容
        }
        __STL_UNWIND(put_node(tmp));
        return tmp;
    }

    // 复制一个节点(的值和颜色)
    link_type clone_node(link_type x)
    {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }

    void destory_node(link_type p)
    {
        destory(&p->value_field);   // 析构内容
        put_node(p);                // 释放内存
    }

protected:
    // RB-tree 只以三笔数据表现
    size_type node_count;   // 追踪记录树的大小(节点数量)
    link_type header;       // 这是实现上的一个技巧
    Compare key_compare;    // 节点间的键值大小比较准则. 应该会是个 function object

    // 以下三个函数用来方便取得 header 成员
    link_type& root() const { return (link_type&) header->parent; }
    link_type& leftmost() const { return (link_type&) header->left; }
    link_type& rightmost() const { return (link_type&) header->right; }

    // 以下六个函数用来方便取得节点 x 的成员
    static link_type& left(link_type x) { return (link_type&)(x->left); }
    static link_type& right(link_type x) { return (link_type&)(x->right); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent); }
    static link_type& value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) { return KeyOfValue()(value(x)); } 
    static color_type& color(link_type x) { return (color_type&)(x->color); }

    // 以下六个函数用来方便取得节点 x 的成员
    static link_type& left(base_ptr x) { return (link_type&)(x->left); }
    static link_type& right(base_ptr x) { return (link_type&)(x->right); }
    static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
    static link_type& value(base_ptr x) { return ((link_type)x)->value_field; }
    static const Key& key(base_ptr x) { return KeyOfValue()(value(link_type(x))); } 
    static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }

    // 求取极大值和极小值. node class 有实现此功能, 交给它们完成即可
    static link_type minimum(link_type x)
    {
        return (link_type) __rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x)
    {
        return (link_type) __rb_tree_node_base::maximum(x);
    }

public:
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

private:
    iterator __insert(base_ptr x, base_ptr y, const value_type& v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void clear();
    void init()
    {
        header = get_node();            // 产生一个节点空间, 令 header 指向它
        color(header) = __rb_tree_red;  // 令 header 为红色, 用来区分 header 和 root,
                                        // 在 iterator.operator-- 之中
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }

public:     // allocation/deallocation
    rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) { init(); }

    ~rb_tree()
    {
        clear();
        put_node(header);
    }
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

public:     // accessors
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }     // RB 树的起头为最左(最小)节点处
    iterator eng() { return header; }           // RB 树的终点为 header 所指处
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

public:     // insert/erase
    // 将 x 插入到 RB-tree 中 (保持节点值独一无二)
    std::pair<iterator, bool> insert_unique(const value_type& x);
    // 将 x 插入到 RB-tree 中 (允许节点值重复)
    iterator insert_equal(const value_type& x);
};

#endif
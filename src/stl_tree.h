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

inline bool operator==(const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y)
{
    return x.node == y.node;
}

inline bool operator!=(const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y)
{
    return x.node != y.node;
}

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

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
    typedef reverse_bidirectional_iterator<iterator, value_type, reference,
                                         difference_type>
          reverse_iterator; 
    typedef reverse_bidirectional_iterator<const_iterator, value_type,
                                         const_reference, difference_type>
          const_reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */ 

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
    iterator end() { return header; }           // RB 树的终点为 header 所指处
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const
    { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const
    { 
        return const_reverse_iterator(begin());
    }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }
    void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
        std::swap(header, t._M_header);
        std::swap(node_count, t._M_node_count);
        std::swap(key_compare, t._M_key_compare);
    }
    iterator find(const Key& k);

public:     // insert/erase
    // 将 x 插入到 RB-tree 中 (保持节点值独一无二)
    std::pair<iterator, bool> insert_unique(const value_type& x);
    // 将 x 插入到 RB-tree 中 (允许节点值重复)
    iterator insert_equal(const value_type& x);

    void insert_unique(const_iterator first, const_iterator last);
};

// 插入新值: 节点键值允许重复
// 注意, 返回值是一个 RB-tree 迭代器, 指向新增节点
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
{
    link_type y = header;
    link_type x = root();       // 从根节点开始
    while (x != 0) {            // 从根节点开始, 往下寻找适当的插入点
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
        // 以上, 遇 "大" 则往左, 遇 "小于或等于" 则往右
    }
    return __insert(x, y, v);
    // 以上, x 为新增插入点, y 为插入点的父节点, v为新值
}

// 插入新值: 节点键值不允许重复, 若重复则插入无效
// 注意, 返回值是个 pair, 第一个元素是个 RB-tree 迭代器, 指向新增节点, 第二元素表示插入成功与否
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
{
    link_type y = header;
    link_type x = root();       // 从根节点开始
    bool comp = true;
    while (x != 0) {            // 从根节点开始, 往下寻找适当的插入点
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x));    // v 键值是否小于目前节点的键值
        x = comp ? left(x) : right(x);                  // 遇 "大" 则往左, 遇 "小于或等于" 则往右
    }
    // 离开 while 循环之后, y 所指即插入点之父节点(此时的它必为叶节点)

    iterator j = iterator(y);   // 令迭代器 j 指向插入点的父节点 y
    if (comp) {     // 如果离开 while 循环时 comp 为真(表示遇 "大" , 将插入于左侧)
        if ( j == begin()) {    // 如果插入点的父节点为最左节点
            return std::pair<iterator, bool>(__insert(x, y, v), true);
            // 以上, x 为插入点， y 为插入点的父节点, v 为新值
        } else {    // 否则(插入点的父节点不为最左节点)
            --j;    // 调整 j
        }
    }
    if (key_compare(key(j.node), KeyOfValue()(v))) {
        // 新键值不与既有节点的键值重复, 于是以下执行安插操作
        return std::pair<iterator, bool>(__insert(x, y, v), true);
        // 以上, x 为新值插入点, y 为插入点的父节点, v 为新值
    }
    // 进行至此, 表示新值一定与树中键值重复, 那么就不该插入新值
    return std::pair<iterator, bool>(j, false);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    insert_unique(const_iterator first, const_iterator last)
{
    for (; first != last; ++first)
        insert_unique(*first);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    __insert(base_ptr x_, base_ptr y_, const Value& v)
{
    // 参数 x_ 为新值插入点, 参数 y_ 为插入点的父节点, 参数 v 为新值
    link_type x = (link_type) x_;
    link_type y = (link_type) y_;
    link_type z;

    // key_compare 是键值大小比较准则. 应该会是个 function object
    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
        z = create_node(v);     // 产生一个新节点
        left(y) == z;           // 这使得当 y 即为 header 时, leftmost() = z
        if (y == header) {
            root() = z;
            rightmost() = z;
        } else if (y == leftmost()) {   // 如果 y 为最左节点
            leftmost() = z;             // 维护 leftmost(), 使它永远指向最左节点
        }
    } else {
        z = create_node(v);             // 产生一个新节点
        right(y) = z;                   // 令新节点成为插入点的父节点 y 的右子节点
        if (y == rightmost()) {
            rightmost() = z;            // 维护 rightmost(), 使它永远指向最右节点
        }
    }
    parent(z) = y;      // 设定新节点的父节点
    left(z) = 0;        // 设定新节点的左子节点
    right(z) = 0;       // 设定新节点的右子节点
                        // 新节点的颜色将在 __rb_tree_rebalance() 设定(并调整)
    __rb_tree_rebalance(z, header->parent);     // 参数一为新增节点, 参数二为 root
    ++node_count;
    return iterator(z);
}

// 全局函数
// 新节点必为红节点. 如果插入处的父节点亦为红节点, 就违反红黑树规则,
// 此时可能需要做树形旋转(及颜色改变, 在程序其它处)
inline void
__rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
    // x 为旋转点
    __rb_tree_node_base* y = x->right;  // 令 y 为旋转点的右子节点
    x->right = y->left;
    if (y->left != 0) {
        y->left->parent = x;    // 回马枪设定父节点
    }
    y->parent = x->parent;

    // 令 y 完全顶替 x 的地位(必须将 x 对其父节点的关系完全接收过来)
    if ( x == root) {                       // x 为根节点
        root = y;
    } else if (x == x->parent->left) {      // x 为其父节点的左子节点
        x->parent->left = y;
    } else {                                // x 为其父节点的右子节点
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// 全局函数
// 新节点必为红节点. 如果插入处的父节点亦为红节点, 就违反红黑树规则,
// 此时可能需要做树形旋转(及颜色改变, 在程序其它处)
inline void
__rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
    // x 为旋转点
    __rb_tree_node_base* y = x->left;       // y 为旋转点的左子节点
    x->left = y->right;
    if (y->right != 0) {
        y->right->parent = x;   // 回马枪设定父节点
    }
    y->parent = x->parent;

    // 令 y 完全顶替 x 的地位(必须将 x 对其父节点的关系完全接收过来)
    if ( x == root) {                       // x 为根节点
        root = y;
    } else if (x == x->parent->right) {      // x 为其父节点的右子节点
        x->parent->right = y;
    } else {                                // x 为其父节点的左子节点
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

#endif

// 全局函数
// 重新令树形平衡(改变颜色及旋转树形)
// 参数一为新增节点, 参数二为 root
inline void
__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
    x->color = __rb_tree_red;       // 新节点必为红
    while (x != root && x->parent->color == __rb_tree_red) {    // 父节点为红
        if (x->parent == x->parent->parent->left) {             // 父节点为祖父节点的左子节点
            __rb_tree_node_base* y = x->parent->parent->right;  // 令 y 为伯父节点
            if (y && y->color == __rb_tree_red) {               // 伯父节点存在, 且为红
                x->parent->color = __rb_tree_black;             // 更改父节点为黑
                y->color = __rb_tree_black;                     // 更改伯父节点为黑
                x->parent->parent->color = __rb_tree_red;       // 更改祖父节点为红
                x = x->parent->parent;
            } else {    // 无伯父节点, 或伯父节点为黑
                if (x == x->parent->right) {            // 如果新节点为父节点的右子节点
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);     // 第一参数为左旋点
                }
                x->parent->color = __rb_tree_black;     // 改变颜色
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(x->parent->parent, root); // 第一参数为右旋点
            }
        } else {    // // 父节点为祖父节点的右子节点
            __rb_tree_node_base* y = x->parent->parent->left;   // 令 y 为伯父节点
            if (y && y->color == __rb_tree_red) {           // 有伯父节点, 且为红
                x->parent->color = __rb_tree_black;         // 更改父节点为黑
                y->color = __rb_tree_black;                 // 更改伯父节点为黑
                x->parent->parent->color = __rb_tree_red;   // 更改祖父节点为红
                x = x->parent->parent;      // 准备继续往上层检查
            } else {        // 无伯父节点, 或伯父节点为黑
                if (x == x->parent->left) {         // 如果新节点为父节点的左子节点
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);    // 第一参数为右旋点
                }
                x->parent->color = __rb_tree_black;     // 改变颜色
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);     // 第一参数为左旋点
            }
        }
    } // while end
    root->color = __rb_tree_black;      // 根节点永远为黑
}

// 寻找 RB 树中是否有键值为 k 的节点
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k)
{
    link_type y = header;       // Last node which is not less than k.
    link_type x = root();       // Current node.

    while (x != 0) {
        // 以下, key_compare 是节点键值大小比较准则. 应该会是个 function object
        if (!key_compare(key(x), k)) {
            // x 键值大于 k, 遇到大值向左走
            y = x, x = left(x);
        } else {
            // x 键值小于 k, 遇到小值向右走
            x = right(x);
        }
    }
    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}
#include <iostream>
#include <algorithm>

#include "../src/stl_function.h"
#include "../src/stl_tree.h"

int main(void)
{
    rb_tree<int, int, identity<int>, std::less<int> > itree;
    std::cout << itree.size() << std::endl;     // 0

    itree.insert_unique(10);    // __rb_tree_rebalance
    itree.insert_unique(7);     // __rb_tree_rebalance
    itree.insert_unique(8);     // __rb_tree_rebalance
                                    // __rb_tree_rotate_left
                                    // __rb_tree_rotate_right
    itree.insert_unique(15);    // __rb_tree_rebalance
    itree.insert_unique(5);     // __rb_tree_rebalance
    itree.insert_unique(6);     // __rb_tree_rebalance
                                    // __rb_tree_rotate_left
                                    // __rb_tree_rotate_right
    itree.insert_unique(11);    // __rb_tree_rebalance
                                    // __rb_tree_rotate_left
                                    // __rb_tree_rotate_right
    itree.insert_unique(13);    // __rb_tree_rebalance
    itree.insert_unique(12);    // __rb_tree_rebalance
                                    // __rb_tree_rotate_right
    std::cout << itree.size() << std::endl;     // 9
    rb_tree<int, int, identity<int>, std::less<int> >::iterator ite1 = itree.begin();
    rb_tree<int, int, identity<int>, std::less<int> >::iterator ite2 = itree.end();
    __rb_tree_base_iterator rbtite;

    for (; ite1 != ite2; ++ite1) {
        std::cout << *ite1 << ' ';      // 5 6 7 8 10 11 12 13 15
    }
    std::cout << std::endl;

    for (; ite1 != ite2; ++ite1) {
        rbtite = __rb_tree_base_iterator(ite1);
        std::cout << *ite1 << '(' << rbtite.node->color << ") ";
    }
    std::cout << std::endl;
    // 5(0) 6(1) 7(0) 8(1) 10(1) 11(0) 12(0) 13(1) 15(0)

    return 0;
}
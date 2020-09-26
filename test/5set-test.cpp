#include <iostream>
#include <algorithm>

#include "../src/stl_set.h"

int main(void)
{
    int i;
    int ia[5] = {0, 1, 2, 3, 4};
    set<int> iset(ia, ia + 5);

    std::cout << "size=" << iset.size() << std::endl;       // size=5
    std::cout << "3 count=" << iset.count(3) << std::endl;  // 3 count=1
    iset.insert(3);
    std::cout << "size=" << iset.size() << std::endl;       // size=5
    std::cout << "3 count=" << iset.count(3) << std::endl;  // 3 count=1
    iset.insert(5);
    std::cout << "size=" << iset.size() << std::endl;       // size=6
    std::cout << "3 count=" << iset.count(3) << std::endl;  // 3 count=1
    iset.erase(1);
    std::cout << "size=" << iset.size() << std::endl;       // size=5
    std::cout << "3 count=" << iset.count(3) << std::endl;  // 3 count=1
    std::cout << "1 count=" << iset.count(3) << std::endl;  // 1 count=0

    set<int>::iterator ite1 = iset.begin();
    set<int>::iterator ite2 = iset.end();
    for (; ite1 != ite2; ++ite1)
        std::cout << *ite1;
    std::cout << std::endl;                                 // 0 2 3 4 5

    // 使用 STL 算法 find() 来搜寻元素, 可以有效运作, 但不是好办法
    ite1 = std::find(iset.begin(), iset.end(), 3);
    if (ite1 != iset.end())
        std::cout << "3 found" << std::endl;                // 3 found

    ite1 = std::find(iset.begin(), iset.end(), 1);
    if (ite1 == iset.end())
        std::cout << "1 not found" << std::endl;            // 1 not found
    
    // 面对关联式容器, 应该使用其所提供的 find 函数来搜寻元素, 会比使用 STL 算法 find() 更有效率.
    // 因为 STL 算法 find() 只是循序搜寻
    ite1 = iset.find(3);
    if (ite1 != iset.end())
        std::cout << "3 found" << std::endl;                // 3 found

    ite1 = iset.find(1);
    if (ite1 != iset.end())
        std::cout << "1 not found" << std::endl;            // 1 not found

    // 企图通过迭代器来改变 set 元素, 是不被允许的
    // *ite1 = 9;       // error, assignment of read-only location

    return 0;
}
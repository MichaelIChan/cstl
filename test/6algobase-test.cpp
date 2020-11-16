#include <iostream>
#include <vector>
#include <string>

#include "../src/stl_function.h"
#include "../src/stl_iterator.h"
#include "../src/stl_algobase.h"

template <class T>
struct display {
    void operator() (const T& x) const
    {
        std::cout << x << ' ';
    }
};

int main(void)
{
    int ia[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> iv1(ia, ia + 5);
    std::vector<int> iv2(ia, ia + 9);

    // {0, 1, 2, 3, 4} v.s {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    std::cout << *(cstl::mismatch(iv1.begin(), iv1.end(), iv2.begin()).first);    // ?
    std::cout << *(cstl::mismatch(iv1.begin(), iv1.end(), iv2.begin()).second);   // 5
    // 以上判断两个区间的第一个不匹配点. 返回一个由两个迭代器组成的 pair,
    // 其中第一个迭代器指向第一区间的不匹配点, 第二个迭代器指向第二区间的不匹配点
    // 上述写法很危险, 应该先判断迭代器是否不等于容器的 end(), 然后才可以做输出操作

    // 如果两个序列在 [first, last) 区间内相等, equal() 返回 true
    // 如果第序列的元素比较多, 多出来的元素不予考虑
    std::cout << cstl::equal(iv1.begin(), iv1.end(), iv2.begin());    // 1, true
    std::cout << equal(iv1.begin(), iv1.end(), &ia[3]);         // 0, false
    // {0, 1, 2, 3, 4} 不等于 {3, 4, 5, 6, 7}

    std::cout << cstl::equal(iv1.begin(), iv1.end(), &ia[3], less<int>());         // 1
    // {0, 1, 2, 3, 4} 小于 {3, 4, 5, 6, 7}

    cstl::fill(iv1.begin(), iv1.end(), 9);    // 区间内全部填 9
    cstl::for_each(iv1.begin(), iv1.end(), display<int>());   // 9 9 9 9 9

    cstl::fill_n(iv1.begin(), 3, 7);          // 从迭代器所指为止开始, 填 3 个 7
    cstl::for_each(iv1.begin(), iv1.end(), display<int>());   // 7 7 7 9 9

    std::vector<int>::iterator ite1 = iv1.begin();
    std::vector<int>::iterator ite2 = ite1;
    advance(ite2, 3);

    cstl::iter_swap(ite1, ite2);        // 将两个迭代器所指元素对调
    std::cout << *ite1 << ' ' << *ite2 << std::endl;        // 9 7
    cstl::for_each(iv1.begin(), iv1.end(), display<int>()); // 9 7 7 7 9

    // 以下取两值之大者
    std::cout << cstl::max(*ite1, *ite2) << std::endl;  // 9
    // 以下取两值之小者
    std::cout << cstl::min(*ite1, *ite2) << std::endl;  // 7

    // 此刻状态, iv1: {9 7 7 7 9}, iv2: {0 1 2 3 4 5 6 7 8}
    cstl::swap(*iv1.begin(), *iv2.begin());
    cstl::for_each(iv1.begin(), iv1.end(), display<int>());   // 0 7 7 9 9
    cstl::for_each(iv2.begin(), iv2.end(), display<int>());   // 9 1 2 3 4 5 6 7 8

    std::string stra1[] = {"Jamie", "JJHou", "Jason"};
    std::string stra2[] = {"Jamie", "JJhou", "Jerry"};

    std::cout << cstl::lexicographical_compare(stra1, stra1 + 2, stra2, stra2 + 2);
    // 1 (stra1 小于 star2)

    std::cout << cstl::lexicographical_compare(stra1, stra1 + 2, stra2, stra2 + 2,
                                               greater<std::string>());
    // 0 (stra1 不大于 star2)

    return 0;
}
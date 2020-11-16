#include <iostream>
#include <vector>

#include "../src/stl_numeric.h"
#include "../src/stl_function.h"
#include "../src/stl_iterator.h"

int main(void)
{
    int ia[5] = {1, 2, 3, 4, 5};
    std::vector<int> iv(ia, ia + 5);

    std::cout << accumulate(iv.begin(), iv.end(), 0) << std::endl;
    // 15, i.e. 0 + 1 + 2 + 3 + 4 + 5

    std::cout << accumulate(iv.begin(), iv.end(), 0, minus<int>()) << std::endl;
    // -15, i.e. 0 - 1 - 2 - 3 - 4 - 5

    std::cout << inner_product(iv.begin(), iv.end(), iv.begin(), 10) << std::endl;
    // 65, i.e. 10 + 1*1 + 2*2 + 3*3 + 4*4 + 5*5

    std::cout << inner_product(iv.begin(), iv.end(), iv.begin(), 10,
                               minus<int>(), plus<int>()) << std::endl;
    // -20, i.e. 10 - 1+1 - 2+2 - 3+3 - 4+4 - 5+5

    // 以下这个迭代器将绑定到 cout, 作为输出用
    ostream_iterator<int> oite(std::cout, " ");

    partial_sum(iv.begin(), iv.end(), oite);
    // 1 3 6 10 15 (第 n 个新元素是前 n 个旧元素的相加总计)

    partial_sum(iv.begin(), iv.end(), oite, minus<int>());
    // 1 -1 -4 -8 -13 (第 n 个新元素是前 n 个旧元素的运算总计)

    adjacent_difference(iv.begin(), iv.end(), iote);
    // 1 1 1 1 1 (#1 新元素照录, #n 新元素等于 #n 旧元素 - #n-1 旧元素)

    adjacent_difference(iv.begin(), iv.end(), iote, plus<int>());
    // 1 3 5 7 9 (#1 新元素照录, #n 新元素等于 op(#n 旧元素 - #n-1 旧元素))

    std::cout << power(10, 3) << std::endl;                 // 1000, i.e. 10 * 10 * 10
    std::cout << power(10, 3, plus<int>()) << std::endl;    // 30, i.e. 10 + 10 + 10

    int n = 3;
    iota(iv.begin(), iv.end(), n);      // 在指定区间内填入n, n + 1, n + 2 ...
    for (int i = 0; i < iv.size(); ++i) {
        std::cout << iv[i] << ' ';      // 3 4 5 6 7
    }

    return 0;
}
#include <iostream>
#include <string>

#include "../src/stl_map.h"

int main(void)
{
    map<std::string, int> simap;        // 以 string 为键值, 以 int 为实值
    simap[std::string("jjhou")] = 1;    // 第 1 对内容是 ("jjhou", 1)
    simap[std::string("jerry")] = 2;    // 第 2 对内容是 ("jerry", 2)
    simap[std::string("jason")] = 3;    // 第 3 对内容是 ("jason", 3)
    simap[std::string("jimmy")] = 4;    // 第 4 对内容是 ("jimmy", 4)

    pair<std::string, int> value(std::string("david"), 5);
    simap.insert(value);

    map<std::string, int>::iterator simap_iter = simap.begin();
    for (; simap_iter != simap.end(); ++simap_iter) {
        std::cout << simap_iter->first << ' '
                  << simap_iter->second << std::endl;   // david 5
                                                        // jason 3
                                                        // jerry 2
                                                        // jimmy 4
                                                        // jjhou 1
    }
    int number = simap[std::string("jjhou")];
    std::cout << number << std::endl;                   // 1

    map<std::string, int>::iterator ite1;

    // 面对关联式容器, 应该使用其所提供的 find 函数来搜寻元素, 会比使用 STL 算法更有效率.
    // 因为 STL 算法 find() 只是循序搜寻
    ite1 = simap.find(std::string("mchen"));
    if (ite1 == simap.end()) {
        std::cout << "mchen not found" << std::endl;    // mchen not found
    }

    ite1 = simap.find(std::string("jerry"));
    if (ite1 != simap.end()) {
        std::cout << "jerry found" << std::endl;        // jerry found
    }

    ite1->second = 9;   // 可以通过 map 迭代器修改 "value" (not key)
    int number2 = simap[std::string("jerry")];
    std::cout << number2 << std::endl;                  // 9

    return 0;
}
#include <iostream>
#include <algorithm>

#include "../src/stl_slist.h"

int main(void)
{
    int i;
    slist<int> islist;
    std::cout << "size=" << islist.size() << std::endl;     // size=0
    
    islist.push_front(9);
    islist.push_front(1);
    islist.push_front(2);
    islist.push_front(3);
    islist.push_front(4);
    std::cout << "size=" << islist.size() << std::endl;     // size=5

    slist<int>::iterator ite = islist.begin();
    slist<int>::iterator ite2 = islist.end();
    for ( ; ite != ite2; ++ite)
        std::cout << *ite << ' ';                           // 4 3 2 1 9
    std::cout << std::endl;

    ite = std::find(islist.begin(), islist.end(), 1);
    if (ite != 0)
        islist.insert(ite, 99);

    std::cout << "size=" << islist.size() << std::endl;     // size=6
    std::cout << *ite << std::endl;                         // 1

    ite = islist.begin();
    ite2 = islist.end();
    for ( ; ite != ite2; ++ite)
        std::cout << *ite << ' ';                           // 4 3 2 99 1 9
    std::cout << std::endl;

    ite = std::find(islist.begin(), islist.end(), 3);
    if (ite != 0)
        std::cout << *(islist.erase(ite)) << std::endl;     // 2
    
    ite = islist.begin();
    ite2 = islist.end();
    for ( ; ite != ite2; ++ite)
        std::cout << *ite << ' ';                           // 4 2 99 1 9
    std::cout << std::endl;

    return 0;
}
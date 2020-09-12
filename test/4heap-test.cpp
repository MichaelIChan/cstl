#include <iostream>

#include "../src/stl_vector.h"
#include "../src/stl_heap.h"

int main()
{
    {
        // test heap (底层以 vector 完成)
        int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
        vector<int> ivec(ia, ia + 9);

        make_heap(ivec.begin(), ivec.end());
        for (int i = 0; i < ivec.size(); ++i) {
            std::cout << ivec[i] << ' ';        // 9 5 8 3 4 0 2 3 1
        }
        std::cout << std::endl;

        ivec.push_back(7);
        push_heap(ivec.begin(), ivec.end());
        for (int i = 0; i < ivec.size(); ++i) {
            std::cout << ivec[i] << ' ';        // 9 7 8 3 5 0 2 3 1 4
        }
        std::cout << std::endl;

        pop_heap(ivec.begin(), ivec.end());
        std::cout << ivec.back() << std::endl;  // 9. return but no remove
        ivec.pop_back();                        // remove last elem and no return

        for (int i = 0; i < ivec.size(); ++i) {
            std::cout << ivec[i] << ' ';        // 8 7 4 3 5 0 2 3 1
        }
        std::cout << std::endl;

        sort_heap(ivec.begin(), ivec.end());
        for (int i = 0; i < ivec.size(); ++i) {
            std::cout << ivec[i] << ' ';        // 0 1 2 3 4 5 6 7 8
        }
        std::cout << std::endl;
    }
}
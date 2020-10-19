// file: 5hastset-test.cpp
#include <iostream>
#include <cstring>

#include "../src/stl_hash_set.h"

struct eqstr {
    bool operator() (const char* s1, const char* s2) const
    {
        return std::strcmp(s1, s2) == 0;
    }
};

void lookup(const hash_set<const char*, hash<const char*>, eqstr>& Set, const char* word)
{
    hash_set<const char*, hash<const char*>, eqstr>::const_iterator it = Set.find(word);
    std::cout << " " << word << ": "
              << (it != Set.end() ? "present" : "not present")
              << std::endl;
}

int main(void)
{
    hash_set<const char*, hash<const char*>, eqstr> Set;
    Set.insert("kiwi");
    Set.insert("plum");
    Set.insert("apple");
    Set.insert("mango");
    Set.insert("apricot");
    Set.insert("banana");

    lookup(Set, "mango");       // mango: present
    lookup(Set, "apple");       // apple: present
    lookup(Set, "durian");      // durian: not present

    hash_set<const char*, hash<const char*>, eqstr>::iterator ite1 = Set.begin();
    hash_set<const char*, hash<const char*>, eqstr>::iterator ite2 = Set.end();
    for (; ite1 != ite2; ++ite1) {
        std::cout << *ite1 << ' ';      // banana plum mango apple kiwi apricot
    }

    return 0;
}
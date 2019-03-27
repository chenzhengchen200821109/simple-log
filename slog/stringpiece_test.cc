#include "stringpiece.h"
#include <string>
#include <iostream>
#include <stdio.h>

int main()
{
    std::string str0("Hello World");
    internal::StringPiece str1(str0);
    internal::StringPiece str2("hello");

    str0 = str0;
    str0[0] = 'h';

    std::cout << str0 << std::endl;
    std::cout << str1 << std::endl;
    std::cout << str1.find_first_of(str2) << std::endl;

    return 0;
}

#include<iostream>
#include"temp.h"

int main(int, char**)
{
    std::vector<int> vec(10,1);
    foo(vec);
    foo<int>(vec);
    foo<std::vector<int>,int>(vec);
}
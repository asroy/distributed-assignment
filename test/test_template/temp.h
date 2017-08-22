#include<iostream>
#include<vector>

template<typename TDataType>
void foo(TDataType r_data)
{
    std::cout<<"1 argument, scalar"<<std::endl;
}

template<typename TDataType, typename TDummyType>
void foo(TDataType r_data)
{
    std::cout<<"2 argument, scalar"<<std::endl;
}

template<typename TDataType>
void foo(std::vector<TDataType> r_data)
{
    std::cout<<"1 argument, vector"<<std::endl;
}

// template<typename TDataType, typename TDummyType>
// void foo(std::vector<TDataType> r_data)
// {
//     std::cout<<"2 argument, vector"<<std::endl;
// }

// template<typename TDataType, typename TReturnType>
// TReturnType foo(TDataType r_data)
// {
//     std::cout<<"2 argument"<<std::endl;
//     return 0;
// }


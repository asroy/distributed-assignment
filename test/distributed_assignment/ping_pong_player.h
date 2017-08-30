#pragma once
#include<string>
#include<set>
#include<math.h>
#include<mpi.h>
#include<MpiLocation.h>

template<typename TContractorKeyType>
class PingPongPlayer
{
public:
    using ContractorKeySet = std::set<TContractorKeyType, typename TContractorKeyType::LessThanComparator>;

    PingPongPlayer()
    :   mName(),
        mKey{TContractorKeyType::NoKey()}
    {}

    ~PingPongPlayer()
    {}

    void SetName( const std::string & r_name )
    { mName = r_name; }

    std::string GetName() const
    { return mName; }

    void SetKey(const TContractorKeyType contrator_key)
    { mKey = contrator_key; }

    TContractorKeyType GetKey() const
    { return mKey; }

    void Execute( const int & r_in, int & r_out)
    {
        r_out = r_in;

        DataUtility::DataPrinter printer;

        std::cout<<"ball is at player ";
        printer.Print(*this);
        std::cout<<std::endl;

        for(int i = 0; i < 1000; i++)
        {
            double x = 10;
            x = sqrt(x*10);
        }
    }

private:
    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{PingPongPlayer: ";
        std::cout << "{Name: "<< mName <<"},",
        std::cout << "{ContractorKey: ";
        r_printer.Print(mKey);
        std::cout << "},";
        std::cout << "},";
    }

    std::string mName;
    TContractorKeyType mKey;

    friend class DataUtility::DataPrinter;
};
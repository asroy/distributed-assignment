#pragma once

namespace DistributedAssignment
{

template<typename TCommunicatorTYpe>
class DistributedKeyIssuer;

template<typename TCommunicatorTYpe>
class DistributedKey
{
public:
    typedef typename TCommunicatorTYpe::Location Location;

    DistributedKey() = delete;

    ~DistributedKey()
    {}

    struct LessThanComparator
    {
        bool operator() ( const DistributedKey & a, const DistributedKey & b ) const
        {
            typename Location::LessThanComparator location_less_than;

            if( location_less_than(a.mBirthPlace, b.mBirthPlace) )
                return true;
            else if( location_less_than(b.mBirthPlace, a.mBirthPlace) )
                return false;
            else if( a.mLocalKey < b.mLocalKey )
                return true;
            else
                return false;
        }
    };

private:
    DistributedKey( const Location birth_place, const std::size_t local_key )
    :   mBirthPlace{birth_place},
        mLocalKey{local_key}
    {}

    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{DistributedKey: ";
        r_printer.Print(mBirthPlace);
        r_printer.Print(mLocalKey);
        std::cout << "}";
    }

    Location mBirthPlace;
    std::size_t mLocalKey;

    friend class DistributedKeyIssuer<TCommunicatorTYpe>;
    friend class DataUtility::DataPrinter;
};

}
#pragma once

namespace DistributedAssignment
{

template<typename TLocationType>
class DistributedKey
{
public:
    DistributedKey() = delete;

    ~DistributedKey()
    {}

    struct LessThan
    {
        bool operator() ( const DistributedKey & a, const DistributedKey & b ) const
        {
            if( a.mBirthPlace.LessThan(b.mBirthPlace) )
                return true;
            else if( b.mBirthPlace.LessThan(a.mBirthPlace) )
                return false;
            else if( a.mLocalId < b.mLocalId )
                return true;
            else
                return false;
        }
    }

private:
    TLocationType mBirthPlace;
    std::size_t mLocalId;

    friend class DistributedKeyIssuer;
};

}
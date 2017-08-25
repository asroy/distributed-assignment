#pragma once
#include<vector>

namespace DataUtilities
{

//profile basic type
#define DATAPROFILE_PROFILE_BASIC_TYPE(TDATATYPE)                           \
DataProfile Profile( const TDATATYPE & r_data ) const                       \
{                                                                           \
    return DataProfile::Default().MakeNonTrivial();                         \
}

class DataProfile
{
public:
    DataProfile() = delete;

    ~DataProfile()
    {}

    static DataProfile Default()
    {
        DataProfile profile = {true, true, false, 0};
        return profile;
    }

    DataProfile MakeTrivial()
    {
        mIsTrivial = true;
        return *this;
    }

    DataProfile MakeNonTrivial()
    {
        mIsTrivial = false;
        return *this;
    }

    DataProfile MakeFromSender()
    {
        mIsFromSender = true;
        return *this;
    }

    DataProfile MakeNotFromSender()
    {
        mIsFromSender = false;
        return *this;
    }

    DataProfile SetBufferContentSize(const std::size_t buffer_size)
    {
        mBufferContentSize = buffer_size;
        return *this;
    }

    std::size_t GetBufferContentSize()
    {
        if( mBufferContentSize < sizeof(Default()) )
        {
            std::cout<<__func__<<": mBufferContentSize too small! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }
        return mBufferContentSize;
     }

    bool IsFromSender()
    { return mIsFromSender; }

    bool IsTrivial()
    { return mIsTrivial; }

    //profile basic data type
    DATAPROFILE_PROFILE_BASIC_TYPE(bool)
    DATAPROFILE_PROFILE_BASIC_TYPE(char)
    DATAPROFILE_PROFILE_BASIC_TYPE(int)
    DATAPROFILE_PROFILE_BASIC_TYPE(long)
    DATAPROFILE_PROFILE_BASIC_TYPE(unsigned int)
    DATAPROFILE_PROFILE_BASIC_TYPE(unsigned long)
    DATAPROFILE_PROFILE_BASIC_TYPE(float)
    DATAPROFILE_PROFILE_BASIC_TYPE(double)

    //profile std::vector
    template<typename TDataType>
    DataProfile Profile ( const std::vector<TDataType> & r_vector ) const
    {
        DataProfile profile = DataProfile::Default();

        if( r_vector.size() == 0 )
            profile.MakeTrivial();
        else
            profile.MakeNonTrivial();

        return profile;
    }

    //profile user defined data type
    template<typename TDataType>
    DataProfile Profile ( const TDataType & r_data ) const
    {
        return r_data.Profile(*this);
    }

private:
    bool mIsTrivial;
    bool mIsFromSender;
    std::size_t mBufferContentSize;
};

}
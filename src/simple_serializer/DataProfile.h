#include<vector>

//profile basic type
#define DATAPROFILE_PROFILE(TDATATYPE)                                      \
DataProfile Profile( const TDATATYPE & r_data ) const                       \
{                                                                           \
    std::cout<<"Profile basic type"<<std::endl;                             \
    return DataProfile::Default().MakeNonTrivial();                         \
}


class DataProfile
{
  public:
    DataProfile()
      : mIsTrivial{true},
        mIsFromSender{false}
    {}

    ~DataProfile()
    {}

    static DataProfile Default()
    {
      DataProfile profile;
      profile.mIsTrivial = false;
      profile.mIsFromSender = false;
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

    bool IsFromSender()
    { return mIsFromSender; }

    bool IsTrivial()
    { return mIsTrivial; }

    //profile basic data type
    DATAPROFILE_PROFILE(bool)
    DATAPROFILE_PROFILE(char)
    DATAPROFILE_PROFILE(int)
    DATAPROFILE_PROFILE(long)
    DATAPROFILE_PROFILE(unsigned int)
    DATAPROFILE_PROFILE(unsigned long)
    DATAPROFILE_PROFILE(float)
    DATAPROFILE_PROFILE(double)

    //profile std::vector
    template<typename TDataType>
    DataProfile Profile ( const std::vector<TDataType> & r_vector ) const
    {
      std::cout<<"Profile vector"<<std::endl;
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
      std::cout<<"Profile user defined data"<<std::endl;
      return r_data.Profile(*this);
    }

  private:
    bool mIsTrivial;
    bool mIsFromSender;
};


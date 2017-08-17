class DataProfiler
{
  public:
    DataProfiler()
      : mIsTrivial{true},
        mCountInt{0},
        mCountDouble{0},
        mCountChar{0},
    {}

    ~DataProfiler()
    {}

    static DataProfiler DefaultProfile()
    {
      DataProfiler profile();
      profile.mIsTrivial = false;
      profile.mCountInt = 0;
      profile.mCountDouble = 0;
      profile.mCountChar = 0;
      return profile;
    }

    DataProfiler & MakeTrivial()
    {
      mIsTrivial = true;
      return *this;
    }

    DataProfiler & MakeNonTrivial()
    {
      mIsTrivial = false;
      return *this;
    }

    //profle basic data type
    template<typename TDataType, typename TDummyType>
    DataProfiler Profile( const TDataType & r_data ) const
    { return DataProfiler::DefaultProfile().MakeNonTrivial(); };

    template<> DataProfiler Profile( const int & r_data ) const
    {
      DataProfiler profile = DataProfiler::DefaultProfile();
      profile.mIsTrivial = false;
      profile.mCountInt = 1;
      return profile;
    }

    template<> DataProfiler Profile( const double & r_data ) const
    {
      DataProfiler profile = DataProfiler::DefaultProfile();
      profile.mIsTrivial = false;
      profile.mCountDouble = 1;
      return profile;
    }

    template<> DataProfiler Profile( const char & r_data ) const
    {
      DataProfiler profile = DataProfiler::DefaultProfile();
      profile.mIsTrivial = false;
      profile.mCountChar = 1;
      return profile;
    }

    //profile std::vector
    template<typename TDataType>
    DataProfiler Profile( const std::vector<TDataType> & r_vector ) const
    {
      DataProfiler profile = DataProfiler::DefaultProfile();

      for( const TDatatype data : r_vector )
      {
        DataProfiler sub_profile = Profile(data);
        profile.mCountInt    += sub_profile.mCountInt;
        profile.mCountDouble += sub_profile.mCountDouble;
        profile.mCountChar   += sub_profile.mCountChar;
      }

      if( r_vector.size() == 0 )
        profile.MakeTrivial();
      else
        profile.MakeNonTrivial();

      return profile;
    }

    //profile user defined data type
    template<typename TDataType>
    DataProfiler Profile( const TDataType & r_data ) const
    { return r_data.Profile(*this); }

  private:
    bool mIsTrivial;
    int mCountInt;
    int mCountDouble;
    int mCountChar;
};

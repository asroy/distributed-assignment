class DataProfile
{
  public:
    DataProfile()
      : mIsTrivial{true},
        mIsFromSender{false},
        mCountInt{0},
        mCountDouble{0},
        mCountChar{0},
    {}

    ~DataProfile()
    {}

    static DataProfile Default()
    {
      DataProfile profile();
      profile.mIsTrivial = false;
      profile.mCountInt = 0;
      profile.mCountDouble = 0;
      profile.mCountChar = 0;
      return profile;
    }

    DataProfile & MakeTrivial()
    {
      mIsTrivial = true;
      return *this;
    }

    DataProfile & MakeNonTrivial()
    {
      mIsTrivial = false;
      return *this;
    }

    DataProfile & MakeFromSender()
    {
      mIsFromSender = true;
      return *this;
    }

    DataProfile & MakeNotFromSender()
    {
      mIsFromSender = false;
      return *this;
    }

    bool IsFromSender()
    { return mIsFromSender; }

    bool IsTrivial()
    { return mIsTrivial; }

    //profle basic data type
    template<typename TDataType, typename TDummyType>
    DataProfile Profile( const TDataType & r_data ) const
    { return DataProfile::Default().MakeNonTrivial(); };

    template<> DataProfile Profile( const int & r_data ) const
    {
      DataProfile profile = DataProfile::Default();
      profile.mIsTrivial = false;
      profile.mCountInt = 1;
      return profile;
    }

    template<> DataProfile Profile( const double & r_data ) const
    {
      DataProfile profile = DataProfile::Default();
      profile.mIsTrivial = false;
      profile.mCountDouble = 1;
      return profile;
    }

    template<> DataProfile Profile( const char & r_data ) const
    {
      DataProfile profile = DataProfile::Default();
      profile.mIsTrivial = false;
      profile.mCountChar = 1;
      return profile;
    }

    //profile std::vector
    template<typename TDataType>
    DataProfile Profile( const std::vector<TDataType> & r_vector ) const
    {
      DataProfile profile = DataProfile::Default();

      for( const TDatatype data : r_vector )
      {
        DataProfile sub_profile = Profile(data);
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
    DataProfile Profile( const TDataType & r_data ) const
    { return r_data.Profile(*this); }

  private:
    bool mIsTrivial;
    bool mIsFromSender;
    int mCountInt;
    int mCountDouble;
    int mCountChar;
};

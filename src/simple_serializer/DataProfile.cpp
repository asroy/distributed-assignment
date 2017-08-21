#include<iostream>
#include"DataProfile.h"

//profile basic data type
template<typename TDataType,typename TDummyType>
DataProfile DataProfile::Profile( const TDataType & r_data ) const
{ 
  std::cout<<"Profile default"<<std::endl;
  return DataProfile::Default().MakeNonTrivial();
}

template<> 
DataProfile DataProfile::Profile<int,int>( const int & r_data ) const
{
  std::cout<<"Profile int"<<std::endl;
  DataProfile profile = DataProfile::Default();
  profile.mIsTrivial = false;
  profile.mCountInt = 1;
  return profile;
}

template<> 
DataProfile DataProfile::Profile<double,int>( const double & r_data ) const
{
  std::cout<<"Profile double"<<std::endl;
  DataProfile profile = DataProfile::Default();
  profile.mIsTrivial = false;
  profile.mCountDouble = 1;
  return profile;
}

template<> 
DataProfile DataProfile::Profile<char,int>( const char & r_data ) const
{
  std::cout<<"Profile char"<<std::endl;
  DataProfile profile = DataProfile::Default();
  profile.mIsTrivial = false;
  profile.mCountChar = 1;
  return profile;
}

//profile std::vector
template<typename TDataType>
DataProfile DataProfile::Profile ( const std::vector<TDataType> & r_vector ) const
{
  std::cout<<"Profile vector"<<std::endl;
  DataProfile profile = DataProfile::Default();

  for( const TDataType data : r_vector )
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
DataProfile DataProfile::Profile ( const TDataType & r_data ) const
{ return r_data.Profile(*this); }

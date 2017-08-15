template < typename TContractorKeyType,
           typename TLocationType >
class Contractor
{
  public:
    Contractor( const TContractorKeyType & key, const TLocationType location )
      : mKey{key}, mLocation{location}
    { std::cout<<"calling Contractor customed constructor"<<std::endl; }

    TContractorKeyType & Key() { return mKey; }
    TLocationType & Location() { return mLocation; }
  private:
    TContractorKeyType mKey;
    TLocationType mLocation;
}

template<typename TContractorKeyType>
class Contractor
{
public:
    Contractor( const TContractorKeyType & key )
      : mKey{key}
    {}

    TContractorKeyType & Key() 
    { return mKey; }
    
private:
    TContractorKeyType mKey;
}

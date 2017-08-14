template < typename TContractorKeyType,
           typename TResidencyType >
class Contractor
{
  public:
    Contractor( const TContractorKeyType & key, const TContractorResidencyType & residency )
      : mKey{key}, mResidency{residency}
    { std::cout<<"calling Contractor customed constructor"<<std::endl; }

    ContractorKeyType & Key() { return mKey; }
    ContractorResidency & Residency() { return mResidency; }
  private:
    ContractorKeyType mKey;
    ResidencyType mResidency;
}

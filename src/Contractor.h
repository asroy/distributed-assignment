template < typename TContractorKeyType,
           typename TContractorResidencyType >
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
    ContractorResidencyType mResidency;
}

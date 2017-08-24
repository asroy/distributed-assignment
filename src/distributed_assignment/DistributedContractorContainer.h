#include<iostream>

template< TContractorType,
          TContractorKeyType,
          TLocationType >
class DistributedContractorContainer
{
  public:
    typedef TContractorType* ContractorPointerType;
    typedef std::map<TContractorKeyType, ContractorPointerType> ContractorPointerContainerType;
    typedef std::map<TContractorKeyType, TLocationType> LocationContainerType;

    void AddLocalContractor( const TContractorType & contractor )
    { mLocalContractors[contractor.Key()] = & contractor; }

    void GenerateGlobalContractorsLocation()
    {

    }

    TLocationContainerType & GlobalContractorsLocation()
    { return mGlobalContractorsLocation; }

    ContractorPointerType LocalContractor( TContractorKeyType & key )
    { 
      typename ContractorPointerContainerType::iterator it = mLocalContractor.find(key);
      if( it == mLocalContractor.end() )
        return nullptr;
      else
        return it->second;
    }

    TLocationType & ContactorLocation( TContractorKeyType & )
    {
      LocationContainerType::iterator it = mGlobalContractorsLocation.find(key);
      if( it == mGlobalContractorsLocation.end() )
      {
        std::cout << __func__ << std::endl;
        exit(EXIT_FAILURE);
      }
      else
        return it->second;
    }

    template< typename TInputDataType,
              typename TOutputDataType >
    void Execute( DistributedAssignmentDataContainer<TContractorKeyType> & keys, 
                  DistributedAssignmentDataContainer<TInputDataType> & inputs,
                  DistributedAssignmentDataContainer<TOutputDataType> & outputs)
    {
      keys.SendAssignorDataToAssigneeData();
      inputs.SendAssignorDatatoAssingeeData();

      typedef typename std::vector<TContractorKeyType>::iterator KeyIteratorType;
      typedef typename std::vector<TInputDataType>    ::iterator InputIteratorType;
      typedef typename std::vector<TOutputDataType>   ::iterator OutputIteratorType;

      for( KeyIteratorType    it_key    = keys   .AssigneeData().begin(),
           InputIteratorType  it_input  = inputs .AssigneeData().begin(),
           OutputIteratorType it_output = outputs.AssigneeData().begin();
           it_key    != keys   .AssigneeData().end(),
           it_input  != inputs .AssigneeData().end(),
           it_output != outputs.AssigneeData().end();
           it_key    = std::next(it_key),
           it_input  = std::next(it_input),
           it_output = std::next(it_output) )
      {
        TContractorType & assignee = LocalContractor(*it_key);
        assignee(*it_input, *it_output); 
      }

      outputs.SendAssigneeDataToAssignorData();
    }

  private:
    ContractorPointerContainerType mLocalContractors;
    LocationContainerType mGlobalContractorsLocation;
};

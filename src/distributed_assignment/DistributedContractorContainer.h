#include<iostream>

template<typename TContractorType,
         typename TContractorKeyType,
         typename TCommunicatorType>
class DistributedContractorContainer
{
public:
    typedef typename TCommunicatorType::Location Location;

    typedef TContractorType* ContractorPointer;
    typedef std::map<TContractorKeyType, ContractorPointer> ContractorPointerContainer;
    typedef std::map<TContractorKeyType, Location> LocationContainer;

    void AddLocalContractor( const TContractorType & contractor )
    { mLocalContractors[contractor.Key()] = & contractor; }

    void GenerateGlobalContractorsLocation()
    {

    }

    LocationContainer & GlobalContractorsLocation()
    { return mGlobalContractorsLocation; }

    ContractorPointer LocalContractor( TContractorKeyType & key )
    {
        typename ContractorPointerContainer::iterator it = mLocalContractor.find(key);
        if( it == mLocalContractor.end() )
            return nullptr;
        else
            return it->second;
    }

    Location ContactorLocation( TContractorKeyType key )
    {
        LocationContainer::iterator it = mGlobalContractorsLocation.find(key);
        if( it == mGlobalContractorsLocation.end() )
            return Location::Nowhere();
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
    ContractorPointerContainer mLocalContractors;
    LocationContainer mGlobalContractorsLocation;
};

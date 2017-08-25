#pragma once
#include<iostream>

namespace DistributedAssignment
{

template<typename TContractorType,
         typename TContractorKeyType,
         typename TCommunicatorType>
class DistributedContractorManager
{
public:
    typedef typename TCommunicatorType::Location Location;

    typedef TContractorType * ContractorPointer;
    typedef std::map<TContractorKeyType, ContractorPointer> ContractorPointerContainer;
    typedef std::map<TContractorKeyType, Location> LocationContainer;

    DistributedContractorManager()
    :   mLocalContractors(),
        mGlobalContractorsLocation(),
        mLocalContractorNewlyAdded{false}
    {}

    ~DistributedContractorManager()
    {}

    void Clear()
    {
        mLocalContractors.clear();
        mGlobalContractorsLocation.clear();
    }

    void AddLocalContractor( const TContractorType & r_contractor )
    {
        mLocalContractors[r_contractor.Key()] = & r_contractor;
        mLocalContractorNewlyAdded = true;
    }

    void GenerateGlobalContractorsLocation( int mpi_tag = 0 )
    {
        typedef std::vector<TContractorKeyType> ContractorKeyVector;
        typedef std::map<Location, ContractorKeyVector> ContractorKeyVectorMap;

        ContractorKeyVector local_contractor_key_vector;
        ContractorKeyVectorMap global_contractor_key_vector_map;

        //local contractors
        for( const std::pair<TContractorKeyType, ContractorPointer> & r_local_contractor_pointer_pair : mLocalContractors )
        {
            TContractorKeyType local_contractor_key = r_local_contractor_pointer_pair.first;
            local_contractor_key_vector.push_back(local_contractor_key);
        }

        //all gather
        mpCommunicator->AllGather( local_contractor_key_vector, global_contractor_key_vector_map, mpi_tag );

        //global contractors location
        mGlobalContractorsLocation.clear();

        for( const std::pair<Location, ContractorKeyVector> & r_global_contractor_key_vector_pair : global_contractor_key_vector_map )
        {
            Location loation = r_global_contractor_key_vector_pair.first;
            ContractorKeyVector r_global_contractor_key_vector = r_global_contractor_key_vector_pair.second;

            for ( const TContractorKeyType & r_global_contractor_key : r_global_contractor_key_vector )
                mGlobalContractorsLocation[r_global_contractor_key] = location;
        }
    }

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

private:
    TCommunicatorType * mpCommunicator;
    ContractorPointerContainer mLocalContractors;
    LocationContainer mGlobalContractorsLocation;
    bool mLocalContractorNewlyAdded;
};

}
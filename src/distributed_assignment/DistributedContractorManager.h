#pragma once
#include<iostream>

namespace DistributedAssignment
{

template<typename TContractorType,
         typename TContractorKeyIssuerType,
         typename TCommunicatorType>
class DistributedContractorManager
{
public:
    typedef typename TCommunicatorType::Location Location;
    typedef typename TContractorKeyIssuerType::Key ContractorKey;
    typedef TContractorType * ContractorPointer;

    typedef std::map<ContractorKey, ContractorPointer> ContractorPointerMap;
    typedef std::map<ContractorKey, Location> LocationMap;

    template<typename TDataType>
    using PairByContractorKeyType = std::pair<const ContractorKey, TDataType>;

    DistributedContractorManager() = delete;

    DistributedContractorManager( const TCommunicatorType & communicator )
    :   mpCommunicator{& communicator},
        mContractorKeyIssuer(communicator),
        mLocalContractorsPointer(),
        mGlobalContractorsLocation(),
        mLocalContractorNewlyAdded{false}
    {}

    ~DistributedContractorManager()
    {}

    void ClearRegistratedContractor()
    {
        mLocalContractorsPointer.clear();
        mGlobalContractorsLocation.clear();
    }

    void RegisterLocalContractor( TContractorType & r_contractor )
    {
        ContractorKey key = mContractorKeyIssuer.IssueNewKey();
        mLocalContractorsPointer[key] = & r_contractor;
        mLocalContractorNewlyAdded = true;
    }

    void GenerateGlobalContractorsLocation( const int mpi_tag = 0 )
    {
        typedef std::vector<ContractorKey> ContractorKeyVector;
        typedef std::map<Location, ContractorKeyVector> ContractorKeyVectorMap;

        ContractorKeyVector local_contractor_key_vector;
        ContractorKeyVectorMap global_contractor_key_vector_map;

        //local contractors
        for( const std::pair<ContractorKey, ContractorPointer> & r_local_contractor_pointer_pair : mLocalContractorsPointer )
        {
            ContractorKey local_contractor_key = r_local_contractor_pointer_pair.first;
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

            for ( const ContractorKey & r_global_contractor_key : r_global_contractor_key_vector )
                mGlobalContractorsLocation[r_global_contractor_key] = location;
        }
    }

    ContractorPointer FindLocalContractor( const ContractorKey key ) const
    {
        typename ContractorPointerMap::iterator it = mLocalContractor.find(key);
        if( it == mLocalContractor.end() )
            return nullptr;
        else
            return it->second;
    }

    Location ContactorLocation( const ContractorKey key ) const
    {
        LocationMap::iterator it = mGlobalContractorsLocation.find(key);
        if( it == mGlobalContractorsLocation.end() )
            return Location::Nowhere();
        else
            return it->second;
    }

private:
    TCommunicatorType * mpCommunicator;
    TContractorKeyIssuerType mContractorKeyIssuer;
    ContractorPointerMap mLocalContractorsPointer;
    LocationMap mGlobalContractorsLocation;
    bool mLocalContractorNewlyAdded;
};

}
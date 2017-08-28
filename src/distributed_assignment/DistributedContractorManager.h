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
    using Location = typename TCommunicatorType::Location ;
    using ContractorKey = typename TContractorKeyIssuerType::Key ;
    using ContractorPointer = TContractorType * ;
    using ContractorPointerMapByContractorKey = std::map<ContractorKey, ContractorPointer, typename ContractorKey::LessThanComparator> ;
    using LocationMapByContractorKey = std::map<ContractorKey, Location, typename ContractorKey::LessThanComparator> ;

    DistributedContractorManager() = delete;

    DistributedContractorManager( TCommunicatorType & communicator )
    :   mpCommunicator{& communicator},
        mContractorKeyIssuer(communicator),
        mLocalContractorsPointer(),
        mGlobalContractorsLocation()
    {}

    ~DistributedContractorManager()
    {}

    void ClearRegistratedContractors()
    {
        mLocalContractorsPointer.clear();
        mGlobalContractorsLocation.clear();
    }

    void RegisterLocalContractor( TContractorType & r_contractor )
    {
        ContractorKey key = mContractorKeyIssuer.IssueNewKey();
        mLocalContractorsPointer[key] = & r_contractor;
        r_contractor.SetKey(key);
    }

    void GenerateGlobalContractorsLocation( const int mpi_tag = 0 )
    {
        using ContractorKeyVector = std::vector<ContractorKey> ;
        using ContractorKeyVectorMapByLocation = std::map<Location, ContractorKeyVector, typename Location::LessThanComparator> ;

        ContractorKeyVector local_contractor_key_vector;
        ContractorKeyVectorMapByLocation global_contractor_key_vector_map;

        //local contractors
        for( std::pair<const ContractorKey, ContractorPointer> & r_local_contractor_pointer_pair : mLocalContractorsPointer )
        {
            ContractorKey local_contractor_key = r_local_contractor_pointer_pair.first;
            local_contractor_key_vector.push_back(local_contractor_key);
        }

        //all gather
        mpCommunicator->AllGather( local_contractor_key_vector, global_contractor_key_vector_map, mpi_tag );

        //global contractors location
        mGlobalContractorsLocation.clear();

        for( std::pair<const Location, ContractorKeyVector> & r_global_contractor_key_vector_pair : global_contractor_key_vector_map )
        {
            Location location = r_global_contractor_key_vector_pair.first;
            ContractorKeyVector r_global_contractor_key_vector = r_global_contractor_key_vector_pair.second;

            for ( const ContractorKey & r_global_contractor_key : r_global_contractor_key_vector )
                mGlobalContractorsLocation[r_global_contractor_key] = location;
        }
    }

    void PrintAllContractors() const
    {
        std::cout << __func__ << "local contractors: " << std::endl;

        for( const std::pair<const ContractorKey, ContractorPointer> & r_contractor_pointer_pair : mLocalContractorsPointer )
        {
            const ContractorKey & r_contractor_key = r_contractor_pointer_pair.first;
            const TContractorType & r_contractor = *(r_contractor_pointer_pair.second);

            DataUtility::DataPrinter printer;
            printer.Print(r_contractor_key);
            printer.Print(r_contractor);
        }

        std::cout << std::endl;
        std::cout << __func__ << "global contractors: " << std::endl;

        for( const std::pair<const ContractorKey, Location> & r_contractor_location_pair : mGlobalContractorsLocation )
        {
            const ContractorKey & r_contractor_key = r_contractor_location_pair.first;
            const Location & r_location = r_contractor_location_pair.second;

            DataUtility::DataPrinter printer;
            printer.Print(r_contractor_key);
            printer.Print(r_location);
        }

        std::cout << std::endl;
    }

    const ContractorPointerMapByContractorKey & LocalContractorsPointer() const
    { return mLocalContractorsPointer; }

    const LocationMapByContractorKey & GlobalContractorsLocation() const
    { return mGlobalContractorsLocation; }

    ContractorPointer const FindLocalContractorPointer( const ContractorKey key ) const
    {
        typename ContractorPointerMapByContractorKey::const_iterator it = mLocalContractorsPointer.find(key);
        if( it == mLocalContractorsPointer.end() )
            return nullptr;
        else
            return it->second;
    }

    Location FindGlobalContractorLocation( const ContractorKey key ) const
    {
        typename LocationMapByContractorKey::const_iterator it = mGlobalContractorsLocation.find(key);
        if( it == mGlobalContractorsLocation.end() )
            return Location::NoWhere();
        else
            return it->second;
    }

private:
    TCommunicatorType * const mpCommunicator;
    TContractorKeyIssuerType mContractorKeyIssuer;
    ContractorPointerMapByContractorKey mLocalContractorsPointer;
    LocationMapByContractorKey mGlobalContractorsLocation;
};

}
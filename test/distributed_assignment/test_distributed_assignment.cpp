#include<mpi.h>
#include<iostream>
#include<unistd.h>
#include<vector>
#include"Serializer.h"
#include"DataProfile.h"
#include"DataPrinter.h"
#include"MpiCommunicator.h"
#include"DistributedKeyIssuer.h"
#include"DistributedContractorManager.h"
#include"DistributedAssignmentManager.h"
#include"my_contractor.h"

namespace ForMainOnly
{

typedef Communication::MpiCommunicator Communicator;
typedef typename Communicator::Location Location;
typedef DistributedAssignment::DistributedKeyIssuer<Communicator> ContractorKeyIssuer;
typedef typename ContractorKeyIssuer::Key ContractorKey;

template<typename TContractorType>
using ContractorManagerType = DistributedAssignment::DistributedContractorManager<TContractorType, ContractorKeyIssuer, Communicator>;

typedef std::pair<const ContractorKey, Location> LocationPair;

template<typename TContractorType>
using ContractorPointerPairType = std::pair<const ContractorKey, TContractorType *>;

typedef DataUtility::DataPrinter DataPrinter;

}

int main( int argc, char** argv )
{
    using namespace ForMainOnly;

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    if ( mpi_rank == 0 )  std::cin >> dump;

    A a0;
    SomeOne someone0;

    //communicator
    Communicator communicator(MPI_COMM_WORLD);

    // contractor manager
    ContractorManagerType<SomeOne> contractor_manager(communicator);

    contractor_manager.ClearRegistratedContractor();
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.GenerateGlobalContractorsLocation();

    std::cout << "local contractors" << std::endl;
    for( const ContractorPointerPairType<SomeOne> & r_contractor_pointer_pair : contractor_manager.LocalContractorsPointer() )
    {
        const ContractorKey & r_contractor_key = r_contractor_pointer_pair.first;
        const SomeOne & r_contractor = *(r_contractor_pointer_pair.second);

        DataPrinter printer;
        printer.Print(r_contractor_key);
        printer.Print(r_contractor);
    }

    std::cout << std::endl;
    std::cout << "global contractors" << std::endl;
    for( const LocationPair & r_location_pair : contractor_manager.GlobalContractorsLocation() )
    {
        const ContractorKey & r_contractor_key = r_location_pair.first;
        const Location & r_location = r_location_pair.second;

        DataPrinter printer;
        printer.Print(r_contractor_key);
        printer.Print(r_location);
    }

    // assignment manager
    DistributedAssignmentManager assignment_manager<SomeOne,SomeOne,int,A,MpiCommunicator,DistributedKeyIssuer,DistributedKeyIssuer> (communicator, contractor_manager, contractor_manager);

    for( const ContractorPointerPairByContractorKey & r_contractor_pointer_pair : contractor_manager.LocalContractorsPointer() )
    {
        const ContractorKey & r_assignor_key = r_contractor_pointer_pair.first;

        for( const LocationPairByContractorKey & r_contractor_location_pair : contractor_manager.GlobalContractorsLocation() )
        {
            const ContractorKey & r_assignee_key = r_contractor_location_pair.first;

            assignment_manager.AddAssignment(r_assignor_key, r_assignee_key, 1);
        }
    }


    std::cin >> dump;

    MPI_Finalize();

}
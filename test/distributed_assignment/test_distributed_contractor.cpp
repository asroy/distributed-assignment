#include<mpi.h>
#include<iostream>
#include<unistd.h>
#include<vector>
#include"Serializer.h"
#include"DataProfile.h"
#include"DataPrinter.h"
#include"MpiCommunicator.h"
#include"DistributedContractorManager.h"
#include"DistributedKeyIssuer.h"
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
    // if ( mpi_rank == 0 )  std::cin >> dump;

    typedef MyContractorA<ContractorKey> ContractorA;

    ContractorA contractor_a;

    Communicator communicator(MPI_COMM_WORLD);

    ContractorManagerType<ContractorA> contractor_manager(communicator);

    contractor_manager.ClearRegistratedContractor();
    contractor_manager.RegisterLocalContractor(contractor_a);
    contractor_manager.GenerateGlobalContractorsLocation();

    std::cout << "global contractors, size:" << contractor_manager.GlobalContractorsLocation().size() << std::endl;


    std::cout << "local contractors" << std::endl;
    for( const ContractorPointerPairType<ContractorA> & r_contractor_pointer_pair : contractor_manager.LocalContractorsPointer() )
    {
        const ContractorKey & r_contractor_key = r_contractor_pointer_pair.first;
        const ContractorA & r_contractor = *(r_contractor_pointer_pair.second);

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

    std::cin >> dump;

    MPI_Finalize();

}

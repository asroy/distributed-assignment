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
#include"AssignmentData.h"
#include"my_contractor.h"

namespace ForMainOnly
{

using Communicator = Communication::MpiCommunicator;
using Location = typename Communicator::Location;
using ContractorKeyIssuer = DistributedAssignment::DistributedKeyIssuer<Communicator>;
using ContractorKey = typename ContractorKeyIssuer::Key ;

template<typename TContractorType>
using ContractorManagerType = DistributedAssignment::DistributedContractorManager<TContractorType, ContractorKeyIssuer, Communicator>;

using LocationPairByContractorKey = std::pair<const ContractorKey, Location> ;

template<typename TContractorType>
using ContractorPointerPairType = std::pair<const ContractorKey, TContractorType *>;

using AssignmentKeyIssuer = DistributedAssignment::DistributedKeyIssuer<Communicator> ;
using AssignmentKey = typename AssignmentKeyIssuer::Key ;

template<typename TDataType>
using AssignmentDataType = DistributedAssignment::AssignmentData<ContractorKey,AssignmentKey,TDataType>;

template<typename TAssignorType, typename TAssigneeType, typename TInputType, typename TOutputType>
using AssignmentManager = DistributedAssignment::DistributedAssignmentManager<TAssignorType,TAssigneeType,TInputType,TOutputType,Communicator,DistributedAssignment::DistributedKeyIssuer,DistributedAssignment::DistributedKeyIssuer> ;

using DataPrinter =DataUtility::DataPrinter ;

}

int main( int argc, char** argv )
{
    using namespace ForMainOnly;

    using Contractor = MyContractorA<ContractorKey>;

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    // if ( mpi_rank == 0 )  std::cin >> dump;

    Contractor someone0;

    //communicator
    Communicator communicator(MPI_COMM_WORLD);

    //contractor manager
    ContractorManagerType<Contractor> contractor_manager(communicator);

    contractor_manager.ClearRegistratedContractors();
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.GenerateGlobalContractorsLocation();
    // contractor_manager.PrintAllContractors();

    //assignment manager
    ContractorManagerType<Contractor> & r_assignor_manager = contractor_manager;
    ContractorManagerType<Contractor> & r_assignee_manager = contractor_manager;

    AssignmentManager<Contractor,Contractor,int,A>  assignment_manager(communicator, r_assignor_manager, r_assignee_manager);

    for( const ContractorPointerPairType<Contractor> & r_assignor_pointer_pair : r_assignor_manager.LocalContractorsPointer() )
    {
        const ContractorKey assignor_key = r_assignor_pointer_pair.first;

        for( const LocationPairByContractorKey & r_assignee_location_pair : r_assignee_manager.GlobalContractorsLocation() )
        {
            const ContractorKey assignee_key = r_assignee_location_pair.first;

            for( int i = 0; i < 10; i++ )
            {
                assignment_manager.AddAssignment(assignor_key, assignee_key, 1);
            }
        }
    }

    //work
    assignment_manager.ExecuteAllDistributedAssignments();

    // assignment_manager.PrintAllAssignments();


    // print results
    using AssignmentOutput = AssignmentDataType<A> ;
    using AssignmentOutputVector = std::vector<AssignmentOutput> ;

    AssignmentOutputVector results;

    assignment_manager.GetResults( results );

    // std::cout<<__func__<<": Results: "<<std::endl;
    // DataUtility::DataPrinter printer;
    // printer.Print(results);
    // std::cout<<std::endl;

    // std::cin >> dump;

    MPI_Finalize();

}
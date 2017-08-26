#include<mpi.h>
#include<iostream>
#include<unistd.h>
#include<vector>
#include"Serializer.h"
#include"DataProfile.h"
#include"DataPrinter.h"
#include"MpiCommunicator.h"
#include"my_contractor.h"

int main( int argc, char** argv )
{
    typedef typename MpiCommunicator::Location Location;
    typedef DistributedKeyIssuer<MpiCommunicator> ContractorKeyIssuer;
    typedef typename ContractorKeyIssuer::Key ContractorKey;
    typedef std::pair<const ContractorKey, SomeOne *> ContractorPointerPairByContractorKey;
    typedef std::pair<const ContractorKey, Location> LocationPairByContractorKey;

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    // std::cin >> dump;

    A a0;
    SomeOne someone0;

    MpiCommunicator communicator(MPI_COMM_WORLD);

    DistributedContractorManager contractor_manager<SomeOne, SimpleKeyIssuer, MpiCommunicator> (communicator);

    contractor_manager.ClearRegistratedContractor();
    contractor_manager.RegisterLocalContractor(someone0);
    contractor_manager.GenerateGlobalContractorsLocation();

    // assignment
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

















}
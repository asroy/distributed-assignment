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
#include"ping_pong_player.h"

namespace ForMainOnly
{

using Communicator = Communication::MpiCommunicator;
using Location = typename Communicator::Location;
using ContractorKeyIssuer = DistributedAssignment::DistributedKeyIssuer<Location>;
using ContractorKey = typename ContractorKeyIssuer::Key ;
using ContractorKeyVector = std::vector<ContractorKey> ;
using ContractorKeySet = std::set<ContractorKey, typename ContractorKey::LessThanComparator> ;

template<typename TContractorType>
using ContractorManagerType = DistributedAssignment::DistributedContractorManager<TContractorType, Communicator, DistributedAssignment::DistributedKeyIssuer >;

using LocationPairByContractorKey = std::pair<const ContractorKey, Location> ;

template<typename TContractorType>
using ContractorPointerPairType = std::pair<const ContractorKey, TContractorType *>;

using AssignmentKeyIssuer = DistributedAssignment::DistributedKeyIssuer<Location> ;
using AssignmentKey = typename AssignmentKeyIssuer::Key ;

template<typename TDataType>
using AssignmentDataType = DistributedAssignment::AssignmentData<ContractorKey,AssignmentKey,TDataType>;

template<typename TDataType>
using AssignmentDataVectorType = std::vector<AssignmentDataType<TDataType>> ;

template<typename TAssignorType, typename TAssigneeType, typename TInputType, typename TOutputType>
using AssignmentManager = DistributedAssignment::DistributedAssignmentManager<TAssignorType,TAssigneeType,TInputType,TOutputType,Communicator,DistributedAssignment::DistributedKeyIssuer,DistributedAssignment::DistributedKeyIssuer> ;

using DataPrinter =DataUtility::DataPrinter ;

}

int main( int argc, char** argv )
{
    using namespace ForMainOnly;

    using Contractor = PingPongPlayer<ContractorKey>;
    using Output = Ball<ContractorKey>;

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, & mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, & mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    // std::cin >> dump;
    if ( mpi_rank == 0 )  std::cin >> dump;

    Contractor player_a;
    Contractor player_b;

    //communicator
    Communicator communicator(MPI_COMM_WORLD);

    //player A manager
    ContractorManagerType<Contractor> player_a_manager(communicator);

    player_a_manager.ClearContractorsRegistry();
    player_a_manager.RegisterLocalContractor(player_a);
    player_a_manager.GenerateGlobalContractorsRegistry();
    // player_a_manager.PrintAllContractors();

    //player B manager
    ContractorManagerType<Contractor> player_b_manager(communicator);

    player_b_manager.ClearContractorsRegistry();
    player_b_manager.RegisterLocalContractor(player_b);
    player_b_manager.GenerateGlobalContractorsRegistry();
    // player_b_manager.PrintAllContractors();

    // rival list
    const ContractorKeySet & r_a_players_key = player_a_manager.GlobalContractorsKey();
    const ContractorKeySet & r_b_players_key = player_b_manager.GlobalContractorsKey();

    //let players know their rivals
    for( const ContractorPointerPairType<Contractor> & r_local_player_a_pointer_pair : player_a_manager.LocalContractorsPointer() )
    {
        Contractor * p_local_player_a_pointer = r_local_player_a_pointer_pair.second;
        p_local_player_a_pointer->SetRivals(r_b_players_key);
    }

    for( const ContractorPointerPairType<Contractor> & r_local_player_b_pointer_pair : player_b_manager.LocalContractorsPointer() )
    {
        Contractor * p_local_player_b_pointer = r_local_player_b_pointer_pair.second;
        p_local_player_b_pointer->SetRivals(r_a_players_key);
    }


    // initialize ball
    Output ball = { *(r_a_players_key.begin()), *(r_b_players_key.begin()) };

    //assignment
    for( int i_round = 0; i_round < 100; i_round++ )
    {
        // if( mpi_rank == 0 )
            std::cout <<"round "<<i_round<<std::endl;

        AssignmentDataVectorType<Output> balls;

        //assignment: ping
        AssignmentManager<Contractor,Contractor,int,Output>  assignment_a_manager(communicator, player_a_manager, player_b_manager);

        Contractor * p_player_a = player_a_manager.FindLocalContractorPointer( ball.mFrom );

        if( p_player_a != nullptr )
        {
            std::cout <<"add a assignment"<<std::endl;
            assignment_a_manager.AddAssignment(p_player_a->GetKey(), ball.mTo, 1 );
        }

        //work
        assignment_a_manager.ExecuteAllDistributedAssignments();
        // assignment_a_manager.PrintAllAssignments();
        // assignment_a_manager.PrintResults();
        assignment_a_manager.GetResults( balls );

        //catch ball
        if( balls.size() > 0 ) ball = balls[0].GetData();


        //assignment: pong
        AssignmentManager<Contractor,Contractor,int,Output>  assignment_b_manager(communicator, player_b_manager, player_a_manager);

        Contractor * p_player_b = player_b_manager.FindLocalContractorPointer( ball.mFrom );

        if( p_player_b != nullptr )
        {
            std::cout <<"add b assignment"<<std::endl;
            assignment_b_manager.AddAssignment(p_player_b->GetKey(), ball.mTo, 1 );
        }

        //work
        assignment_b_manager.ExecuteAllDistributedAssignments();
        // assignment_b_manager.PrintAllAssignments();
        // assignment_b_manager.PrintResults();
        assignment_b_manager.GetResults( balls );

        //catch ball
        if( balls.size() > 0 ) ball = balls[0].GetData();
    }

    MPI_Finalize();

    std::cout<<"done!"<<std::endl;
    // std::cin >> dump;
}
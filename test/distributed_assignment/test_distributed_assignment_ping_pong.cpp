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

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, & mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, & mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    // std::cin >> dump;
    // if ( mpi_rank == 0 )  std::cin >> dump;

    //communicator
    Communicator communicator(MPI_COMM_WORLD);


    //player A manager
    int player_a_num = mpi_rank+1;
    Contractor player_a[player_a_num];

    ContractorManagerType<Contractor> player_a_manager(communicator);

    player_a_manager.ClearContractorsRegistry();
    for( int i = 0; i < player_a_num; i++ )
    {
        player_a_manager.RegisterLocalContractor(player_a[i], "PlayerA");
    }
    player_a_manager.GenerateGlobalContractorsRegistry();
    // player_a_manager.PrintAllContractors();


    //player B manager
    int player_b_num = mpi_size-mpi_rank;
    Contractor player_b[player_b_num];

    ContractorManagerType<Contractor> player_b_manager(communicator);

    player_b_manager.ClearContractorsRegistry();
    for( int i = 0; i < player_b_num; i++ )
    {
        player_b_manager.RegisterLocalContractor(player_b[i], "PlayerB");
    }
    player_b_manager.GenerateGlobalContractorsRegistry();
    // player_b_manager.PrintAllContractors();

    // assignment manager
    AssignmentManager<Contractor,Contractor,int,int>  assignment_a_manager(communicator, player_a_manager, player_b_manager);
    AssignmentManager<Contractor,Contractor,int,int>  assignment_b_manager(communicator, player_b_manager, player_a_manager);


    // first pong, assigned by player A, to player B
    {
        ContractorKey player_a_key = *(player_a_manager.GlobalContractorsKey().begin());
        ContractorKey player_b_key = *(player_b_manager.GlobalContractorsKey().begin());

        if ( player_a_manager.LocalContractorsKey().find(player_a_key) != player_a_manager.LocalContractorsKey().end() )
        {
            assignment_a_manager.AddAssignment(player_a_key, player_b_key, 0 );
        }

        assignment_a_manager.ExecuteAllDistributedAssignments();

    }

    // start ping/pong loop
    for( int i_round = 0; i_round < 100; i_round++ )
    {
        std::cout << "round " << i_round << std::endl;


        AssignmentDataVectorType<int> assignment_a_result_vector;
        assignment_a_manager.GetResultsAtAssignee(assignment_a_result_vector);

        //ping: assignment B
        // for each results of assignment A, assign an assignment B
        assignment_b_manager.ClearAllAssignment();

        for( AssignmentDataType<int> & assignment_a_result : assignment_a_result_vector )
        {
            ContractorKey last_player_a_key = assignment_a_result.GetAssignorKey();
            ContractorKey player_b_key = assignment_a_result.GetAssigneeKey();

            ContractorKeySet::iterator it_last_player_a_key = player_a_manager.GlobalContractorsKey().find(last_player_a_key);

            ContractorKeySet::iterator it_next_player_a_key = std::next(it_last_player_a_key);
            if( it_next_player_a_key == player_a_manager.GlobalContractorsKey().end() )
                it_next_player_a_key = player_a_manager.GlobalContractorsKey().begin();

            ContractorKey next_player_a_key = *it_next_player_a_key;

            assignment_b_manager.AddAssignment(player_b_key, next_player_a_key, 0 );
        }

        assignment_b_manager.ExecuteAllDistributedAssignments();

        AssignmentDataVectorType<int> assignment_b_result_vector;
        assignment_b_manager.GetResultsAtAssignee(assignment_b_result_vector);

        //ping: assignment A
        // for each results of assignment B, assign an assignment A
        assignment_a_manager.ClearAllAssignment();

        for( AssignmentDataType<int> & assignment_b_result : assignment_b_result_vector )
        {
            ContractorKey last_player_b_key = assignment_b_result.GetAssignorKey();
            ContractorKey player_a_key = assignment_b_result.GetAssigneeKey();

            ContractorKeySet::iterator it_last_player_b_key = player_b_manager.GlobalContractorsKey().find(last_player_b_key);

            ContractorKeySet::iterator it_next_player_b_key = std::next(it_last_player_b_key);
            if( it_next_player_b_key == player_b_manager.GlobalContractorsKey().end() )
                it_next_player_b_key = player_b_manager.GlobalContractorsKey().begin();

            ContractorKey next_player_b_key = *it_next_player_b_key;

            assignment_a_manager.AddAssignment(player_a_key, next_player_b_key, 0 );
        }

        assignment_a_manager.ExecuteAllDistributedAssignments();
    }

    MPI_Finalize();

    std::cout<<"done!"<<std::endl;
    std::cin >> dump;
}
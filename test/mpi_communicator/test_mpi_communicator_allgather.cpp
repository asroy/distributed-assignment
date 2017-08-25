#include<mpi.h>
#include<iostream>
#include<unistd.h>
#include<vector>
#include"Serializer.h"
#include"DataProfile.h"
#include"DataPrinter.h"
#include"MpiCommunicator.h"
#include"my_data.h"

int main( int argc, char** argv )
{
    typedef Communication::MpiCommunicator::Location Location;
    typedef std::vector<A> DataVector;
    typedef std::pair<const Location, std::vector<A>> DataVectorPair;

    int mpi_rank, mpi_size;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );

    std::cout << "rank " << mpi_rank << " PID "<< ::getpid() << std::endl;

    int dump;
    // std::cin >> dump;

    A a0 = {mpi_rank,1.0*mpi_rank,1.0*mpi_rank,1.0*mpi_rank,{mpi_rank,1.0*mpi_rank,'b'}};

    //communicator
    Communication::MpiCommunicator communicator;

    //data containers
    DataVector send_data_vector;
    std::map<Location, DataVector, Location::LessThan> recv_data_vector_map;

    //data to be sent
    for( int j = 0; j < mpi_rank; j++ )
        send_data_vector.push_back(a0);

    //all gather
    communicator.AllGather( send_data_vector, recv_data_vector_map, 0 );

    //print recv
    for( const DataVectorPair & r_recv_data_vector_pair : recv_data_vector_map )
    {
        Location recv_from_location = r_recv_data_vector_pair.first;
        const DataVector & r_rev_data_vector = r_recv_data_vector_pair.second;

        std::cout<<"recv from rank "<<recv_from_location.MpiRank()<<std::endl;
        std::cout<<"recv vector sizes: "<<r_rev_data_vector.size()<<std::endl;

        DataUtilities::DataPrinter data_printer;
        data_printer.Print(recv_data_vector_map[recv_from_location]);
        std::cout<<std::endl;
    }

    std::cin >> dump;

    MPI_Finalize();
}

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
  typedef std::vector<A> DataContainerType;

  int mpi_rank, mpi_size;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
  MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );

  std::cout << "rank " << mpi_rank << " getPID "<< ::getpid() << std::endl;

  int dump;
  // std::cin >> dump;


  //build complete communicator
  MpiCommunicator communicator;
  communicator.BuildCompleteCommunication();

  //send data containers
  A a0 = {1,1.1,2.1,3.1,{4,4.1,'b'}};

  DataContainerType send_data_container;
  send_data_container.clear();

  for( int j = 0; j < 10000; j++ )
    send_data_container.push_back(a0);

  //recv data containers
  std::vector<DataContainerType> recv_data_containers;
  recv_data_containers.resize(mpi_size);

  //all gather
  communicator.AllGather( send_data_container, recv_data_containers, 0 );

  //print
  for( int i = 0; i < mpi_size; i++ )
  {
    std::cout<<"recv vector sizes"<<recv_data_containers[i].size()<<std::endl;
    DataPrinter data_printer;
    data_printer.Print(recv_data_containers[i]);
  }


  std::cin >> dump;

  MPI_Finalize();
}

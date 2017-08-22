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


  A a0 = {1,1.1,2.1,3.1,{4,4.1,'b'}};

  //send data containers
  std::vector<DataContainerType> send_data_containers;
  send_data_containers.clear();

  for( int i = 0; i < mpi_size; i++ )
  {
    DataContainerType & r_send_data_container = send_data_containers[i];
    r_send_data_container.clear();
    for( int j = 1; j < mpi_size; j++ )
      r_send_data_container.push_back(a0);
  }

  //recv data containers
  std::vector<DataContainerType> recv_data_containers;
  recv_data_containers.clear();

  //send recv
  MpiCommunicator communicator;
  communicator.BuildCompleteCommunication();
  communicator.AllSendAllRecv( send_data_containers, recv_data_containers, 0 );

  //print
  for( int i = 0; i < mpi_size; i++ )
  {
    DataPrinter data_printer;
    data_printer.Print(recv_data_containers[i]);
  }


  MPI_Finalize();
}

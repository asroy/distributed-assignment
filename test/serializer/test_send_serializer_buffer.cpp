#include<mpi.h>
#include<iostream>
#include<unistd.h>
#include<vector>
#include"Serializer.h"
#include"DataProfile.h"
#include"DataPrinter.h"
#include"my_data.h"

int main( int argc, char** argv )
{
  int rank, size;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  std::cout << "rank " << rank << " getPID "<< ::getpid() << std::endl;

  int dump;
  // std::cin >> dump;

  if( rank == 0 )
  {
    A a0 = {1,1.1,2.1,3.1,{4,4.1,'b'}};
    A a1 = {2,1.2,2.2,3.2,{4,4.2,'c'}};

    std::vector<A> send_vector;

    for( int i = 0; i < 100000; i++ )
    {
      send_vector.push_back(a0);
      send_vector.push_back(a1);
    }

    //print
    // DataPrinter data_printer;
    // data_printer.Print(send_vector);

    DataUtilities::Serializer send_serial;

    // buffer header
    send_serial.ReserveSpaceForBufferHeader(DataUtilities::DataProfile::Default());

    const std::vector<A> & r_send_vector = send_vector;
    DataUtilities::DataProfile send_data_profile = DataUtilities::DataProfile::Default().Profile(r_send_vector).MakeFromSender();
    send_serial.WriteBufferHeader(send_data_profile);

    std::cout << "send_serial, IsTrivial?" << send_data_profile.IsTrivial() <<std::endl;

    //save data
    int send_size = (int) send_serial.FreshSave(r_send_vector);

    std::cout << rank << ": send_size " << send_size << std::endl;

    MPI_Send( &send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );

    MPI_Send( send_serial.BufferPointer(), send_size, MPI_CHAR, 1, 1, MPI_COMM_WORLD );

    std::cin >> dump;
  }

  if (rank == 1)
  {
    MPI_Status status;

    int recv_size;

    MPI_Recv( &recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

    std::cout << rank << ": recv_size " << recv_size << std::endl;

    DataUtilities::Serializer recv_serial;

    //increase buffer size
    recv_serial.IncreaseBufferSize(recv_size);

    // buffer header
    recv_serial.ReserveSpaceForBufferHeader(DataUtilities::DataProfile::Default());
    recv_serial.WriteBufferHeader(DataUtilities::DataProfile::Default().MakeNotFromSender());

    //receive
    MPI_Recv( recv_serial.BufferPointer(), recv_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status );

    std::vector<A> recv_vector;

    int load_size = (int) recv_serial.FreshLoad(recv_vector);

    DataUtilities::DataProfile recv_data_profile;
    recv_serial.ReadBufferHeader(recv_data_profile);

    std::cout << "recv_serial, IsTrivial?" << recv_data_profile.IsTrivial() <<std::endl;

    std::cout << " recv_size " <<recv_size << " load_size " << load_size << std::endl;
    std::cout << " recv_vector size " <<recv_vector.size() << std::endl;

    //print
    DataUtilities::DataPrinter data_printer;
    data_printer.Print(recv_vector);

    std::cin >> dump;
  }

  MPI_Finalize();
}

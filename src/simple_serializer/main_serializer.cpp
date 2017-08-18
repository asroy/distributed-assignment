#include<mpi.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<typeinfo>
#include<cxxabi.h>
#include<vector>
#include "serializer.h"
#include "my_data.h"

template<typename T>
void type_name(T u)
{
  int     status;
  char   *realname;

  const std::type_info  &ti = typeid(u);

  realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
  free(realname);
}

template void Serializer::Save<A>( const std::vector<A> & );

int main( int argc, char** argv )
{
  int rank, size;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  std::cout << "rank " << rank << " getPID "<< ::getpid() << std::endl;

  // int dump;
  // std::cin >> dump;


  if( rank == 0 )
  {
    A a0, a1;
  
    a0.i = 1;
    a0.x = 1.1;
    a0.y[0] = 2.1;
    a0.y[1] = 3.1; 
  
    a0.b.i = 4;
    a0.b.z = 4.1;
    a0.b.c = 'c';
  
    a1.i = 2;
    a1.x = 1.2;
    a1.y[0] = 2.2;
    a1.y[1] = 3.2; 
  
    a1.b.i = 4;
    a1.b.z = 4.2;
    a1.b.c = 'c';
  
    std::cout << "rank" << rank << a0.i << a0.x << a0.y[0] << a0.y[1] << a0.b.i << a0.b.z << a0.b.c << std::endl;
    std::cout << "rank" << rank << a1.i << a1.x << a0.y[0] << a1.y[1] << a1.b.i << a1.b.z << a1.b.c << std::endl;

    std::vector<A> send_vector;
    
    send_vector.push_back(a0);
    send_vector.push_back(a1);

    Serializer send_serial;

    send_serial.ReallocateBuffer(200);


    const std::vector<A> & r_send_vector = send_vector;
    send_serial.Save(r_send_vector);

    int send_size = (int) send_serial.BufferSaveSize();

    std::cout << rank << ": send_size " << send_size << std::endl;

    MPI_Send( &send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );

    MPI_Send( send_serial.BufferPointer(), send_size, MPI_CHAR, 1, 1, MPI_COMM_WORLD );
  }


  if (rank == 1)
  {
    MPI_Status status;

    int recv_size;

    MPI_Recv( &recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

    std::cout << rank << ": recv_size " << recv_size << std::endl;

    Serializer recv_serial;

    recv_serial.ReallocateBuffer(recv_size);

    MPI_Recv( recv_serial.BufferPointer(), recv_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status );

    std::vector<A> recv_vector;

    recv_serial.Load(recv_vector);

    A a2, a3;

    a2 = recv_vector[0];
    a3 = recv_vector[1];

    std::cout << "rank" << rank << a2.i << a2.x << a2.y[0] << a2.y[1] << a2.b.i << a2.b.z << a2.b.c << std::endl;
    std::cout << "rank" << rank << a3.i << a3.x << a3.y[0] << a3.y[1] << a3.b.i << a3.b.z << a3.b.c << std::endl;
  }

  MPI_Finalize();
}

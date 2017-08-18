#include<mpi.h>
#include<iostream>
#include<sstream>
#include<sys/types.h>
#include<unistd.h>
#include<cstring>
#include<cstdio>
#include<typeinfo>
#include<cxxabi.h>

struct B
{
  int i;
  double z;
  char c;
};


struct A
{
  int i;
  double x;
  double y[2];

  B b;
};

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

    int send_size = 2*(int)sizeof(a0);

    char* send_c_ptr = new char[send_size];

    A* send_ptr = (A*) send_c_ptr;

    send_ptr[0] = a0;
    send_ptr[1] = a1;

    MPI_Send( &send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );

    MPI_Send( send_c_ptr, send_size, MPI_CHAR, 1, 1, MPI_COMM_WORLD );

    delete[] send_c_ptr;
  }


  if (rank == 1)
  {
    MPI_Status status;

    int recv_size;

    MPI_Recv( &recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

    char* recv_c_ptr = new char[recv_size];

    MPI_Recv( recv_c_ptr, recv_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status );

    A* recv_ptr = (A*)recv_c_ptr;

    A a2, a3;

    a2 = recv_ptr[0];
    a3 = recv_ptr[1];

    std::cout << "rank" << rank << a2.i << a2.x << a2.y[0] << a2.y[1] << a2.b.i << a2.b.z << a2.b.c << std::endl;
    std::cout << "rank" << rank << a3.i << a3.x << a3.y[0] << a3.y[1] << a3.b.i << a3.b.z << a3.b.c << std::endl;

    delete[] recv_c_ptr;
  }

  MPI_Finalize();


}

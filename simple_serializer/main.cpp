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

  int dump;
  std::cin >> dump;

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

    std::stringstream send;

    send.write( (char*)(&a0), sizeof(a0) );
    send.write( (char*)(&a1), sizeof(a1) );

    std::cout <<"rank" << rank << send.str() << std::endl;

    int send_size;

    send_size = (int)(send.str().size())+1;

    MPI_Send( &send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );

    char* send_c_str = new char[send.str().size()+1];
    char* p = std::strcpy(send_c_str, send.str().c_str());
    std::string send_str;

    std::cout << "type!!!" << typeid(send.str().c_str()).name() << std::endl;
    std::cout << "type!!!" << typeid(send_str).name() << std::endl;
    type_name(send.str().c_str());

    if( p == NULL )
    {
      printf("wrongxxxxxxxxxxxxxxxxxxxxxxxxx");
    }

    printf("send.str().c_str():");
    for(size_t i = 0; i < send.str().size()+1; i++)
      printf("%c",(send.str().c_str())[i]);
    printf("---\n");

    printf("send_c_str:");
    for(size_t i = 0; i < send.str().size()+1; i++)
      printf("%c",(send_c_str[i]));
    printf("---\n");

    MPI_Send( send_c_str, send_size, MPI_CHAR, 1, 1, MPI_COMM_WORLD );

    std::cout << "rank" << rank << "send_size"<< send_size << std::endl;
    std::cout << "rank" << rank << "send.str() "<< send.str() << std::endl;
    std::cout << "rank" << rank << "send.str().c_str() "<< send.str().c_str() << std::endl;
    std::cout << "rank" << rank << "send_c_str "<< send_c_str << std::endl;

    std::printf("rank %d send.str().c_str() %s---\n", rank, send.str().c_str());
    std::printf("rank %d send_c_str %s---\n", rank, send_c_str);

    int dump;
    std::cin >> dump;
    std::cin >> dump;
  }


  if (rank == 1)
  {
    // int dump;
    // std::cin >> dump;

    MPI_Status status;

    int recv_size;

    MPI_Recv( &recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

    std::cout << "rank+++" << rank << "recv_size"<< recv_size << std::endl;

    char* recv_c_str = new char[recv_size];

    MPI_Recv( recv_c_str, recv_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status );

    std::cout << "rank" << rank << "recv_size"<< recv_size << std::endl;
    std::cout << "rank" << rank << "recv_c_str "<< recv_c_str << std::endl;

    std::printf("rank %d recv_c_str %s\n", rank, recv_c_str);

    printf("recv_size %d, recv_c_str:", recv_size);
    for(size_t i = 0; i < recv_size; i++)
      printf("%c",recv_c_str[i]);
    printf("---\n");

    std::string recv_str;
    recv_str.assign(recv_c_str);

    std::stringstream recv;
    recv.str(recv_str);

    A a2, a3;

    recv.read( (char*)(&a2), sizeof(a2) );
    recv.read( (char*)(&a3), sizeof(a3) );

    std::cout << "rank" << rank << a2.i << a2.x << a2.y[0] << a2.y[1] << a2.b.i << a2.b.z << a2.b.c << std::endl;
    std::cout << "rank" << rank << a3.i << a3.x << a3.y[0] << a3.y[1] << a3.b.i << a3.b.z << a3.b.c << std::endl;

    int dump;
    std::cin >> dump;
    std::cin >> dump;
  }

  MPI_Finalize();


}

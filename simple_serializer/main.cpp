#include<mpi.h>
#include<iostream>
#include<sstream>

using namespace std;

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


int main( int argc, char** argv )
{
  int rank, size;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  if( rank == 0) 
  {
    int dump;
    cin >> dump;
  }

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
  
    cout << "rank" << rank << a0.i << a0.x << a0.y[0] << a0.y[1] << a0.b.i << a0.b.z << a0.b.c << endl;
    cout << "rank" << rank << a1.i << a1.x << a0.y[0] << a1.y[1] << a1.b.i << a1.b.z << a1.b.c << endl;

    stringstream send;

    send.write( (char*)(&a0), sizeof(a0) );
    send.write( (char*)(&a1), sizeof(a1) );

    cout <<"rank" << rank << send.str() << endl;

    int send_size;

    send_size = (int)(send.str().size());

    MPI_Send( &send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );

    string send_str;

    send_str = send.str();

    MPI_Send( &send_str, send_size, MPI_BYTE, 1, 1, MPI_COMM_WORLD );

    cout << "rank" << rank << "send_size"<< send_size << endl;
    cout << "rank" << rank << "send_str "<< send_str << endl;
  }


  if (rank == 1)
  {
    MPI_Status status;

    int recv_size;

    MPI_Recv( &recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

    cout << "rank+++" << rank << "recv_size"<< recv_size << endl;

    string recv_str;

    recv_str.resize(recv_size);

    MPI_Recv( &recv_str, recv_size, MPI_BYTE, 0, 1, MPI_COMM_WORLD, &status );

    cout << "rank" << rank << "recv_size"<< recv_size << endl;
    cout << "rank" << rank << "recv_str "<< recv_str << endl;

    stringstream recv;
    recv.str(recv_str);

    A a2, a3;

    recv.read( (char*)(&a2), sizeof(a2) );
    recv.read( (char*)(&a3), sizeof(a3) );

    cout << "rank" << rank << a2.i << a2.x << a2.y[0] << a2.y[1] << a2.b.i << a2.b.z << a2.b.c << endl;
    cout << "rank" << rank << a3.i << a3.x << a3.y[0] << a3.y[1] << a3.b.i << a3.b.z << a3.b.c << endl;
  }

  MPI_Finalize();

}

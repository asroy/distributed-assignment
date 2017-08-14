#include<mpi.h>
#include"serializer.h"

class MpiLocation
{
  public:
    MpiLocation( MPI_Comm comm, int rank, int size ) 
      : mComm{comm}, mRank{rank}, mSize{size}
    { }

    struct LessThan
    {
      bool operator() ( const MpiLocation & a, const MpiLocation & b ) const
      { return ( a.mRank < b.mRank ); }
    }

    MpiLocation WhatIsThisPlace()
    {
      int rank, size;
    
      MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_Size(MPI_COMM_WORLD, &size);
    
      MpiLocation mpi_location(MPI_COMM_WORLD, rank, size );

      return mpi_location;
    }

  private:
    MPI_Comm mComm;
    int mRank;
    int mSize;

  friend class Serializer;
};

template void Serializer::Save<MpiLocation>(const MpiLocation &);
template void Serializer::Load<MpiLocation>(const MpiLocation &);

class MpiWorldMap
{
  public:
    typedef std::vector<MpiLocation> LocationContainerType;

    void Build()
    {
      MpiLocation mpi_location = WhatIsThisPlace();

      MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_Size(MPI_COMM_WORLD, &size);

       
    }

    LocationContainerType AllLocations()
    { return mAllLocations; }

    LocationContainerType AccessibleLocations()
    { return mAccessibleLocations; }

  private:
    LocationContainerType mAllLocations;
    LocationContainerType mAccessibleLocations;
};

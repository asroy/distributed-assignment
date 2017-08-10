#include <mpi.h>

class ContractorMpiResidency
{
  public:
    ContractorMpiResidency() 
      : mComm{MPI_COMM_WORLD}, mRank{0}, mSize{0}
    { 
      MPI_Comm_Rank(MPI_COMM_WORLD, &mRank);
      MPI_Comm_Size(MPI_COMM_WORLD, &mSize);
    }

    struct LessThan
    {
      bool operator() ( const ContractorMpiResidency & a, const ContractorMpiResidency & b ) const
      { return ( a.mRank < b.mRank ); }
    }

  private:
    MPI_Comm mComm;
    int mRank;
    int mSize;
};


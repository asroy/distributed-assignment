#pragma once

#include<iostream>

namespace Communication
{

//implementation of Location type for MpiCommunicator
class MpiLocation
{
public:
    MpiLocation()
    {}

    MpiLocation( MPI_Comm comm, int rank, int size )
      : mMpiComm{comm},
        mMpiRank{rank},
        mMpiSize{size}
    {}

    ~MpiLocation()
    {}

    static MpiLocation NoWhere()
    {
        MpiLocation location(-1,-1,-1);
        return location;
    }

    MPI_Comm MpiComm() const
    { return mMpiComm; }

    int MpiRank() const
    { return mMpiRank; }

    int MpiSize() const
    { return mMpiSize; }

    struct LessThan
    {
        bool operator() ( const MpiLocation & a, const MpiLocation & b ) const
        {
            if( a.mMpiComm != b.mMpiComm )
            {
                std::cout << __func__ << "two MpiLocation not in the same not in the same MPI communicator! exit" << std::endl;
                exit(EXIT_FAILURE);
            }
            return ( a.mMpiRank < b.mMpiRank );
        }
    };

private:
    MPI_Comm mMpiComm;
    int mMpiRank;
    int mMpiSize;
};

}
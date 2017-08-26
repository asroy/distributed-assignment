#pragma once
#include<iostream>
#include<mpi.h>

namespace Communication
{

//implementation of Location type for MpiCommunicator
class MpiLocation
{
public:
    MpiLocation() = delete;

    ~MpiLocation()
    {}

    static MpiLocation NoWhere()
    {
        return MpiLocation{0, -1, -1};
    }

    struct LessThanComparator
    {
        bool operator() ( const MpiLocation & a, const MpiLocation & b ) const
        {
            if( a.mMpiComm != b.mMpiComm )
            {
                std::cout << __func__ << "two MpiLocation not in the same MPI communicator! exit" << std::endl;
                exit(EXIT_FAILURE);
            }
            return ( a.mMpiRank < b.mMpiRank );
        }
    };

private:
    MpiLocation( MPI_Comm comm, int rank, int size )
    :   mMpiComm{comm},
        mMpiRank{rank},
        mMpiSize{size}
    {}

    MPI_Comm MpiComm() const
    { return mMpiComm; }

    int MpiRank() const
    { return mMpiRank; }

    int MpiSize() const
    { return mMpiSize; }

    void Printer( const DataUtility::DataPrinter & r_printer ) const
    {
        char comm_name[MPI_MAX_OBJECT_NAME];
        int comm_name_length;

        MPI_Comm_get_name(mMpiComm, comm_name, & comm_name_length);

        std::string comm_name_string;
        comm_name_string.copy(comm_name, comm_name_length);

        std::cout << "{MpiLocation: " << comm_name_string;
        r_printer.Print(mMpiRank);
        r_printer.Print(mMpiSize);
        std::cout << "}";
    }

    MPI_Comm mMpiComm;
    int mMpiRank;
    int mMpiSize;

    friend class MpiCommunicator;
    friend class DataUtility::DataPrinter;
};

}
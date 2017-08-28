#pragma once
#include<iostream>
#include<mpi.h>

namespace Communication
{

//implementation of Location type for MpiCommunicator
class MpiLocation
{
public:
    MpiLocation()
    :   mMpiComm{MPI_COMM_NULL},
        mMpiRank{-1},
        mMpiSize{-1}
    {}

    ~MpiLocation()
    {}

    static MpiLocation NoWhere()
    {
        return MpiLocation{MPI_COMM_NULL, -1, -1};
    }

    bool operator == (const MpiLocation & other) const
    {
        if( mMpiComm != other.mMpiComm )
            return false;
        else if( mMpiRank != other.mMpiRank )
            return false;
        else if( mMpiSize != other.mMpiSize )
            return false;
        else
            return true;
    }

    bool operator != (const MpiLocation & other) const
    { return !( *this == other ); }

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

    void Save( DataUtility::Serializer & r_serializer ) const
    {
        r_serializer.Save(mMpiComm);
        r_serializer.Save(mMpiRank);
        r_serializer.Save(mMpiSize);
    }

    void Load( DataUtility::Serializer & r_serializer )
    {
        r_serializer.Load(mMpiComm);
        r_serializer.Load(mMpiRank);
        r_serializer.Load(mMpiSize);
    }

    void Profile( DataUtility::DataProfile & r_profile ) const
    {
       r_profile.SetIsTrivial(false);
    }

    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{MpiLocation: ";
        r_printer.Print(mMpiComm);
        r_printer.Print(mMpiRank);
        r_printer.Print(mMpiSize);
        std::cout << "},";
    }

    MPI_Comm mMpiComm;
    int mMpiRank;
    int mMpiSize;

    friend class MpiCommunicator;
    friend class DataUtility::Serializer;
    friend class DataUtility::DataProfile;
    friend class DataUtility::DataPrinter;
};

}
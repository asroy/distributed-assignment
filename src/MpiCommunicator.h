#include<mpi.h>
#include"Serializer.h"

class MpiCommunicator
{
  public:
    typedef MpiLocation LocationType;
    typedef MpiLocation::LessThan LocationLessThanType;
    typedef std::vector<MpiLocation> LocationContainerType;

    MpiCommunicator(const MPI_Comm comm)
      : mComm{comm}
    {
      MPI_Comm_Rank(mComm, &rank);
      MPI_Comm_Size(mComm, &size);

      mAccessibleLocations.clear();
      mAccessibleLocations.push_back(MpiLocation mpi_location(MPI_COMM_WORLD, rank, size);

      mAllLocations.clear();
      for( int i = 0; i < size; i++ )
      {
        MpiLocation mpi_location(MPI_COMM_WORLD, i, size);
        mAllLocations.push_back(mpi_location);
      }

      mSendSerializers.clear();
      mRecvSerializers.clear();

      mSendSerializers.resize(size);
      mRecvSerializers.resize(size);

      for ( Serializer serializer : mSendSerializers )
        serializer.ReallocateBuffer(1000);

      for ( Serializer serializer : mRecvSerializers )
        serializer.ReallocateBuffer(1000);
    }

    LocationType Here()
    {
      int rank, size;
    
      MPI_Comm_Rank(mComm, &rank);
      MPI_Comm_Size(mComm, &size);
    
      LocationType mpi_location( mComm, rank, size );

      return mpi_location;
    }

    const LocationContainerType & AllLocations() const
    { return mAllLocations; }

    const LocationContainerType & AccessibleLocations() const
    { return mAccessibleLocations; }

    template<typename TDataType> 
    void AlltoAll( std::vector<TDataType> & r_send_datas, std::vector<TDataType> & r_recv_datas, int mpi_tag )
    {
      typedef std::vector<TDataType>::iterator       DataIteratorType;
      typedef std::vector<TDataType>::const_iterator DataConstIteratorType;
      typedef std::vector<Serializer>::iterator       SerializerIteratorType;
      typedef std::vector<Serializer>::const_iterator SerializerConstIteratorType;

      int mpi_rank, mpi_size;

      MPI_Comm_Rank(MPI_COMM_WORLD, &mpi_rank);
      MPI_Comm_Size(MPI_COMM_WORLD, &mpi_size);

      int* send_size = new int[mpi_size];
      int* recv_size = new int[mpi_size];

      // save data to buffer
      for( int i = 0,
           DataConstIterator      it_send_data       = r_send_datas.begin(), 
           SerializerIteratorType it_send_serializer = mSendSerializers.begin();
           i < mpi_size,
           it_send_data       != r_send_datas.end(),
           it_send_serializer != mSendSerializers.end();
           i++,
           it_send_data       = std::next(it_send_data),
           it_send_serializer = std::next(it_send_serializer); )
      {
        const Serializer & r_send_serializer = *it_send_serializer;

        r_send_serializer.ClearBuffer();

        r_send_serializer.Save(*it_send_data);

        if ( r_send_serializer.SaveIsTrivial() )
          send_size[i] = 0;
        else
          send_size[i] = (int) r_send_serializer.BufferSaveSize();
      }

      // receive message size
      MPI_Alltoall( send_size, 1, MPI_INT, recv_size, 1, MPI_INT, mComm );

      // resize recv serializer
      for( int i = 0; i < mpi_size; i++ )
      {
        if( recv_size[i] > 0 ) 
          mRecvSerializer[i].ResizeBiggerBuffer(recv_size[i]);
      }

      int num_event = 0;

      for ( int = i; i < mpi_size; i++ )
      {
        if( send_size[i] > 0 ) num_event++;
        if( recv_size[i] > 0 ) num_event++;
      }

      MPI_Request * reqs = new MPI_Request [num_event];
      MPI_Status * stats = new MPI_Status  [num_event];

      // send receive buffer
      for( int k = 0,
           int i = 0,
           SerializerConstIteratorType it_send_serializer = mSendSerializers.begin(),
           SerializerIteratorType      it_recv_serializer = mRecvSerializers.begin();
           i < mpi_size,
           it_send_serializer != mSendSerializer.end(),
           it_recv_serializer != mRecvSerializer.end();
           i++,
           it_send_serializer = std::next(it_send_serializer),
           it_recv_serializer = std::next(it_recv_serializer) )
      {
        if( send_size[i] > 0 )
        {
          MPI_Isend( (*it_send_serializer).BufferPointer(), send_size[i], MPI_CHAR, i, 0, mComm, reqs[k] ); 
          k++;
        }

        if( recv_size[i] > 0 )
        {
          MPI_Irecv( (*it_recv_serializer).BufferPointer(), recv_size[i], MPI_CHAR, i, 0, mComm, reqs[k] ); 
          k++;
        }
      }

      // wait until all send recv are done
      MPI_Waitall( num_event, reqs, status );

      // load data from buffer
      for( DataIterator           it_recv_data       = r_recv_datas.begin(), 
           SerializerIteratorType it_recv_serializer = mRecvSerializers.begin();
           it_recv_data       != r_recv_datas.end(),
           it_recv_serializer != mRecvSerializers.end();
           it_recv_data       = std::next(it_recv_data),
           it_recv_serializer = std::next(it_recv_serializer); )
      {
        *it_recv_serializer.Load(*it_recv_data);
      }

      delete [] send_size;
      delete [] recv_size;
      delete [] MPI_Request;
      delete [] MPI_Status;
    }

    template<typename TDataType>
    void AllGather( TDataType & r_data, std::vector<TDataType> & r_datas )
    {
      typedef std::vector<Serializer>::iterator       SerializerIteratorType;
      typedef std::vector<Serializer>::const_iterator SerializerConstIteratorType;

      int mpi_rank, mpi_size;

      MPI_Comm_Rank(MPI_COMM_WORLD, &mpi_rank);
      MPI_Comm_Size(MPI_COMM_WORLD, &mpi_size);

      // save data to buffer
      Serializer & r_send_serializer = mSendSerializer[mpi_rank];
      r_send_serializer.Save[r_data];
      int send_size = (int) r_send_serializer.BufferSaveSize();







      




      


    }


  private:
    MPI_Comm mComm;

    class MpiLocation
    {
      public:
        MpiLocation( MPI_Comm comm, int rank, int size ) 
          : mComm{comm},
            mRank{rank},
            mSize{size}
        { }
    
        struct LessThan
        {
          bool operator() ( const MpiLocation & a, const MpiLocation & b ) const
          { return ( a.mRank < b.mRank ); }
        }

        MPI_Comm Comm() const
        { return mComm; }

        int Rank() const
        { return mRank; }

        int Size() const
        { return mSize; }

      private:
        MPI_Comm mComm;
        int mRank;
        int mSize;
    };

    LocationContainerType mAllLocations;
    LocationContainerType mAccessibleLocations;

    std::vector<Serializer> mSendSerializers;
    std::vector<Serializer> mRecvSerializers;
};
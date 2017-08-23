#pragma once

#include<iostream>
#include<vector>
#include<mpi.h>
#include"Serializer.h"
#include"DataProfile.h"

class MpiLocation
{
  public:
    MpiLocation( MPI_Comm comm, int rank, int size )
      : mComm{comm},
        mRank{rank},
        mSize{size}
    {}

    MPI_Comm Comm() const
    { return mComm; }

    int Rank() const
    { return mRank; }

    int Size() const
    { return mSize; }

    struct LessThan
    {
      bool operator() ( const MpiLocation & a, const MpiLocation & b ) const
      {
        if( a.mComm != b.mComm )
        {
          std::cout << __func__ << "two MpiLocation not in the same not in the same MPI communicator! exit" << std::endl;
          exit(EXIT_FAILURE);
        }
        return ( a.mRank < b.mRank );
      }
    };

  private:
    MPI_Comm mComm;
    int mRank;
    int mSize;
};

class MpiCommunicator
{
  public:
    typedef MpiLocation LocationType;
    typedef MpiLocation::LessThan LocationLessThanType;
    typedef std::vector<LocationType> LocationContainerType;

    MpiCommunicator()
      : mComm{MPI_COMM_WORLD}
    {}

    MpiCommunicator(const MPI_Comm comm)
      : mComm{comm}
    {}

    ~MpiCommunicator()
    {}

    void BuildCompleteCommunication()
    {
      int size;
      MPI_Comm_size(mComm, &size);

      mSendSerializers.clear();
      mRecvSerializers.clear();

      mSendSerializers.resize(size);
      mRecvSerializers.resize(size);
    }

    LocationType Here()
    {
      int rank, size;

      MPI_Comm_rank(mComm, &rank);
      MPI_Comm_size(mComm, &size);

      LocationType mpi_location( mComm, rank, size );

      return mpi_location;
    }

    template<typename TDataType>
    void AllSendAllRecv( std::vector<TDataType> & r_send_datas, std::vector<TDataType> & r_recv_datas, int mpi_tag )
    {
      typedef typename std::vector<TDataType>::iterator       DataIteratorType;
      typedef typename std::vector<TDataType>::const_iterator DataConstIteratorType;
      typedef typename std::vector<Serializer>::iterator       SerializerIteratorType;
      typedef typename std::vector<Serializer>::const_iterator SerializerConstIteratorType;

      int mpi_rank, mpi_size;

      MPI_Comm_rank(mComm, &mpi_rank);
      MPI_Comm_size(mComm, &mpi_size);

      int* send_sizes = new int[mpi_size];

      //profile and save send_data
      {
        int i;
        DataConstIteratorType  it_send_data;
        SerializerIteratorType it_send_serializer;

        for(i = 0,
            it_send_data       = r_send_datas.begin(),
            it_send_serializer = mSendSerializers.begin();
            i < mpi_size ||
            it_send_data       != r_send_datas.end() ||
            it_send_serializer != mSendSerializers.end();
            i++,
            it_send_data       = std::next(it_send_data),
            it_send_serializer = std::next(it_send_serializer) )
        {
          Serializer & r_send_serializer = *it_send_serializer;
          const TDataType & r_send_data = *it_send_data;

          //profile data and add sender mark
          r_send_serializer.ReserveSpaceForBufferHeader(DataProfile::Default());
          DataProfile send_data_profile = DataProfile::Default().Profile(r_send_data).MakeFromSender();
          r_send_serializer.WriteBufferHeader(send_data_profile);

          send_sizes[i] = r_send_serializer.FreshSave(r_send_data);

          if ( send_data_profile.IsTrivial() )
            send_sizes[i] = 0;
        }
      }

      //receive message size
      int* recv_sizes = new int[mpi_size];

      MPI_Alltoall( send_sizes, 1, MPI_INT, recv_sizes, 1, MPI_INT, mComm );

      //write dummy header in recv buffer
      for( Serializer & r_recv_serializer : mRecvSerializers )
      {
        r_recv_serializer.ReserveSpaceForBufferHeader(DataProfile::Default());
        r_recv_serializer.WriteBufferHeader( DataProfile::Default().MakeNotFromSender().MakeTrivial() );
      }

      // resize recv serializer
      for( int i = 0; i < mpi_size; i++ )
      {
        if( recv_sizes[i] > 0 )
          mRecvSerializers[i].IncreaseBufferSize(recv_sizes[i]);
      }

      int num_event = 0;

      for ( int i = 0; i < mpi_size; i++ )
      {
        if( send_sizes[i] > 0 ) num_event++;
        if( recv_sizes[i] > 0 ) num_event++;
      }

      MPI_Request * reqs = new MPI_Request [num_event];
      MPI_Status * stats = new MPI_Status  [num_event];

      // send receive buffer
      {
        int k, i;
        SerializerConstIteratorType it_send_serializer;
        SerializerIteratorType      it_recv_serializer;

        for(k = 0,
            i = 0,
            it_send_serializer = mSendSerializers.begin(),
            it_recv_serializer = mRecvSerializers.begin();
            i < mpi_size ||
            it_send_serializer != mSendSerializers.end() ||
            it_recv_serializer != mRecvSerializers.end();
            i++,
            it_send_serializer = std::next(it_send_serializer),
            it_recv_serializer = std::next(it_recv_serializer) )
        {
          if( send_sizes[i] > 0 )
          {
            MPI_Isend( (*it_send_serializer).BufferPointer(), send_sizes[i], MPI_CHAR, i, mpi_tag, mComm, &(reqs[k]) );
            k++;
          }

          if( recv_sizes[i] > 0 )
          {
            MPI_Irecv( (*it_recv_serializer).BufferPointer(), recv_sizes[i], MPI_CHAR, i, mpi_tag, mComm, &(reqs[k]) );
            k++;
          }
        }
      }

      // wait until all send recv are done
      MPI_Waitall( num_event, reqs, stats );

      // load data from buffer
      {
        int i;
        DataIteratorType       it_recv_data;
        SerializerIteratorType it_recv_serializer;

        for(i = 0,
            it_recv_data       = r_recv_datas.begin(),
            it_recv_serializer = mRecvSerializers.begin();
            i < mpi_size ||
            it_recv_data       != r_recv_datas.end() ||
            it_recv_serializer != mRecvSerializers.end();
            i++,
            it_recv_data       = std::next(it_recv_data),
            it_recv_serializer = std::next(it_recv_serializer) )
        {
          TDataType & r_recv_data = *it_recv_data;
          Serializer & r_recv_serializer = *it_recv_serializer;

          if( recv_sizes[i] > 0 )
          {
            //check
            DataProfile recv_data_profile;
            r_recv_serializer.ReadBufferHeader(recv_data_profile);

            if( ! recv_data_profile.IsFromSender() )
            {
              std::cout << __func__ << "recv_sizes > 0, but buffer header is not marked by sender! exit" << std::endl;
              exit(EXIT_FAILURE);
            }

            if( recv_data_profile.IsTrivial() )
            {
              std::cout << __func__ << "recv_sizes > 0, but buffer header is marked trivial! exit" << std::endl;
              exit(EXIT_FAILURE);
            }

            r_recv_serializer.FreshLoad(r_recv_data);
          }
          else
          {
            r_recv_data.clear();
          }
        }
      }

      delete [] send_sizes;
      delete [] recv_sizes;
      delete [] reqs;
      delete [] stats;
    }

    template<typename TDataType>
    void AllGather( TDataType & r_send_data, std::vector<TDataType> & r_recv_datas, int mpi_tag )
    {
      typedef typename std::vector<TDataType>::iterator       DataIteratorType;
      typedef typename std::vector<Serializer>::iterator       SerializerIteratorType;
      typedef typename std::vector<Serializer>::const_iterator SerializerConstIteratorType;

      int mpi_rank, mpi_size;

      MPI_Comm_rank(mComm, &mpi_rank);
      MPI_Comm_size(mComm, &mpi_size);

      int* send_sizes = new int[mpi_size];

      //write send buffer header
      mSendSerializers[0].ReserveSpaceForBufferHeader(DataProfile::Default());
      DataProfile send_data_profile = DataProfile::Default().Profile(r_send_data).MakeFromSender();
      mSendSerializers[0].WriteBufferHeader(send_data_profile);

      // save data to buffer
      send_sizes[0] = (int) mSendSerializers[0].FreshSave(r_send_data);

      if( send_data_profile.IsTrivial() )
        send_sizes[0] = 0;

      for( int i = 1; i < mpi_size; i++ )
      {
        send_sizes[i] = send_sizes[0];
        mSendSerializers[i].CopyBufferContent(mSendSerializers[0]);
      }

      //receive message size
      int* recv_sizes = new int[mpi_size];

      MPI_Alltoall( send_sizes, 1, MPI_INT, recv_sizes, 1, MPI_INT, mComm );

      //write dummy header in recv buffer
      for( Serializer & r_recv_serializer : mRecvSerializers )
      {
        r_recv_serializer.ReserveSpaceForBufferHeader(DataProfile::Default());
        r_recv_serializer.WriteBufferHeader( DataProfile::Default().MakeTrivial().MakeNotFromSender() );
      }

      // resize recv serializer
      for( int i = 0; i < mpi_size; i++ )
      {
        if( recv_sizes[i] > 0 )
          mRecvSerializers[i].IncreaseBufferSize(recv_sizes[i]);
      }

      int num_event = 0;

      for ( int i = 0; i < mpi_size; i++ )
      {
        if( send_sizes[i] > 0 ) num_event++;
        if( recv_sizes[i] > 0 ) num_event++;
      }

      MPI_Request * reqs = new MPI_Request [num_event];
      MPI_Status * stats = new MPI_Status  [num_event];

      // send receive buffer
      {
        int k, i;
        SerializerConstIteratorType it_send_serializer;
        SerializerIteratorType      it_recv_serializer;

        for(k = 0,
            i = 0,
            it_send_serializer = mSendSerializers.begin(),
            it_recv_serializer = mRecvSerializers.begin();
            i < mpi_size ||
            it_send_serializer != mSendSerializers.end() ||
            it_recv_serializer != mRecvSerializers.end();
            i++,
            it_send_serializer = std::next(it_send_serializer),
            it_recv_serializer = std::next(it_recv_serializer) )
        {
          if( send_sizes[i] > 0 )
          {
            MPI_Isend( (*it_send_serializer).BufferPointer(), send_sizes[i], MPI_CHAR, i, mpi_tag, mComm, &(reqs[k]) );
            k++;
          }

          if( recv_sizes[i] > 0 )
          {
            MPI_Irecv( (*it_recv_serializer).BufferPointer(), recv_sizes[i], MPI_CHAR, i, mpi_tag, mComm, &(reqs[k]) );
            k++;
          }
        }
      }

      // wait until all send recv are done
      MPI_Waitall( num_event, reqs, stats );

      // load data from buffer
      {
        int i;
        DataIteratorType it_recv_data;
        SerializerIteratorType it_recv_serializer;

        for(i = 0,
            it_recv_data       = r_recv_datas.begin(),
            it_recv_serializer = mRecvSerializers.begin();
            i < mpi_size ||
            it_recv_data       != r_recv_datas.end() ||
            it_recv_serializer != mRecvSerializers.end();
            i++,
            it_recv_data       = std::next(it_recv_data),
            it_recv_serializer = std::next(it_recv_serializer) )
        {
          TDataType & r_recv_data = *it_recv_data;
          Serializer & r_recv_serializer = *it_recv_serializer;

          if( recv_sizes[i] > 0 )
          {
            //check
            DataProfile recv_data_profile;
            r_recv_serializer.ReadBufferHeader(recv_data_profile);

            if( ! recv_data_profile.IsFromSender() )
            {
              std::cout << __func__ << "recv_sizes > 0, but buffer header is not marked by sender! exit" << std::endl;
              exit(EXIT_FAILURE);
            }

            if( recv_data_profile.IsTrivial() )
            {
              std::cout << __func__ << "recv_sizes > 0, but buffer header is marked trivial! exit" << std::endl;
              exit(EXIT_FAILURE);
            }

            r_recv_serializer.FreshLoad(r_recv_data);
          }
          // clear recv data if there is no send
          else
          {
            r_recv_data.clear();
          }
        }
      }

      delete [] send_sizes;
      delete [] recv_sizes;
      delete [] reqs;
      delete [] stats;
    }

  private:
    MPI_Comm mComm;
    std::vector<Serializer> mSendSerializers;
    std::vector<Serializer> mRecvSerializers;
};

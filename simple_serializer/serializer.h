template<typename TBufferHeaderType>
class Serializer
{
  public:
    Serializer()
      : mpBuffer{nullptr},
        mBufferSize{0},
        mBufferHead{0},
        mBufferSavePos{0},
        mBufferLoadPos{0},
    {
      //reserve space for buffer header
      TBufferHeaderType buffer_header();
      std::size_t buffer_header_size = sizeof(buffer_header);
      IncreaseBufferSize(buffer_header_size);

      //
      mBufferHead = buffer_header_size;
      ResetBufferLoadPos();
      ResetBufferSavePos();
    }

    Serializer(std::size_t buffer_size)
      : Serializer()
    {
      IncreaseBufferSize(buffer_size);
    }

    ~Serializer()
    { delete [] mpBuffer; }

    void IncreaseBufferSize( const std::size_t buffer_size )
    {
      if( mpBuffer == nullptr )
      {
        mpBuffer = new char[buffer_size];
        mBufferSize = buffer_size;
        mBufferHead = 0;
        mBufferSavePos = 0;
        mBufferLoadPos = 0;
      }
      else if( mpBuffer < buffer_size )
      {
        char *p = new char[buffer_size];
        std::memcpy( p, mpBuffer, mBufferSize );
        delete[] mpBuffer;
        mpBuffer = p;
        mBufferSize = buffer_size;
      }
      else
        std::cout << __func__ << "Buffer size is bigger than requested size. Do nothing" << std::endl;
    }

    char* const BufferPointer() const
    { return mpBuffer; }

    const TBufferHeaderType ReadBufferHeader() const
    {
      TBufferHeaderType buffer_header;

      if( mBufferSize < sizeof( buffer_header ) )
      {
        std::cout << __func__ << "wrong: buffer size smaller than buffer header size! exit" << std::endl;
        exit(EXIT_FAILURE);
      }
      const TBufferHeaderType* const p = (TBufferHeaderType*) mpBuffer;
      return *p;
    }

    void WriteBufferHeader(const TBufferHeaderType & buffer_header) const
    {
      if( mBufferSize < sizeof(buffer_header) )
      {
        std::cout << __func__ << "wrong: buffer size smaller than buffer header size! exit" << std::endl;
        exit(EXIT_FAILURE);
      }
      TBufferHeaderType* p = (TBufferHeaderType*) mpBuffer;
      *p = buffer_header;
    }

    template<typename TDataType>
    int FreshSave( const TDataType & r_data )
    {
      ResetBufferSavePos();
      Save<TDataType> (r_data);
      return BufferSavePos();
    }

    template<typename TDataType>
    void FreshLoad( TDataType & r_data )
    {
      ResetBufferLoadPos();
      Load<TDataType> (r_data);
    }

    //save basic datatype
    template<typename TDataType,
             typename TDummyType>
    void Save( const TDataType & r_data )
    {
      std::size_t size = sizeof(r_data);

      if( mBufferSavePos + size > mBufferSize )
        IncreaseBufferSize(1.25*(mBufferSavePos+ size));

      TDataType* p = (TDataType*) (mpBuffer + mBufferSavePos);

      *p = r_data;

      mBufferSavePos += size;
    }

    //load basic datatype
    template<typename TDataType,
             typename TDummyType>
    void Load( TDataType & r_data )
    {
      std::size_t size = sizeof(r_data);

      if( mBufferLoadPos + size > mBufferSize )
      {
        std::cout << __func__ << "wrong: load position larger than buffer size! exit" << std::endl;
        exit(EXIT_FAILURE);
      }

      TDataType* p = (TDataType*) (mpBuffer + mBufferLoadPos);

      r_data = *p;

      mBufferLoadPos += size;
    }

    //save std::vector
    template<typename TDataType>
    void Save( const std::vector<TDataType> & r_vector )
    {
      typedef typename std::vector<TDataType>::size_type SizeType;

      SizeType vector_size = r_vector.size();

      Save(vector_size);

      for( TDataType data : r_vector )
        Save(data);
    }

    //load std::vector
    template<typename TDataType>
    void Load( std::vector<TDataType> & r_vector )
    {
      typedef typename std::vector<TDataType>::size_type SizeType;

      r_vector.clear();

      SizeType vector_size;

      Load(vector_size);

      r_vector.resize(vector_size);

      for( TDataType data : r_vector )
        Load(data);
    }

    //save user data type
    template<typename TDataType>
    void Save( const TDataType & r_data )
    {
      r_data.Save(*this);
    }

    //load user data type
    template<typename TDataType>
    void Load( TDataType & r_data )
    {
      r_data.Load(*this);
    }

  private:
    void ResetBufferSavePos()
    { mBufferSavePos = mBufferSaveHead; }

    void ResetBufferLoadPos()
    { mBufferLoadPos = mBufferSaveHead; }

    std::size_t BufferSavePos() const
    { return mBufferSavePos; }

    char* mpBuffer;

    std::size_t mBufferSize;
    std::size_t mBufferHead;
    std::size_t mBufferSavePos;
    std::size_t mBufferLoadPos;
};

template Serializer::Save<bool,int> ( const bool &);
template Serializer::Load<bool,int> (       bool &);

template Serializer::Save<char, int> ( const char &);
template Serializer::Load<char, int> (       char &);

template Serializer::Save<int, int> ( const int &);
template Serializer::Load<int, int> (       int &);

template Serializer::Save<long, int> ( const long &);
template Serializer::Load<long, int> (       long &);

template Serializer::Save<unsigned int,  int> ( const unsigned int  &);
template Serializer::Load<unsigned int,  int> (       unsigned int  &);

template Serializer::Save<unsigned long, int> ( const unsigned long &);
template Serializer::Load<unsigned long, int> ( const unsigned long &);

template Serializer::Save<double, int> ( const double &);
template Serializer::Load<double, int> ( const double &);

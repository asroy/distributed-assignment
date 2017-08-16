class Serializer
{
  public:
    Serializer()
      : mpBuffer{nullptr},
        mBufferSize{0}, 
        mBufferSaveHead{0}, 
        mBufferLoadHead{0},
    {}

    ~Serializer()
    { delete [] mpBuffer; }


    void PrepareSave()
    {
      CleanBuffer();

      MetaData meta_data();



      Save(mMetaData);

      
      Save()
    }

    void FinishSave()
    {
      SaveMetaData();
    }

    char* const BufferPointer() const
    { return mpBuffer; }

    std::size_t BufferSaveSize() const
    { return mBufferSaveHead; }

    bool BufferIsTrivial() const;

    void BufferSaveMetaInfo();

    //save basic type
    template<typename TDataType,
             typename TDummyType>
    void Save( const TDataType & r_data, bool & i_am_trivial )
    {                                                             
                                                                  
      std::size_t size = sizeof(r_data);                          
                                                                  
      if( mBufferSaveHead + size > mBufferSize )                  
        ResizeBiggerBuffer(1.25*(mBufferSaveHead+ size));         
                                                                  
      TDataType* p = (TDataType*) (mpBuffer + mBufferSaveHead);   
                                                                  
      *p = r_data;                                                
                                                                  
      mBufferSaveHead += size;                                    

      r_i_am_trivial = false;                                       
    }

    //load basic type
    template<typename TDataType,
             typename TDummyType>
    void Load( TDataType & r_data )
    {                                                             
      std::size_t size = sizeof(r_data);                          
                                                                  
      if( mBufferLoadHead + size > mBufferSize )                  
      {                                                           
        std::cout<<"wrong: " <<__func__<<std::endl;               
        exit(EXIT_FAILURE);                                       
      }                                                           
                                                                  
      TDataType* p = (TDataType*) (mpBuffer + mBufferLoadHead);   
                                                                  
      r_data = *p;                                                
                                                                  
      mBufferLoadHead += size;                                    
    }

    //load (trivial) basic type
    template<typename TDataType,
             typename TDummyType>
    void LoadTrivial( TDataType & r_data )
    {                                                             
      TDataType initialized_data();
      r_data = initialized_data;
    }

    //save std::vector
    template<typename TDataType>
    void Save( const std::vector<TDataType> & r_vector, bool & r_i_am_trivial )
    {
      typedef std::vector<TDataType> VectorType;
      typedef typename VectorType::const_iterator ConstIteratorType;
      typedef typename VectorType::size_type SizeType;

      SizeType size = r_vector.size();

      Save(size);

      for( ConstIteratorType it = r_vector.begin(); it != r_vector.end(); it = std::next(it) )
        Save(*it);

      if( size == 0 ) r_i_am_trivial = true;
    }

    //load std::vector
    template<typename TDataType>
    void Load( std::vector<TDataType> & r_vector )
    {
      typedef std::vector<TDataType> VectorType;
      typedef typename VectorType::iterator IteratorType;
      typedef typename VectorType::size_type SizeType;

      r_vector.clear();

      SizeType size;

      Load(size);

      r_vector.resize(size);

      for( IteratorType it = r_vector.begin(); it != r_vector.end(); it = std::next(it) )
        Load(*it);
    }

    //load (trivial) std::vector
    template<typename TDataType>
    void LoadTrivial( std::vector<TDataType> & r_vector )
    {
      r_vector.clear();
    }

    //save user data type 
    template<typename TDataType>
    void Save( const TDataType & r_data, bool & r_i_am_trivial )
    {
      r_data.Save(*this, r_i_am_trivial);
    }

    //load user data type
    template<typename TDataType>
    void Load( TDataType & r_data )
    {
      r_data.Load(*this);
    }

    //load (trivial) user data type
    template<typename TDataType>
    void LoadTrivial( TDataType & r_data )
    {
      r_data.LoadTrivial(*this);
    }

  private:
    class MetaData
    {
      bool mBufferIsTrivial;
    };

    void ReallocateBuffer(std::size_t buffer_size)
    {
      DeleteBuffer();

      mpBuffer = new char[buffer_size];

      mBufferSize = buffer_size;
      mBufferSaveHead = 0;
      mBufferLoadHead = 0;
    }

    void ResizeBiggerBuffer(std::size_t buffer_size)
    {
      if( mpBuffer == nullptr )
        ReallocateBuffer(buffer_size);
      else if( mpBuffer < buffer_size  )
      {
        char *p = new char[buffer_size];
        mBufferSize = buffer_size;
        std::memcpy( p, mpBuffer, mBufferSize );
        mpBuffer = p;
      }
      else
        std::cout << __func__ << "Buffer size is bigger than requested size. Do nothing" << std::enl;
    }

    void DeleteBuffer()
    {
      if( mpBuffer != nullptr )
        delete[] mpBuffer;

      mBufferSize = 0;
      mBufferSaveHead = 0;
      mBufferLoadHead = 0;
    }

    void CleanBuffer()
    {
      mBufferSaveHead = 0;
      mBufferLoadHead = 0;
    }

    char* mpBuffer;

    std::size_t mBufferSize;
    std::size_t mBufferSaveHead;
    std::size_t mBufferLoadHead;
};


template Serializer::Save<bool,          int> ( const bool &,          bool & )
template Serializer::Save<char,          int> ( const char &,          bool & )
template Serializer::Save<int ,          int> ( const int  &,          bool & )
template Serializer::Save<long,          int> ( const long &,          bool & )
template Serializer::Save<unsigned int,  int> ( const unsigned int  &, bool & )
template Serializer::Save<unsigned long, int> ( const unsigned long &, bool & )
template Serializer::Save<double,        int> ( const double &,        bool & )

template Serializer::Load<bool,          int> ( const bool &,          bool & )
template Serializer::Load<char,          int> ( const char &,          bool & )
template Serializer::Load<int ,          int> ( const int  &,          bool & )
template Serializer::Load<long,          int> ( const long &,          bool & )
template Serializer::Load<unsigned int,  int> ( const unsigned int  &, bool & )
template Serializer::Load<unsigned long, int> ( const unsigned long &, bool & )
template Serializer::Load<double,        int> ( const double &,        bool & )

template Serializer::LoadTrivial<bool,          int> ( const bool &,          bool & )
template Serializer::LoadTrivial<char,          int> ( const char &,          bool & )
template Serializer::LoadTrivial<int ,          int> ( const int  &,          bool & )
template Serializer::LoadTrivial<long,          int> ( const long &,          bool & )
template Serializer::LoadTrivial<unsigned int,  int> ( const unsigned int  &, bool & )
template Serializer::LoadTrivial<unsigned long, int> ( const unsigned long &, bool & )
template Serializer::LoadTrivial<double,        int> ( const double &,        bool & )

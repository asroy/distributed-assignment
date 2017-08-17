class Serializer
{
  public:
    Serializer()
      : mpBuffer{nullptr},
        mBufferSize{0}, 
        mBufferSaveSize{0}, 
        mBufferLoadSize{0},
    {}

    Serializer(std::size_t buffer_size)
      : mpBuffer{nullptr},
        mBufferSize{0}, 
        mBufferSaveSize{0}, 
        mBufferLoadSize{0},
    {
      mpBuffer = new char[buffer_size];
      mBufferSize = buffer_size;
    }

    ~Serializer()
    { delete [] mpBuffer; }
    
    void IncreaseBufferSize(std::size_t buffer_size)
    {
      if( mpBuffer == nullptr )
        mpBuffer = new char[buffer_size];

        mBufferSize = buffer_size;
        mBufferSaveSize = 0;
        mBufferLoadSize = 0;
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

    //reset buffer, so it will be ready for following senario:
    //  receive, followed by fresh non-trivial load
    //  no receive, followed by fresh trivial load
    void ResetBufferForRecvAndLoad()
    {
      ResetBufferLoadSize();
      UpdateBufferMetaData( BufferMetaData::TrivialBufferMetaData() );
    }

    char* const BufferPointer() const
    { return mpBuffer; }

    template<typename TDataType>
    void FreshSave( const TDataType & r_data, std::size_t & r_buffer_save_size, bool & r_buffer_is_trivial)
    {
      ResetBufferSaveSize();

      //save default meta data
      BufferMetaData default_meta_data = BufferMetaData::DefaultBufferMetaData();
      bool dummy;
      SaveData<BufferMetaData, int> (default_meta_data, dummy);  //this is a direct save

      //save data
      r_buffer_is_trivial = true;
      SaveData<TDataType> (r_data, r_buffer_is_trivial);

      //update meta data
      if(r_buffer_is_trivial)
        UpdateBufferMetaData( BufferMetaData::TrivialBufferMetaData() );
      else
        UpdateBufferMetaData( BufferMetaData::NonTrivialBufferMetaData() );

      //
      r_buffer_save_size = BufferSaveSize();
    }

    template<typename TDataType>
    void FreshLoad( TDataType & r_data, bool & r_buffer_is_trivial )
    {
      ResetBufferLoadSize();

      //load meta data
      BufferMetaData meta_data;
      LoadData<BufferMetaData, int> (meta_data);
      r_buffer_is_trivial = meta_data.BufferIsTrivial();

      //load data
      if(r_buffer_is_trivial)
        LoadDataTrivial<TDataType> (r_data);
      else
        LoadData<TDataType> (r_data);
    }

    //direct save
    template<typename TDataType,
             typename TDummyType>
    void SaveData( const TDataType & r_data, bool & r_i_am_trivial )
    {                                                             
      std::size_t size = sizeof(r_data);                          
                                                                  
      if( mBufferSaveSize + size > mBufferSize )                  
        IncreaseBufferSize(1.25*(mBufferSaveSize+ size));         
                                                                  
      TDataType* p = (TDataType*) (mpBuffer + mBufferSaveSize);   
                                                                  
      *p = r_data;                                                
                                                                  
      mBufferSaveSize += size;                                    

      r_i_am_trivial = false;                                       
    }

    //direct load
    template<typename TDataType,
             typename TDummyType>
    void LoadData( TDataType & r_data )
    {                                                             
      std::size_t size = sizeof(r_data);                          
                                                                  
      if( mBufferLoadSize + size > mBufferSize )                  
      {                                                           
        std::cout<<"wrong: " <<__func__<<std::endl;               
        exit(EXIT_FAILURE);                                       
      }                                                           
                                                                  
      TDataType* p = (TDataType*) (mpBuffer + mBufferLoadSize);   
                                                                  
      r_data = *p;                                                
                                                                  
      mBufferLoadSize += size;                                    
    }

    //load (trivial) basic type
    template<typename TDataType,
             typename TDummyType>
    void LoadDataTrivial( TDataType & r_data )
    {                                                             
      TDataType initialized_data();
      r_data = initialized_data;
    }

    //save std::vector
    template<typename TDataType>
    void SaveData( const std::vector<TDataType> & r_vector, bool & r_i_am_trivial )
    {
      typedef std::vector<TDataType> VectorType;
      typedef typename VectorType::const_iterator ConstIteratorType;
      typedef typename VectorType::size_type SizeType;

      SizeType size = r_vector.size();

      SaveData(size);

      for( ConstIteratorType it = r_vector.begin(); it != r_vector.end(); it = std::next(it) )
        SaveData(*it);

      if( size == 0 ) r_i_am_trivial = true;
    }

    //load std::vector
    template<typename TDataType>
    void LoadData( std::vector<TDataType> & r_vector )
    {
      typedef std::vector<TDataType> VectorType;
      typedef typename VectorType::iterator IteratorType;
      typedef typename VectorType::size_type SizeType;

      r_vector.clear();

      SizeType size;

      LoadData(size);

      r_vector.resize(size);

      for( IteratorType it = r_vector.begin(); it != r_vector.end(); it = std::next(it) )
        LoadData(*it);
    }

    //load (trivial) std::vector
    template<typename TDataType>
    void LoadDataTrivial( std::vector<TDataType> & r_vector )
    {
      r_vector.clear();
    }

    //save user data type 
    template<typename TDataType>
    void SaveData( const TDataType & r_data, bool & r_i_am_trivial )
    {
      r_data.SaveData(*this, r_i_am_trivial);
    }

    //load user data type
    template<typename TDataType>
    void LoadData( TDataType & r_data )
    {
      r_data.LoadData(*this);
    }

    //load (trivial) user data type
    template<typename TDataType>
    void LoadDataTrivial( TDataType & r_data )
    {
      r_data.LoadDataTrivial(*this);
    }

  private:
    class BufferMetaData
    {
      public:
        BufferMetaData()
          : mBufferIsTrivial{false}
        {}

        ~BufferMetaData()
        {}

        static BufferMetaData DefaultBufferMetaData()
        { 
          BufferMetaData meta_data();
          meta_data.mBufferIsTrivial = true;
          return meta_data;
        }

        static BufferMetaData TrivialBufferMetaData()
        { 
          BufferMetaData meta_data();
          meta_data.mBufferIsTrivial = true;
          return meta_data;
        }

        static BufferMetaData NonTrivialBufferMetaData()
        { 
          BufferMetaData meta_data();
          meta_data.mBufferIsTrivial = false;
          return meta_data;
        }

        bool BufferIsTrivial()
        { return mBufferIsTrivial;}

      private:
        bool mBufferIsTrivial;
    };

    void ResetBufferSaveSize()
    { mBufferSaveSize = 0; }

    void ResetBufferLoadSize()
    { mBufferLoadSize = 0; }

    std::size_t BufferSaveSize() const
    { return mBufferSaveSize; }

    BufferMetaData ReadBufferMetaData() const
    {
      BufferMetaData* p = (BufferMetaData*) mpBuffer;
      return *p;
    }

    void UpdateBufferMetaData(const BufferMetaData meta_data)
    {
      BufferMetaData* p = (BufferMetaData*) mpBuffer;
      *p = meta_data;
    }

    char* mpBuffer;

    std::size_t mBufferSize;
    std::size_t mBufferSaveSize;
    std::size_t mBufferLoadSize;
};


template Serializer::Save<bool,          int> ( const bool &,          bool & );
template Serializer::Save<char,          int> ( const char &,          bool & );
template Serializer::Save<int ,          int> ( const int  &,          bool & );
template Serializer::Save<long,          int> ( const long &,          bool & );
template Serializer::Save<unsigned int,  int> ( const unsigned int  &, bool & );
template Serializer::Save<unsigned long, int> ( const unsigned long &, bool & );
template Serializer::Save<double,        int> ( const double &,        bool & );

template Serializer::Load<bool,          int> ( const bool &,          bool & );
template Serializer::Load<char,          int> ( const char &,          bool & );
template Serializer::Load<int ,          int> ( const int  &,          bool & );
template Serializer::Load<long,          int> ( const long &,          bool & );
template Serializer::Load<unsigned int,  int> ( const unsigned int  &, bool & );
template Serializer::Load<unsigned long, int> ( const unsigned long &, bool & );
template Serializer::Load<double,        int> ( const double &,        bool & );

template Serializer::LoadTrivial<bool,          int> ( const bool &,          bool & );
template Serializer::LoadTrivial<char,          int> ( const char &,          bool & );
template Serializer::LoadTrivial<int ,          int> ( const int  &,          bool & );
template Serializer::LoadTrivial<long,          int> ( const long &,          bool & );
template Serializer::LoadTrivial<unsigned int,  int> ( const unsigned int  &, bool & );
template Serializer::LoadTrivial<unsigned long, int> ( const unsigned long &, bool & );
template Serializer::LoadTrivial<double,        int> ( const double &,        bool & )

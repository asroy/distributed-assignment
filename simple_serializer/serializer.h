#include"my_data.h"

#define SERIALIZER_DIRECT_SAVE(TDataType)                         \
    void Save( const TDataType & r_data )                         \
    {                                                             \
      size_t size = sizeof(r_data);                               \
                                                                  \
      if( mBufferSaveSize + size > mBufferSize )                  \
      {                                                           \
        std::cout<<"wrong: " <<__func__<<std::endl;               \
        exit(EXIT_FAILURE);                                       \
      }                                                           \
                                                                  \
      TDataType* p = (TDataType*) (mpBuffer + mBufferSaveSize);   \
                                                                  \
      *p = r_data;                                                \
                                                                  \
      mBufferSaveSize += size;                                    \
    }

#define SERIALIZER_DIRECT_LOAD(TDataType)                         \
    void Load( TDataType & r_data )                               \
    {                                                             \
      size_t size = sizeof(r_data);                               \
                                                                  \
      if( mBufferLoadSize + size > mBufferSize )                  \
      {                                                           \
        std::cout<<"wrong: " <<__func__<<std::endl;               \
        exit(EXIT_FAILURE);                                       \
      }                                                           \
                                                                  \
      TDataType* p = (TDataType*) (mpBuffer + mBufferLoadSize);   \
                                                                  \
      r_data = *p;                                                \
                                                                  \
      mBufferLoadSize += size;                                    \
    }

class Serializer
{
  public:
    Serializer()
      : mpBuffer{nullptr},
        mBufferSize{0}, mBufferSaveSize{0}, mBufferLoadSize{0}
    {}

    ~Serializer()
    { delete [] mpBuffer; }

    void ClearBuffer()
    {
      mBufferSize = 0;
      mBufferSaveSize = 0;
      mBufferLoadSize = 0;
    }

    void ReallocateBuffer(size_t buffer_size)
    {
      if( mpBuffer != nullptr )
        delete[] mpBuffer;

      mpBuffer = new char[buffer_size];

      mBufferSize = buffer_size;
      mBufferSaveSize = 0;
      mBufferLoadSize = 0;
    }

    void DeleteBuffer()
    {
      if( mpBuffer != nullptr )
        delete[] mpBuffer;

      mBufferSize = 0;
      mBufferSaveSize = 0;
      mBufferLoadSize = 0;
    }

    char* const BufferPointer() const
    { return mpBuffer; }

    const size_t BufferSaveSize() const
    { return mBufferSaveSize; }

    //save basic type
    SERIALIZER_DIRECT_SAVE(bool)
    SERIALIZER_DIRECT_SAVE(char)
    SERIALIZER_DIRECT_SAVE(int)
    SERIALIZER_DIRECT_SAVE(long)
    SERIALIZER_DIRECT_SAVE(unsigned int)
    SERIALIZER_DIRECT_SAVE(unsigned long)
    SERIALIZER_DIRECT_SAVE(double)
    SERIALIZER_DIRECT_SAVE(A)

    //load basic type
    SERIALIZER_DIRECT_LOAD(bool)
    SERIALIZER_DIRECT_LOAD(char)
    SERIALIZER_DIRECT_LOAD(int)
    SERIALIZER_DIRECT_LOAD(long)
    SERIALIZER_DIRECT_LOAD(unsigned int)
    SERIALIZER_DIRECT_LOAD(unsigned long)
    SERIALIZER_DIRECT_LOAD(double)
    SERIALIZER_DIRECT_LOAD(A)

    //save std::vector
    template<typename TDataType>
    void Save( const std::vector<TDataType> & r_vector )
    {
      typedef std::vector<TDataType> VectorType;
      typedef typename VectorType::const_iterator ConstIteratorType;
      typedef typename VectorType::size_type SizeType;

      SizeType size = r_vector.size();

      Save(size);

      for( ConstIteratorType it = r_vector.begin(); it != r_vector.end(); it = std::next(it) )
      {
        Save(*it);
      }
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
      {
        Load(*it);
      }
    }

    //save customed data type 
    template<typename TDataType>
    void Save( const TDataType & r_data )
    {
      r_data.Save(*this);
    }

    //load customed data type
    template<typename TDataType>
    void Load( TDataType & r_data )
    {
      r_data.Load(*this);
    }

  private:
    char* mpBuffer;

    size_t mBufferSize;
    size_t mBufferSaveSize;
    size_t mBufferLoadSize;
};

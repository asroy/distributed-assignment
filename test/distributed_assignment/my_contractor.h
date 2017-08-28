#include<mpi.h>

struct A
{
public:
    A()
    :   mThisRank{-1},
        mNextRank{-1}
    {}

    A( const int this_rank, const int next_rank )
    :   mThisRank{this_rank},
        mNextRank{next_rank}
    {}

    ~A()
    {}

private:

    void Save( DataUtility::Serializer & r_serializer ) const
    {
        r_serializer.Save(mThisRank);
        r_serializer.Save(mNextRank);
    }

    void Load( DataUtility::Serializer & r_serializer )
    {
        r_serializer.Load(mThisRank);
        r_serializer.Load(mNextRank);
    }

    void Profile( DataUtility::DataProfile & r_profile ) const
    {
        r_profile.SetIsTrivial(false);
    }

    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{A: ";
        r_printer.Print(mThisRank);
        r_printer.Print(mNextRank);
        std::cout << "}";
    }

    int mThisRank;
    int mNextRank;

    friend class DataUtility::Serializer;
    friend class DataUtility::DataProfile;
    friend class DataUtility::DataPrinter;
};

template<typename TContractorKeyType>
class MyContractorA
{
public:
    MyContractorA()
    :   mSize{0},
        mRank{0}
    {
        int mpi_rank, mpi_size;
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        mSize = mpi_size;
        mRank = mpi_rank;
    }

    ~MyContractorA()
    {}

    void SetKey(const TContractorKeyType contrator_key)
    { mKey = contrator_key; }

    TContractorKeyType GetKey() const
    { return mKey; }

    void Execute( const int & r_step, A & r_next )
    {
        int next_rank = mRank + r_step;
        next_rank = next_rank - (next_rank/mSize)*mSize;

        // r_next = { mRank, next_rank};
        r_next = { mRank, mRank };
    }

    void Execute( std::vector<int> & r_inputs, std::vector<A> & r_outputs )
    {
        int size = (int) r_inputs.size();

        r_outputs.clear();
        r_outputs.reserve(size);

        for( int i = 0; i < size; i++ )
        {
            int & r_input = r_inputs[i];
            A & r_output = r_outputs[i];
            Execute( r_input, r_output );
        }
    }

private:
    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{MyContractorA: ";
        r_printer.Print(mSize);
        r_printer.Print(mRank);
        std::cout << "}";
    }

    TContractorKeyType mKey;
    int mSize;
    int mRank;

    friend class DataUtility::DataPrinter;
};
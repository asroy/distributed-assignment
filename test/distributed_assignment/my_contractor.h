#include<mpi.h>

struct A
{
    int mThisRank;
    int mNextRank;
};

class SomeOne
{
public:
    SomeOne()
    :   mSize{0},
        mRank{0}
    {
        int mpi_rank, mpi_size;
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        mSize = mpi_size;
        mRank = mpi_rank;
    }

    ~SomeOne()
    {}

    void Execute( int & r_step, A & r_next )
    {
        int next_rank = mRank + r_step;
        next_rank = next_rank - (next_rank/mSize)*mSize;

        r_next = { mRank, next_rank};
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
        std::cout << "{SomeOne: ";
        r_printer.Print(mSize);
        r_printer.Print(mRank);
        std::cout << "}";
    }

    int mSize;
    int mRank;

    friend class DataUtility::DataPrinter;
};
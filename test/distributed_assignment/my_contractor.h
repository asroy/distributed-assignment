#include<mpi.h>

struct A
{
    int mThisRank;
    int mNextRank;
}

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

    Execute( int & r_step, A & r_next )
    {
        int next_rank = mRank + r_step;
        next_rank = next_rank - (next_rank/mSize)*mSize;

        r_next = { mRank, next_rank};
    }

    Execute( std::vector<int> & r_inputs, std::vector<A> & r_outputs )
    {
        int size = (int) r_inputs.size();

        r_outputs.clear();
        r_outputs.reserve(size);

        for( int i = 0; i < size; i++ )
        {
            input = r_inputs[i];
            Execute( input, output );
            r_outputs[i] = output;
        }
    }

private:
    int mSize;
    int mRank;
}
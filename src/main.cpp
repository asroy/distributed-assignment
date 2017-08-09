#include<mpi.h>
#include<ContractorMpiResidency.h>
#include<Contractor.h>

class SomeBody : public Contractor <int,ContractorMpiResidency>
{
  typedef Contractor <int, ContractorMpiResidency> BaseType;
  
  SomeBody( const int & key, const ContractorMpiResidency & residency )
    : BaseType(key, residency) {}

  public:
    void operator( const int & input, int & output )
    {
      output = 100*mData + input;
    }

  private:
    int mData;
};

int main(int argc, char* argv[])
{
  int ierr;
  ierr = MPI_Int( &argc, &argv );

  int rank, size;
  MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_Size(MPI_COMM_WORLD, &size);

  typedef int KeyType;
  typedef int InputType;
  typedef int outputType;

//residency
  ContractorMpiResidency residency; 

//contractors
  SomeBody a_person( rank, residency );

//contractor container
  DistributedContractorContainer< SomeBody, KeyType, ContractorMpiResidency > a_group_of_people;

  a_group_of_people.AddLocalContractor(a_person);

  a_group_of_people.GenerateGlobalContractorsResidency();

//key/input/output container
  DistributedAssignmentDataContainer<KeyType>    keys;
  DistributedAssignmentDataContainer<InputType>  inputs;
  DistributedAssignmentDataContainer<OutputType> outputs;

//fill up key, input
  keys.AssignorData().clear();
  keys.AssingorData().push_back(rank);

  inputs.AssignorData().clear();
  inputs.AssignorData().push_back(1);

//communicator
  DistributedAssignmentDataCommunicator <ContractorMpiResidency> data_communicator; 
  data_communicator.BuildCommunicator( a_group_of_people.GlobalContractorsResidency(), keys.AssignorData() );

//keys, inputs build communication
  keys.BuildCommunication(data_communicator);
  inputs.BuildCommunication(data_communicator);

//execute tasks
  a_group_of_people(keys,inputs,outputs);
  
  MPI_Finalize();

  return 0;
}

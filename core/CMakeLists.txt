set(DistributedAssignment_User_Header
  data_utility/DataPrinter.h;
  data_utility/DataProfile.h;
  data_utility/Serializer.h;
  data_utility/TypeId.h;
  mpi_communicator/MpiCommunicator.h;
  mpi_communicator/MpiLocation.h;
  distributed_assignment/AssignmentData.h;
  distributed_assignment/DummyContractor.h;
  distributed_assignment/DistributedAssignmentManager.h;
  distributed_assignment/DistributedContractorManager.h;
  distributed_assignment/DistributedKey.h;
  distributed_assignment/DistributedKeyIssuer.h;)

install(FILES ${DistributedAssignment_User_Header} DESTINATION include)


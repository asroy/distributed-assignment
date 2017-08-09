template< typename TDataType, 
          typename TContractorResidencyType >
class DistributedAssignmentDataContainer
{
  public:
    typedef std::vector <TDataType> DataContainerType;
    typedef std::map < TContractorResidencyType, std::vector<TDataType> > BufferType;
    typedef DistributedAssignmentDataCommunicator <TContractorResidencyType> DistributedAssignmentDataCommunicatorType;

    BuildCommunication( DistributedAssignmentDataCommunicatorType & );

    DataContainerType & AssignorData()
    { return mAssignorData; }

    DataContainerType & AssigneeData()
    { return mAssingeeData; }

    void SendAssignorDataToAssigneeData();
    void SendAssigneeDataToAssignorData();

  private:
    TDataContainerType mAssignorData;
    TDataContainerType mAssigneeData;

    TBufferType mAssignorBuffer;
    TBufferType mAssigneeBuffer;

    DistributedAssignmentDataCommunicatorType* mpDistributedAssignmentDataCommunicator;
};

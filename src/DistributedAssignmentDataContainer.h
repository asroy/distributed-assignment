template< typename TDataType, 
          typename TContractorResidencyType >
class DistributedAssignmentDataContainer
{
  public:
    typedef std::vector <TDataType> DataContainerType;
    typedef std::map < TContractorResidencyType, std::vector<TDataType> > BufferType;
    typedef DistributedAssignmentDataCommunicator <TContractorResidencyType> DistributedAssignmentDataCommunicatorType;

    BuildCommunication( DistributedAssignmentDataCommunicatorType & );

    DataContainerType & AssignorData() { return mAssignorDatas; };
    DataContainerType & AssigneeData() { return mAssingeeDatas; };

    void SendAssignorDataToAssigneeData();
    void SendAssigneeDataToAssignorData();

  private:
    TDataContainerType mAssignorDatas;
    TDataContainerType mAssigneeDatas;

    TBufferType mAssignorBuffer;
    TBufferType mAssigneeBuffer;

    DistributedAssignmentDataCommunicatorType* mpDistributedAssignmentDataCommunicator;
};

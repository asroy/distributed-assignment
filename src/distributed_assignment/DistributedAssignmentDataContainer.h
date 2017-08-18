template< typename TDataType, 
          typename TContractorResidencyType >
class DistributedAssignmentDataContainer
{
  public:
    typedef std::map < TContractorResidencyType, std::vector<TDataType> > DataContainerType;
    typedef DistributedAssignmentDataCommunicator <TContractorResidencyType> CommunicatorType;

    DataContainerType & AssignorData()
    { return mAssignorData; }

    DataContainerType & AssigneeData()
    { return mAssingeeData; }

    void SendAssignorDataToAssigneeData();
    void SendAssigneeDataToAssignorData();

  private:
    DataContainerType mAssignorData;
    DataContainerType mAssigneeData;

    CommunicatorType* mpCommunicator;
};

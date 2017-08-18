template< typename TResidencyType>
class DistributedAssignmentDataCommunicator
{
  public:
    TResidencyType LocalResidency();


    template< typename TContractorKeyType,
              typename TContractorResidencyContainerType = std::map <TContractorKeyType, TContractorResidencyType> >
    Build( TContractorResidencyContainerType &, std::vector<TContractorKeyType> &);

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void SendAssignorDataToAssigneeData( TDataContainerType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void ReceiveAssigneeDatafromAssingorData( TDataContainerType &, TDataContainerType & );

  private:
    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void LoadAssignorBufferFromData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void LoadAssignorBufferToData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void LoadAssigneeBufferFromData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void LoadAssigneeBufferToData( TBufferType &, TDataContainerType & );

    typedef std::map < TContractorResidencyType, std::vector <int> > BufferToDataMapType;

    BufferToDataMapType mAssignorBufferToDataMap;
    BufferToDataMapType mAssigneeBufferToDataMap;
}

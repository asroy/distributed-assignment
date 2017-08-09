template< typename TContractorResidencyType>
class DistributedAssignmentDataCommunicator
{
  public:
    template< typename TContractorKeyType,
              typename TContractorResidencyContainerType = std::map <TContractorKeyType, TContractorResidencyType> >
    BuildCommunicator( TContractorResidencyContainerType &, std::vector<TContractorKeyType> &);

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void SendAssignorDataToAssigneeData( TDataContainerType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void SendAssigneeDataToAssingorData( TDataContainerType &, TDataContainerType & );

  private:
    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void SetAssignorBufferFromData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void PutAssignorBufferToData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void SetAssigneeBufferFromData( TBufferType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType>,
               typename TBufferType = std::map < TContractorResidencyType, std::vector<TDataType> > >
    void PutAssigneeBufferToData( TBufferType &, TDataContainerType & );

    typedef std::map < TContractorResidencyType, std::vector <int> > BufferToDataMapType;

    BufferToDataMapType mAssignorBufferToDataMap;
    BufferToDataMapType mAssigneeBufferToDataMap;
}

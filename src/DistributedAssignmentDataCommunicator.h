template< typename TContractorResidencyType>
class DistributedAssignmentDataCommunicator
{
  public:
    template< typename TContractorKeyType,
              typename TContractorResidencyContainerType = std::map <TContractorKeyType, TContractorResidencyType> >
    BuildCommunicator( TContractorResidencyContainerType &, std::vector<TContractorKeyType> &);

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void PutAssignorDataToAssigneeData( TDataContainerType &, TDataContainerType & );

    template < typename TDataType,
               typename TDataContainerType = std::vector<TDataType> >
    void PutAssigneeDataToAssingorData( TDataContainerType &, TDataContainerType & );

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

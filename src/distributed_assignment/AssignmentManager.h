template<typename TContractorKeyType, 
         typename TAssignmentKeyType,
         typename TLocationType>
class AssignmentManager
{
public:
    AssignmentManager()
    {}
    
    ~AssignmentManager()
    {}

    template<TDataType>
    TAssignmentKeyType CreateAssignment( TContractorKeyType assignor_key, 
        TContractorKeyType assignee_key, TDataType & r_data );

    void DistributeAssignments();

    void GenerateWorkUnits();

    void ProcessWorkUnit();

private:
    DistributedContractorContainer* mpDistributedContractors;
    std::map<TContractorKeyType,vector<TAssignmentKeyType,>>




}

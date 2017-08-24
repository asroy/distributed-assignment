template<typename TContractorKeyType,
         typename TAssignmentKeyType>
class Assignment
{
private:
    TContractorKeyType mAssignorKey;
    TAssignmentKeyType mAssignmentKey;
    TContractorKeyType mAssigneeKey;
}
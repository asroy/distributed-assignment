#pragma once

namespace DistributedAssignment
{

template<typename TContractorKeyType,
         typename TAssignmentKeyType
         typename TDataType>
struct AssignmentData
{
    TContractorKeyType mAssignorKey;
    TAssignmentKeyType mAssignmentKey;
    TContractorKeyType mAssigneeKey;
    TDataType mData;
};

}
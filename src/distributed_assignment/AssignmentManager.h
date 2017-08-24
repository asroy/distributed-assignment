#pragma once
#include<iostream>
#include"Contractor.h"
#include"DistributedContractorContainer.h"
#include"AssignmentData.h"

namespace DistributedAssignment{

template<typename TAssignorType,
         typename TAssigneeType,
         typename TContractorKeyType,
         typename TAssignmentKeyType,
         typename TCommunicatorType,
         typename TInputDataType,
         typename TOutputDataType>
class AssignmentManager
{
public:
    typedef typename TCommunicatorType::Location Location;

    template<TContractorType>
    using ContractorContainerType = DistributedContractorContainer<TContractorType,TContractorKeyType,TLocationType>;

    template<TDataType>
    using AssignmentDataType = AssignmentData<TContractorKeyType,TAssignmentKeyType,TDataType>;

    template<TKeyType, TDataType>
    using AssignmentDataContainerType = std::map<TKeyType, std::vector<AssignmentDataType<TDataType>>;

    AssignmentManager()
    :   mpCommunicator{nullptr},
        mpAssignors{nullptr},
        mpAssignees{nullptr}
    {}

    AssignmentManager( TCommunicatorType & r_communicator, AssignorContainer & r_assignors, AssigneeContainer & r_assignees  )
    :   mpCommunicator{& r_communicator},
        mpAssignors{& r_assignors},
        mpAssignees{& r_assignees}
    {}

    ~AssignmentManager()
    {
        mpCommunicator = nullptr;
        mpAssignors = nullptr;
        mpAssignees = nullptr;
    }

    void StartAddAssignment();

    void FinishAddAssignment();

    TAssignmentKeyType AddAssignment( TContractorKeyType assignor_key, TContractorKeyType assignee_key, TInputDataType input_data )
    {
        typedef std::map<TLocationType, std::vector<AssignmentInputData>>::iterator AssignmentInputContainer;

        TAssignmentKeyType assignment_key = ?????????????????
        AssignmentData assignment_data = { assignor_key, assignment_key, assignee_key, input_data };

        Location assignee_location = mpAssignees.ContactorLocation(assignee_key);

        if( assignee_location == Location::NoWhere() )
        {
            std::cout<<__func__<<": cannot find assignee! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        it = mAssignorInputDatas.find(assignee_location);

    }

    void DistributeAssignments( int mpi_tag = 0 )
    {
        mpCommunicator->AllSendAllRecv( mAssignorInputDatas, mAssigneeInputDatas, mpi_tag );
    }

    void GenerateWorkUnits()
    {
        for( )
    }

    void ProcessWorkUnit();

    TOutputDataType GetResult( TContractorKeyType assignor_key, TAssignmentKeyType assignment_key );

private:
    TCommunicatorType* mpCommunicator;

    ContractorContainerType<TAssignorType>* mpAssignors;
    ContractorContainerType<TAssigneeType>* mpAssignees;

    AssignmentDataContainerType<Location,           TInputData> mAssignorInputDatas;
    AssignmentDataContainerType<Location,           TInputData> mAssigneeInputDatas;
    AssignmentDataContainerType<TContractorKeyType, TInputData> mWorkUnitInputDatas;

    AssignmentDataContainerType<Location,           TOutputData> mAssignorOutputDatas;
    AssignmentDataContainerType<Location,           TOutputData> mAssigneeOutputDatas;
    AssignmentDataContainerType<TContractorKeyType, TOutputData> mWorkUnitOutputDatas;
};

}
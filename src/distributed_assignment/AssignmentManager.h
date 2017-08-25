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

    template<typename TContractorType>
    using ContractorContainerType = DistributedContractorContainer<TContractorType,TContractorKeyType,TLocationType>;

    template<typename TDataType>
    using AssignmentDataType = AssignmentData<TContractorKeyType,TAssignmentKeyType,TDataType>;

    template<typename TDataType>
    using AssignmentDataVectorType = std::vector<AssignmentDataType<TDataType>>;

    template<typename TKeyType, typename TDataType>
    using AssignmentDataVectorMapType = std::map<TKeyType, AssignmentDataVectorType<TDataType>>;

    typedef AssignmentDataVectorType<TInputDataType>  WorkUnitInputDataVector;
    typedef AssignmentDataVectorType<TOutputDataType> WorkUnitOutputDataVector;

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

    void StartAddAssignment()
    {
        mAssignorInputDatas.clear();
        mAssigneeInputDatas.clear();
    }

    void FinishAddAssignment()
    {}

    TAssignmentKeyType AddAssignment( TContractorKeyType assignor_key, TContractorKeyType assignee_key, TInputDataType input_data )
    {
        TAssignmentKeyType assignment_key = //generate a new assignment key ?????????????????

        AssignmentData assignment_data = { assignor_key, assignment_key, assignee_key, input_data };

        Location assignee_location = mpAssignees.ContactorLocation(assignee_key);

        if( assignee_location == Location::NoWhere() )
        {
            std::cout<<__func__<<": cannot find assignee! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        mAssignorInputDatas[assignee_location].push_back(assignment_data);

        return assignment_key;
    }

    void DistributeAssignmentsInput( int mpi_tag = 0 )
    {
        mpCommunicator->AllSendAllRecv( mAssignorInputDatas, mAssigneeInputDatas, mpi_tag );
    }

    void GenerateWorkUnitsInputDatas()//convert assignee input data to work unit input data
    {
        typedef AssignmentDataType<TInputDataType> Data;
        typedef std::vector<Data> DataVector;
        
        mWorkUnitInputDatas.clear();

        for( const std::pair<Location, DataVector> & r_assignee_data_vector_pair : mAssigneeInputDatas )
        {
            const DataVector & r_assignee_data_vector = r_assignee_data_vector_pair.second;
            for ( const Data & r_data : r_assignee_data_vector )
            {
                TContractorKeyType assignee_key = r_data.mAssigneeKey;
                mWorkUnitInputDatas[assignee_key].push_back(r_data);
            }
        }
    }

    void ProcessOneWorkUnit( const TContractorKeyType assignee_key, WorkUnitInputDataVector & r_work_unit_input_datas, WorkUniteOutputDataVector & r_work_unit_output_datas )
    {
        int assignments_size = (int) r_work_unit_input_datas.size();

        //clear work unit output datas
        r_work_unit_output_datas.clear();
        r_work_unit_output_datas.reserve(assignments_size);

        std::vector<TInputDataType>  inputs(assignments_size);
        std::vector<TOutputDataType> outputs(assignments_size);

        for ( int i == 0; i < assignments_size; i++ )
        {
            inputs[i] = r_work_unit_input_datas[i].mData;
        }

        //process
        TAssigneeType & r_assignee = mpAssignees->LocalContractor(assignee_key);
        r_assignee.ProcessDatas(inputs, outputs);

        for ( int i == 0; i < assignments_size; i++ )
        {
            r_work_unit_output_datas[i].mAssignorKey   = r_work_unit_input_datas[i].mAssignorKey
            r_work_unit_output_datas[i].mAssignmentKey = r_work_unit_input_datas[i].mAssignmentKey
            r_work_unit_output_datas[i].mAssigneeKey   = r_work_unit_input_datas[i].mAssigneeKey
            r_work_unit_output_datas[i].mData = outputs[i];
        }
    }

    void GenerateAssigneeOuputDatas()//convert assignee input data to work unit input data
    {
        typedef AssignmentDataType<TOutputDataType> Data;
        typedef std::vector<Data> DataVector;
        
        mAssigneeOutputDatas.clear();

        for( const std::pair<TContractorKeyType, DataVector> & r_work_unit_data_vector_pair : mWorkUnitOutputDatas )
        {
            const DataVector & r_work_unit_data_vector = r_work_unit_data_vector_pair.second;
            for ( const Data & r_data : r_work_unit_data_vector )
            {
                TContractorKeyType assignor_key = r_data.mAssignorKey;
                Location assignor_location = mpAssignor->ContractorLocation(assignor_key);
                mAssigneeOutputDatas[assignor_location].push_back(r_data);
            }
        }
    }

    void DistributeAssignmentsOutput( int mpi_tag = 0 )
    {
        mpCommunicator->AllSendAllRecv( mAssigneeOutputDatas, mAssignorOutputDatas );
    }

    TOutputDataType GetResult( TContractorKeyType assignor_key, TAssignmentKeyType assignment_key );

private:
    TCommunicatorType * mpCommunicator;

    ContractorContainerType<TAssignorType> * mpAssignors;
    ContractorContainerType<TAssigneeType> * mpAssignees;

    AssignmentDataVectorMapType<Location,           TInputDataType> mAssignorInputDatas;
    AssignmentDataVectorMapType<Location,           TInputDataType> mAssigneeInputDatas;
    AssignmentDataVectorMapType<TContractorKeyType, TInputDataType> mWorkUnitInputDatas;

    AssignmentDataVectorMapType<Location,           TOutputDataType> mAssignorOutputDatas;
    AssignmentDataVectorMapType<Location,           TOutputDataType> mAssigneeOutputDatas;
    AssignmentDataVectorMapType<TContractorKeyType, TOutputDataType> mWorkUnitOutputDatas;
};

}

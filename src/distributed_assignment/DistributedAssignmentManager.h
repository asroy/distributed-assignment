#pragma once
#include<iostream>
#include"Contractor.h"
#include"DistributedContractorManager.h"
#include"AssignmentData.h"

namespace DistributedAssignment{

template<typename TAssignorType,
         typename TAssigneeType,
         typename TInputDataType,
         typename TOutputDataType,
         typename TCommunicatorType,
         typename TContractorKeyType,
         typename TDistributedAssignmentKeyIssuerType>
class DistributedAssignmentManager
{
public:
    typedef typename TCommunicatorType::Location Location;
    typedef typename TDistributedAssignmentKeyIssuerType<TCommunicatorType> AssignmentKeyIssuer;
    typedef typename AssignmentKeyIssuer::Key AssignmentKey;

    template<typename TContractorType>
    using ContractorManagerType = DistributedContractorManager<TContractorType,TContractorKeyIssuerType,TCommunicatorType>;

    template<typename TDataType>
    using AssignmentDataType = AssignmentData<TContractorKeyType,AssignmentKey,TDataType>;

    template<typename TDataType>
    using AssignmentDataVectorType = std::vector<AssignmentDataType<TDataType>>;

    template<typename TKeyType, typename TDataType>
    using AssignmentDataVectorMapType = std::map<TKeyType, AssignmentDataVectorType<TDataType>, typename TKeyType::LessThanComparator>;

    typedef AssignmentDataVectorType<TInputDataType>  WorkUnitInputDataVector;
    typedef AssignmentDataVectorType<TOutputDataType> WorkUnitOutputDataVector;

    DistributedAssignmentManager() = delete();

    DistributedAssignmentManager
    (   TCommunicatorType & r_communicator,
        ContractorManagerType<TAssignorType> & r_assignor_manager,
        ContractorManagerType<TAssigneeType> & r_assignee_manager )
    :   mpCommunicator {& r_communicator},
        mpAssignorManager {& r_assignor_manager},
        mpAssigneeManager {& r_assignee_manager},
        mAssignmentKeyIssuer (r_communicator)
    {}

    ~DistributedAssignmentManager()
    {
        mpCommunicator = nullptr;
        mpAssignorManager = nullptr;
        mpAssigneeManager = nullptr;
    }

    void ClearAllAssignment()
    {
        mAssignmentKeyIssuer.Clear();

        mAssignorInputDatas.clear();
        mAssigneeInputDatas.clear();
        mWorkUnitInputDatas.clear();
        mAssignorOutputDatas.clear();
        mAssigneeOutputDatas.clear();
        mWorkUnitOutputDatas.clear();
    }

    AssignmentKey AddAssignment( TContractorKeyType assignor_key, TContractorKeyType assignee_key, TInputDataType input_data )
    {
        AssignmentKey assignment_key = mAssignmentKeyIssuer.IssueNewKey();

        AssignmentData assignment_data = { assignor_key, assignment_key, assignee_key, input_data };

        Location assignee_location = mpAssigneeManager.ContactorLocation(assignee_key);

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
        TAssigneeType & r_assignee = mpAssigneeManager->LocalContractor(assignee_key);
        r_assignee.Execute(inputs, outputs);

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

    TOutputDataType GetResult( TContractorKeyType assignor_key, AssignmentKey assignment_key );

private:
    TCommunicatorType * mpCommunicator;

    ContractorManagerType<TAssignorType> * mpAssignorManager;
    ContractorManagerType<TAssigneeType> * mpAssigneeManager;

    AssignmentKeyIssuer mAssignmentKeyIssuer;

    AssignmentDataVectorMapType<Location,           TInputDataType> mAssignorInputDatas;
    AssignmentDataVectorMapType<Location,           TInputDataType> mAssigneeInputDatas;
    AssignmentDataVectorMapType<TContractorKeyType, TInputDataType> mWorkUnitInputDatas;

    AssignmentDataVectorMapType<Location,           TOutputDataType> mAssignorOutputDatas;
    AssignmentDataVectorMapType<Location,           TOutputDataType> mAssigneeOutputDatas;
    AssignmentDataVectorMapType<TContractorKeyType, TOutputDataType> mWorkUnitOutputDatas;
};

}

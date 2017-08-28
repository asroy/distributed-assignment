#pragma once
#include<iostream>
#include"DistributedContractorManager.h"
#include"AssignmentData.h"

namespace DistributedAssignment{

template<typename TAssignorType,
         typename TAssigneeType,
         typename TInputType,
         typename TOutputType,
         typename TCommunicatorType,
         typename TDistributedContractorKeyIssuerType,
         typename TDistributedAssignmentKeyIssuerType>
class DistributedAssignmentManager
{
public:
    typedef typename TCommunicatorType::Location Location;
    typedef typename TDistributedContractorKeyIssuerType<TCommunicatorType> ContractorKeyIssuer;
    typedef typename ContractorKeyIssuer::Key ContractorKey;
    typedef typename TDistributedAssignmentKeyIssuerType<TCommunicatorType> AssignmentKeyIssuer;
    typedef typename AssignmentKeyIssuer::Key AssignmentKey;

    template<typename TContractorType>
    using ContractorManagerType = DistributedContractorManager<TContractorType,ContractorKeyIssuer,TCommunicatorType>;

    template<typename TDataType>
    using AssignmentDataType = AssignmentData<ContractorKey,AssignmentKey,TDataType>;

    template<typename TDataType>
    using AssignmentDataVectorType = std::vector<AssignmentDataType<TDataType>>;

    template<typename TKeyType, typename TDataType>
    using AssignmentDataVectorMapType = std::map<TKeyType, AssignmentDataVectorType<TDataType>, typename TKeyType::LessThanComparator>;

    template<typename TKeyType, typename TDataType>
    using AssignmentDataVectorPairType = std::pair<TKeyType,  AssignmentDataVectorType<TDataType>>;

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

        mAssignorInputs.clear();
        mAssigneeInputs.clear();
        mWorkUnitInputs.clear();
        mAssignorOutputs.clear();
        mAssigneeOutputs.clear();
        mWorkUnitOutputs.clear();
    }

    AssignmentKey AddAssignment( const TContractorKeyType assignor_key, const TContractorKeyType assignee_key, const TInputType input_data )
    {
        typedef AssignmentDataType<TInputType> AssignmentInput;

        AssignmentKey assignment_key = mAssignmentKeyIssuer.IssueNewKey();

        AssignmentInput assignment_input = { assignor_key, assignment_key, assignee_key, input_data };

        Location assignee_location = mpAssigneeManager.ContactorLocation(assignee_key);

        if( assignee_location == Location::NoWhere() )
        {
            std::cout<<__func__<<": cannot find assignee! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        mAssignorInputs[assignee_location].push_back(assignment_data);

        return assignment_key;
    }

    void ExecuteAllDistributedAssignments()
    {
        typedef AssignmentDataVectorType<TInputType> InputVector;
        typedef AssignmentDataVectorPairType<const ContractorKey, TInputType> InputVectorPairByContractorKey;

        SendAssignorInputsToAssigneeInputs(0);

        GenerateWorkUnitsInputs();

        for( const InputVectorPairByContractorKey & r_input_vector_pair : mWorkUnitInputs )
        {
            const ContractorKey assignee_key = r_input_vector_pair.first;
            ExecuteLocalWorkUnit(assignee_key);
        }

        SendAssigneeOutputsToAssignorOutputs(1);
    }

    void SendAssignorInputsToAssigneeInputs( const int mpi_tag = 0 )
    {
        mpCommunicator->AllSendAllRecv( mAssignorInputs, mAssigneeInputs, mpi_tag );
    }

    void GenerateWorkUnitsInputs()//convert assignee input data to work unit input data
    {
        typedef AssignmentDataType<TInputType> Input;
        typedef AssignmentDataVectorType<TInputType> InputVector;
        typedef AssignmentDataVectorPairType<const Location, TInputType> InputVectorPairByLocation;

        mWorkUnitInputs.clear();

        for( const InputVectorPairByLocation & r_assignee_input_vector_pair : mAssigneeInputs )
        {
            const InputVector & r_assignee_input_vector = r_assignee_input_vector_pair.second;
            for ( const Input & r_assignee_input : r_assignee_input_vector )
            {
                TContractorKeyType assignee_key = r_assignee_input.mAssigneeKey;
                mWorkUnitInputs[assignee_key].push_back(r_assignee_input);
            }
        }
    }

    void ExecuteLocalWorkUnit( const TContractorKeyType assignee_key )
    {
        typedef AssignmentDataVectorType<TInputType> InputVector;
        typedef AssignmentDataVectorType<TOuputDataType> OutputVector;

        typedef AssignmentDataVectorMapType<ContractorKey, TInputType>  InputVectorMap;
        typedef AssignmentDataVectorMapType<ContractorKey, TOutputType> OutputVectorMap;

        typedef typename InputVectorMap::const_iterator InputVectorMapIterator;

        InputVectorMapIterator it_work_unit_input_vector = mWorkUnitInputs.find(assignee_key);

        if( it_work_unit_input_vector == mWorkUnitInputs.end() )
        {
            std::cout<<__func__<<": work unit inputs not found! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        InputVector & r_work_unit_input_vector = *it_work_unit_input_vector;
        OutputVector & r_work_unit_output_vector = mWorkUnitOutputs[assignee_key];

        int num_assignment = (int) r_work_unit_input_vector.size();

        //clear work unit output datas
        r_work_unit_output_vector.clear();
        r_work_unit_output_vector.reserve(num_assignment);

        std::vector<TInputType>  inputs(num_assignment);
        std::vector<TOutputType> outputs(num_assignment);

        for ( int i = 0; i < num_assignment; i++ )
            inputs[i] = r_work_unit_input_vector[i].mData;

        //process
        TAssigneeType * p_assignee = mpAssigneeManager->FindLocalContractorPointer(assignee_key);
        if( p_assignee == nullptr ) then
        {
            std::cout <<__func__<<"local contractor not found! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        p_assignee->Execute(inputs, outputs);

        for ( int i == 0; i < num_assignment; i++ )
        {
            r_work_unit_output_vector[i].mAssignorKey   = r_work_unit_input_vector[i].mAssignorKey
            r_work_unit_output_vector[i].mAssignmentKey = r_work_unit_input_vector[i].mAssignmentKey
            r_work_unit_output_vector[i].mAssigneeKey   = r_work_unit_input_vector[i].mAssigneeKey
            r_work_unit_output_vector[i].mData = outputs[i];
        }
    }

    void GenerateAssigneeOuputs()//convert work unit output data to assignee output data
    {
        typedef AssignmentDataType<TOutputType> Output;
        typedef AssignmentDataVectorType<TOutputType> OutputVector;
        typedef AssignmentDataVectorPairType<const TContractorKeyType, TOutputType> OutputVectorPairByContractorKey;

        mAssigneeOutputs.clear();

        for( OutputVectorPairByContractorKey & r_work_unit_output_vector_pair : mWorkUnitOutputs )
        {
            const OutputVector & r_work_unit_output_vector = r_work_unit_output_vector_pair.second;
            for ( const Output & r_output : r_work_unit_output_vector )
            {
                TContractorKeyType assignor_key = r_output.mAssignorKey;
                Location assignor_location = mpAssignor->ContractorLocation(assignor_key);
                mAssigneeOutputs[assignor_location].push_back(r_output);
            }
        }
    }

    void SendAssigneeOutputsToAssignorOutputs( const int mpi_tag = 0 )
    {
        mpCommunicator->AllSendAllRecv( mAssigneeOutputs, mAssignorOutputs, mpi_tag );
    }

    void GetResults( std::vector<AssignmentDataType<TOutputType>> & r_result_vector ) const
    {
        typedef AssingmentDataType<TOutputType> Output;
        typedef AssignmentDataVectorType<TOutputType> OutputVector;
        typedef AssignmentDataVectorPairType<const Location, TOutputType> OutputVectorPairByLocation;

        r_result_vector.clear();

        for( const OutputVectorPairByLocation & r_assignor_output_vector_pair : mAssignorOutputs )
        {
            const OutputVector & r_assignor_output_vector = r_assignor_output_vector_pair.second;
            for( const Output & r_output : r_assignor_output_vector )
            {
                r_result_vector.push_back(r_output);
            }
        }
    }

private:
    TCommunicatorType * mpCommunicator;

    ContractorManagerType<TAssignorType> * mpAssignorManager;
    ContractorManagerType<TAssigneeType> * mpAssigneeManager;

    AssignmentKeyIssuer mAssignmentKeyIssuer;

    AssignmentDataVectorMapType<Location,           TInputType> mAssignorInputs;
    AssignmentDataVectorMapType<Location,           TInputType> mAssigneeInputs;
    AssignmentDataVectorMapType<TContractorKeyType, TInputType> mWorkUnitInputs;

    AssignmentDataVectorMapType<Location,           TOutputType> mAssignorOutputs;
    AssignmentDataVectorMapType<Location,           TOutputType> mAssigneeOutputs;
    AssignmentDataVectorMapType<TContractorKeyType, TOutputType> mWorkUnitOutputs;
};

}

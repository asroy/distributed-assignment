#include<mpi.h>
#include<MpiLocation.h>
#include<set>

template<typename TContractorKeyType>
class Ball
{
public:
    Ball()
    :   mFromRival{}
    {}

    Ball( const TContractorKeyType from )
    :   mFromRival{from}
    {}

    ~Ball()
    {}

    TContractorKeyType mFromRival;

private:

    void Save( DataUtility::Serializer & r_serializer ) const
    {
        r_serializer.Save(mFromRival);
    }

    void Load( DataUtility::Serializer & r_serializer )
    {
        r_serializer.Load(mFromRival);
    }

    void Profile( DataUtility::DataProfile & r_profile ) const
    {
        r_profile.SetIsTrivial(false);
    }

    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{Ball: ";
        r_printer.Print(mFromRival);
        std::cout << "}";
    }

    friend class DataUtility::Serializer;
    friend class DataUtility::DataProfile;
    friend class DataUtility::DataPrinter;
};

template<typename TContractorKeyType>
class PingPongPlayer
{
public:
    using ContractorKeySet = std::set<TContractorKeyType, typename TContractorKeyType::LessThanComparator>;

    PingPongPlayer()
    :   mKey(),
        mRivals()
    {}

    ~PingPongPlayer()
    {}

    void SetKey(const TContractorKeyType contrator_key)
    { mKey = contrator_key; }

    TContractorKeyType GetKey() const
    { return mKey; }

    void Execute( const Ball<TContractorKeyType> & r_in, Ball<TContractorKeyType> & r_out )
    {
        TContractorKeyType from = GetKey();

        auto it = mRivals.find(my_key);

        if( it == mRivals.end() )
        {
            std::cout<<__func__<<": cannot find myself! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        for( int i = 0; i < n; i++ )
        {
            it = std::next(it);

            if( it == mRivals.end() )
                it = mRivals.begin();
        }

        r_out = { GetKey(), *it };

        DataUtility::DataPrinter printer;

        std::cout<<"ball is at player";
        printer.Print( GetKey() );
        std::cout<<"going to player";
        printer.Print( r_out.mTo );
        std::cout<<std::endl;
    }

    void Execute( const std::vector<int> & r_inputs, std::vector<Ball<TContractorKeyType>> & r_outputs )
    {
        int size = (int) r_inputs.size();

        r_outputs.clear();
        r_outputs.resize(size);

        for( int i = 0; i < size; i++ )
        {
            int & r_input = r_inputs[i];
            Ball<TContractorKeyType> & r_output = r_outputs[i];
            Execute( r_input, r_output );
        }
    }

    void SetRivals( const ContractorKeySet & r_rivals )
    {
        mRivals.clear();
        mRivals = r_rivals;
    }

private:
    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{PingPongPlayer: ";
        std::cout << "{ContractorKey: ";
        r_printer.Print(mKey);
        std::cout << "},";
        std::cout << "},";
    }

    TContractorKeyType mKey;
    ContractorKeySet mRivals;

    friend class DataUtility::DataPrinter;
};
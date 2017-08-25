class DummyContractor
{
    void Execute( TInputDataType &, TOutputDataType & );

    void Execute( std::vector<TInputDataType> &, std::vector<TOutputDataType> );
}
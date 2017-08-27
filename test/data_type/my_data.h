#include<iostream>

class B
{
public:
    int i;
    double z;
    char c;

private:
    void Save( DataUtility::Serializer & r_serializer ) const
    {
        r_serializer.Save(i);
        r_serializer.Save(z);
        r_serializer.Save(c);
    }

    void Load( DataUtility::Serializer & r_serializer )
    {
        r_serializer.Load(i);
        r_serializer.Load(z);
        r_serializer.Load(c);
    }

    void Profile( DataUtility::DataProfile & r_profile ) const
    {
        r_profile.SetIsTrivial(false);
    }

    void Print( const DataUtility::DataPrinter & r_printer ) const
    {
        std::cout << "{B: ";
        r_printer.Print(i);
        r_printer.Print(z);
        r_printer.Print(c);
        std::cout << "}";
    }

    friend class DataUtility::Serializer;
    friend class DataUtility::DataProfile;
    friend class DataUtility::DataPrinter;
};

class A
{
  public:
    int i;
    double x;
    double y[2];
    B b;

    void Save( DataUtility::Serializer & r_serializer ) const
    {
        r_serializer.Save(i);
        r_serializer.Save(x);
        for( int i = 0; i < 2; i++ )
          r_serializer.Save(y[i]);
        r_serializer.Save(b);
    }

    void Load( DataUtility::Serializer & r_serializer )
    {
        r_serializer.Load(i);
        r_serializer.Load(x);
        for( int i = 0; i < 2; i++ )
          r_serializer.Load(y[i]);
        r_serializer.Load(b);
    }

    void Profile( DataUtility::DataProfile & r_profile ) const
    {
       r_profile.SetIsTrivial(false);
    }

    void Print( const DataUtility::DataPrinter & r_data_printer ) const
    {
        std::cout << "{A: ";
        r_data_printer.Print(i);
        r_data_printer.Print(x);
        for( int i = 0; i < 2; i++ )
          r_data_printer.Print(y[i]);
        r_data_printer.Print(b);
        std::cout << "}";
    }

    friend class DataUtility::Serializer;
    friend class DataUtility::DataProfile;
    friend class DataUtility::DataPrinter;
};


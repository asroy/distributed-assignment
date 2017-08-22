class B
{
  public:
    int i;
    double z;
    char c;

    void Save( Serializer & r_serializer ) const
    {
      r_serializer.Save(i);
      r_serializer.Save(z);
      r_serializer.Save(c);
    }

    void Load( Serializer & r_serializer )
    {
      r_serializer.Load(i);
      r_serializer.Load(z);
      r_serializer.Load(c);
    }

    DataProfile Profile( DataProfile & r_data_profile )
    {
      return r_data_profile.MakeNonTrivial();
    }

  friend class Serializer;
  friend class DataProfile;
};

class A
{
  public:
    int i;
    double x;
    double y[2];
    B b;

    void Save( Serializer & r_serializer ) const
    {
      r_serializer.Save(i);
      r_serializer.Save(x);
      for( int i = 0; i < 2; i++ )
        r_serializer.Save(y[1]);
      r_serializer.Save(b);
    }

    void Load( Serializer & r_serializer )
    {
      r_serializer.Load(i);
      r_serializer.Load(x);
      for( int i = 0; i < 2; i++ )
        r_serializer.Load(y[1]);
      r_serializer.Load(b);
    }

    DataProfile Profile( DataProfile & r_data_profile )
    {
      return r_data_profile.MakeNonTrivial();
    }

  friend class Serializer;
  friend class DataProfile;
};


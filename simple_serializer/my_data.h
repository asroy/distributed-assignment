class B
{
  public:
    int i;
    double z;
    char c;

    void SaveData( Serializer & r_serializer, bool & r_i_am_trivial ) const
    {
      r_serializer.SaveData(i);
      r_serializer.SaveData(z);
      r_serializer.SaveData(c);

      r_i_am_trivial = false;
    }

    void LoadData( Serializer & r_serializer )
    {
      r_serializer.Load(i);
      r_serializer.Load(z);
      r_serializer.Load(c);
    }

    void LoadTrivial( Serializer & r_serializer )
    {
      std::cout << __func__ << "not defined" << std::endl;
      exit(EXIT_FAILURE);
    }

  friend class Serializer;
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

  friend class Serializer;
};


#ifndef SR_STEELTYPE_H
#define SR_STEELTYPE_H

#include <queue>


class SteelArrayRef
{
public:
    SteelArrayRef(){}
    ~SteelArrayRef(){}

    std::string getArrayRef() const { return m_array; }
    void setArrayRef( const std::string &s){ m_array =s;}
private:
    std::string m_array;
    friend bool operator<(const SteelArrayRef &lhs, 
			  const SteelArrayRef &rhs);
    friend bool operator==(const SteelArrayRef &lhs, 
			  const SteelArrayRef &rhs);
};

bool operator<(const SteelArrayRef &lhs, const SteelArrayRef &rhs);
bool operator==(const SteelArrayRef &lhs, const SteelArrayRef &rhs);
    


class SteelType
{
private:
    enum storage
    {
	ARRAY,
	BOOL,
	INT,
	DOUBLE,
	STRING
    };

public:
    SteelType();
    SteelType(const SteelType &);
    ~SteelType();

    operator int () const;
    operator double () const;
    operator std::string () const;
    operator bool () const;
    operator SteelArrayRef () const;

    void set(int i);
    void set(double d);
    void set(bool b);
    void set(const std::string &);
    void set(const SteelArrayRef &ref);

    bool isArray() const { return m_storage == ARRAY; }


    // Assignment
    SteelType & operator=(const SteelType &rhs);
    // Unary operators
    SteelType operator-();
    SteelType operator!();
    // Binary operators
    SteelType  operator+(const SteelType &rhs);
    SteelType  operator-(const SteelType &rhs);
    SteelType  operator*(const SteelType &rhs);
    SteelType  operator^(const SteelType &rhs);
    SteelType  operator/(const SteelType &rhs);
    SteelType  operator%(const SteelType &rhs);
    SteelType  operator==(const SteelType &rhs);
    SteelType  operator!=(const SteelType &rhs);
    SteelType  operator<(const SteelType &rhs);
    SteelType  operator<=(const SteelType &rhs);
    SteelType  operator>(const SteelType &rhs);
    SteelType  operator>=(const SteelType &rhs);
    SteelType  operator&&(const SteelType &rhs);
    SteelType  operator||(const SteelType &rhs);
    SteelType  d(const SteelType &rhs);

private:
    int strInt() const ;
    double strDouble()const;
    bool strBool()const;
    std::string strToBool(bool b)const;
    std::string strToInt(int i)const;
    std::string strToDouble(double d)const;


    union value
    {
	bool b;
	double d;
	int i;
	SteelArrayRef *a;
	std::string *s;
    };
    value m_value;
    storage m_storage;

};


class ParamList
{
public:
    ParamList();
    ParamList(const ParamList &rhs);
    virtual ~ParamList();
    
    SteelType next();
    void enqueue(const SteelType &type);
    ParamList & operator=(const ParamList &rhs);
private:
    std::queue<SteelType> m_params;
};


#endif

#ifndef SR_STEELTYPE_H
#define SR_STEELTYPE_H

#include <queue>


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
    operator std::vector<SteelType> () const;

    void set(int i);
    void set(double d);
    void set(bool b);
    void set(const std::string &);
    void set(const std::vector<SteelType> &);

    // Array stuff
    bool isArray() const { return m_storage == ARRAY; }
    SteelType getElement(int index) const;
    SteelType *getLValue(int index) const;
    void setElement(int index,const SteelType &);
    int getArraySize()const;
    void add(const SteelType &var);
    void removeTail();
    void reserveArray(int index);

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
    SteelType  operator<(const SteelType &rhs);
    SteelType  operator<=(const SteelType &rhs);
    SteelType  operator>(const SteelType &rhs);
    SteelType  operator>=(const SteelType &rhs);
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
	std::string *s;
	std::vector<SteelType> *a;
    };
    value m_value;
    storage m_storage;

    friend bool operator==(const SteelType &lhs, const SteelType &rhs);
};

typedef std::vector<SteelType> SteelArray;

bool operator==(const SteelType &lhs, const SteelType &rhs);
bool operator!=(const SteelType &lhs, const SteelType &rhs);
#endif

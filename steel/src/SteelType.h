#ifndef SR_STEELTYPE_H
#define SR_STEELTYPE_H

#include <functional>
#include <string>
#include <deque>
#include <memory>
#include <unordered_map>
#include <memory>

namespace Steel { 
class SteelFunctor;


class SteelType
{
public:
    SteelType();
    SteelType(const SteelType &);
    SteelType(SteelType&&)noexcept;
    ~SteelType();
   
    // The only need for this class is so that dynamic_cast can be used
    class IHandle{
    public:
	IHandle(){}
	virtual ~IHandle(){}
    };

    typedef IHandle * Handle;
    typedef std::shared_ptr<SteelFunctor> Functor;
    typedef std::deque<SteelType> Container;
    typedef std::unordered_map<std::string,SteelType,std::hash<std::string>> Map;

    //operator int () const;
    operator const int () const;
    operator const unsigned int() const { return static_cast<unsigned int>( (int)(*this) ); }
    operator const double () const;
    operator const std::string () const;
    operator bool () const;
    operator const Container () const;
    operator Handle () const;
    operator Functor () const;
    operator const Map () const;
    void set(int i);
    void set(double d);
    void set(bool b);
    void set(const std::string &);
    void set(const Container &);
    void set(Handle h);
    void set(Functor f);
    void set(const Map& );

    // Array stuff
    bool isArray() const { return m_storage == ARRAY; }
    bool isHashMap() const { return m_storage == HASHMAP; }
    SteelType getElement(int index) const;
    SteelType getElement(const std::string& key) const;
    SteelType *getLValue(const std::string& key) const;
    SteelType *getLValue(int index) const;
    void setElement(int index,const SteelType &);
    void setElement(const std::string& key, const SteelType&);
    int getArraySize()const;
    void add(const SteelType &var); // adds to the tail (append) (Note: Different from pushb)
    void add(const Map& map);
    SteelType removeElement(int index);
    SteelType pop();
    SteelType pop_back();
    void shuffle();
    void push(const SteelType &var); // adds to the front
    void pushb(const SteelType &var); // adds to the back (adds an array as a single element)

    // Handle stuff
    bool isHandle() const { return m_storage == HANDLE; }
    bool isValidHandle() const { return isHandle() && m_value.h != NULL; }

    // Functor stuff
    bool isFunctor() const { return m_storage == FUNCTOR; }
    Functor getFunctor()const;
    // Assignment
    SteelType & operator=(const SteelType &rhs);
    SteelType & operator=(SteelType&& rhs)noexcept;
    // Unary operators
    SteelType operator-();
    SteelType operator!();
    SteelType operator++();
    SteelType operator++(int); //postfix
    SteelType operator--();
    SteelType operator--(int); //postfix
  
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
    // Assignment binary ops
    SteelType operator+=(const SteelType &rhs);
    SteelType operator-=(const SteelType &rhs);
    SteelType operator*=(const SteelType &rhs);
    SteelType operator/=(const SteelType &rhs);
    SteelType operator%=(const SteelType &rhs);

    bool isConst()const;
    void makeConst();
#ifndef NDEBUG
    void debugPrint();
#endif
private:
    enum storage
    {
        ARRAY,
        BOOL,
        INT,
        DOUBLE,
        STRING,
        HANDLE,
	FUNCTOR,
	HASHMAP
    };

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
        Handle h;
        std::string *s;
    };
    Functor m_functor;
    std::shared_ptr<Container> m_array;
    std::shared_ptr<Map> m_map;
    value m_value;
    storage m_storage;
    bool m_bConst:1;
    bool m_bCopyArray:1;
    bool m_bCopyHash:1;
    friend bool operator==(const SteelType &lhs, const SteelType &rhs);
};


// Why two typedefs for the same thing? One is truly an array of steeltypes,
// the other is a special array type that HAPPENS to only be an array of steeltypes as well
typedef SteelType::Container SteelArray;

bool operator==(const SteelType &lhs, const SteelType &rhs);
bool operator!=(const SteelType &lhs, const SteelType &rhs);

}
#endif



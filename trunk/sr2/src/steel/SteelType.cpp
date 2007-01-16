#include <string>
#include <sstream>
#include <cmath>
#include "SteelType.h"
#include "SteelException.h"


bool operator<(const SteelArrayRef &lhs, const SteelArrayRef &rhs)
{
    return lhs.m_array < rhs.m_array;
}
bool operator==(const SteelArrayRef &lhs, const SteelArrayRef &rhs)
{
    return lhs.m_array == rhs.m_array;
}

SteelType::SteelType()
{
    m_value.i = 0;
    m_value.s = NULL;
    m_value.a = NULL;
    m_storage = SteelType::INT;
}

SteelType::SteelType(const SteelType &rhs)
{
    set(0);
    *this = rhs;
}

SteelType::~SteelType()
{
    if(m_storage == SteelType::STRING)
	delete m_value.s;
    else if (m_storage == SteelType::ARRAY)
	delete m_value.a;

    m_storage = SteelType::INT;
}


SteelType::operator int () const
{
    switch(m_storage)
    {
    case SteelType::ARRAY:
	throw TypeMismatch();
    case SteelType::BOOL:
	if(m_value.b) return 1;
	else return 0;
    case SteelType::INT:
	return m_value.i;
    case SteelType::DOUBLE:
	return static_cast<int>(m_value.d);
    case SteelType::STRING:
	return strInt();
    }
	assert ( 0 );
	return 0;
}

SteelType::operator double () const
{
    switch(m_storage)
    {
    case SteelType::ARRAY:
	throw TypeMismatch();
    case SteelType::BOOL:
	if(m_value.b) return 1.0;
	else  return 0.0;
    case SteelType::INT:
	return m_value.i;
    case SteelType::DOUBLE:
	return m_value.d;
    case SteelType::STRING:
	return strDouble();
    }
	assert( 0 );
	return 0.0;
}

SteelType::operator std::string () const
{
    switch(m_storage)
    {
    case SteelType::ARRAY:
	throw TypeMismatch();
    case SteelType::BOOL:
	if(m_value.b) return "TRUE";
	else return "FALSE";
    case SteelType::INT:
	return strToInt(m_value.i);
    case SteelType::DOUBLE:
	return strToDouble(m_value.d);
    case SteelType::STRING:
	return *m_value.s;
    }
    return "";
}

SteelType::operator bool () const
{
    switch(m_storage)
    {
    case SteelType::ARRAY:
	return true;
    case SteelType::BOOL:
	return m_value.b;
    case SteelType::INT:
	return m_value.i != 0;
    case SteelType::DOUBLE:
	return m_value.d != 0.0;
    case SteelType::STRING:
	return !m_value.s->empty();
    }
    return true;
}

SteelType::operator SteelArrayRef () const
{
    if( !isArray() ) throw TypeMismatch();

    else return *m_value.a;
}

void SteelType::set(int i)
{
    m_value.i = i;
    m_storage = SteelType::INT;
}

void SteelType::set(double d)
{
    m_value.d = d;
    m_storage = SteelType::DOUBLE;
}

void SteelType::set(bool b)
{
    m_value.b = b;
    m_storage = SteelType::BOOL;
}

void SteelType::set(const std::string &str)
{
    m_value.s  = new std::string(str);
    m_storage = SteelType::STRING;
}

void SteelType::set(const SteelArrayRef &ref)
{
    m_value.a = new SteelArrayRef();
    *m_value.a = ref;
    m_storage = SteelType::ARRAY;
}

SteelType & SteelType::operator=(const SteelType &rhs)
{
    if(&rhs == this) return *this;

    if(m_storage == SteelType::STRING)
	delete m_value.s;
    else if (m_storage == SteelType::ARRAY)
	delete m_value.a;

    m_storage = SteelType::INT;

    if(rhs.m_storage == SteelType::STRING)
	set ( *rhs.m_value.s );
    else if ( rhs.m_storage == SteelType::ARRAY)
	set ( *rhs.m_value.a );
    else if ( rhs.m_storage == SteelType::BOOL)
	set ( rhs.m_value.b );
    else if ( rhs.m_storage == SteelType::INT)
	set ( rhs.m_value.i );
    else if ( rhs.m_storage == SteelType::DOUBLE)
	set ( rhs.m_value.d );

    return *this;
}


SteelType  SteelType::operator+(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( (int)*this + (int)rhs );
	return val;
    case SteelType::DOUBLE:
	val.set ( (double)*this + (double)rhs);
	return val;
    case SteelType::STRING:
	val.set ( (std::string)*this + (std::string)rhs);
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator-(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( (int)*this - (int)rhs );
	return val;
    case SteelType::DOUBLE:
	val.set ( (double)*this - (double)rhs);
	return val;
    case SteelType::STRING:
	throw OperationMismatch();
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator*(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( (int)*this * (int)rhs );
	return val;
    case SteelType::DOUBLE:
	val.set ( (double)*this * (double)rhs);
	return val;
    case SteelType::STRING:
	throw OperationMismatch();
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator^(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( pow(*this,(int)rhs) );
	return val;
    case SteelType::DOUBLE:
	val.set ( pow(*this,(double)rhs) );
	return val;
    case SteelType::STRING:
	throw OperationMismatch();
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator/(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( (int)*this / (int)rhs );
	return val;
    case SteelType::DOUBLE:
	val.set ( (double)*this / (double)rhs);
	return val;
    case SteelType::STRING:
	throw OperationMismatch();
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator%(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
	return val;
    case SteelType::INT:
	val.set ( (int)*this % (int)rhs );
	return val;
    case SteelType::DOUBLE:
	val.set ( (int)*this % (int)rhs);
	return val;
    case SteelType::STRING:
	throw OperationMismatch();
	return val;
    }

	assert ( 0 );
	return val;
}

SteelType  SteelType::operator==(const SteelType &rhs)
{
    SteelType val;
    val.set(false);
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
	if( m_value.a == rhs.m_value.a)
	    val.set(true);
	break;
    case SteelType::BOOL:
	if((bool)*this == (bool)rhs)
	    val.set(true);
	break;
    case SteelType::INT:
	if((int)*this == (int)rhs)
	    val.set(true);
	break;
    case SteelType::DOUBLE:
	if((double)*this == (double)rhs)
	    val.set(true);
	break;
    case SteelType::STRING:
	if((std::string)*this == (std::string)rhs)
	    val.set(true);
	   
    }

    return val;
}

SteelType  SteelType::operator!=(const SteelType &rhs)
{
    SteelType var;

    if( *this == rhs )
	var.set(false);
    else var.set(true);
}

SteelType  SteelType::operator<(const SteelType &rhs)
{
    SteelType val;
    val.set(false);
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
    case SteelType::INT:
	if((int)*this < (int)rhs)
	    val.set(true);
    case SteelType::DOUBLE:
	if((double)*this < (double)rhs)
	    val.set(true);
    case SteelType::STRING:
	if((std::string)*this < (std::string)rhs)
	    val.set(true);
	   
    }

    return val;
}

SteelType  SteelType::operator<=(const SteelType &rhs)
{
    SteelType val;
    val.set(false);
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case SteelType::ARRAY:
    case SteelType::BOOL:
	throw OperationMismatch();
    case SteelType::INT:
	if((int)*this <= (int)rhs)
	    val.set(true);
    case SteelType::DOUBLE:
	if((double)*this <= (double)rhs)
	    val.set(true);
    case SteelType::STRING:
	if((std::string)*this <= (std::string)rhs)
	    val.set(true);
	   
    }
}

SteelType  SteelType::operator>(const SteelType &rhs)
{
    SteelType var;
    if( *this <= rhs ) 
	var.set(false);
    else var.set(true);
    
    return var;
}

SteelType  SteelType::operator>=(const SteelType &rhs)
{
    SteelType var;
    
    if(*this < rhs )
	var.set(false);
    else var.set(true);

    return var;
}

SteelType  SteelType::d(const SteelType &rhs)
{
    SteelType var;
    int dice = (int)*this;
    int sides = (int)rhs;

    int total = 0;

    for(int d = 0; d < dice; d++)
    {
	total +=  rand() % sides + 1;
    }

    var.set(total);

    return var;
}

SteelType  SteelType::operator&&(const SteelType &rhs)
{
    SteelType var;
    if ( (bool)*this && (bool)rhs )
	var.set(true);
    else var.set(false);
    return var;
}
SteelType  SteelType::operator||(const SteelType &rhs)
{
    SteelType var;
    if ((bool)*this || (bool)rhs )
	var.set(true);
    else var.set(false);
    return var;
}

SteelType SteelType::operator-()
{
    SteelType var;
    switch(m_storage)
    {
    case SteelType::BOOL:
    case SteelType::STRING:
    case SteelType::ARRAY:
	throw OperationMismatch();
    case SteelType::INT:
	var.set(0 - m_value.i);
	break;
    case SteelType::DOUBLE:
	var.set(0.0 - m_value.d);
	break;
    }

    return var;
}

SteelType SteelType::operator!()
{
    SteelType var;
    switch(m_storage)
    {
    case SteelType::BOOL:
	if(m_value.b)
	    var.set ( false );
	else var.set( true );
    case SteelType::INT:	
    case SteelType::STRING:
    case SteelType::DOUBLE:
    case SteelType::ARRAY:
	throw OperationMismatch();
    }

    return var;
}

int SteelType::strInt() const
{
    if(m_storage != SteelType::STRING) return 0;

    std::istringstream str(*m_value.s);

    int i;
    str >> i;

    return i;
}

double SteelType::strDouble() const
{
    if(m_storage != SteelType::STRING) return 0.0;
    std::istringstream str(*m_value.s);
    
    double d;
    str >> d;
    
    return d;
}

bool SteelType::strBool() const
{
    if(m_storage == SteelType::STRING && *m_value.s == "TRUE") return true;
    else return false;
}

std::string SteelType::strToBool(bool b) const
{
    if(b)
	return "TRUE";
    else return "FALSE";
}

std::string SteelType::strToInt(int i) const
{
   std::ostringstream str;

   str << i;

   return str.str();
}

std::string SteelType::strToDouble(double d) const
{
    std::ostringstream str;

    str << d;

    return str.str();
}






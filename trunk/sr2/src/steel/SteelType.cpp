#include <string>
#include <sstream>
#include <cmath>
#include "SteelType.h"
#include "SteelException.h"

SteelType::SteelType()
{
    m_value.i = 0;
    m_storage = INT;
}

SteelType::SteelType(const SteelType &rhs)
{
    *this = rhs;
}

SteelType::~SteelType()
{
    if(m_storage == STRING)
	delete m_value.s;
    else if (m_storage == ARRAY)
	delete m_value.a;
}


SteelType::operator int () const
{
    switch(m_storage)
    {
    case ARRAY:
	throw TypeMismatch();
    case BOOL:
	if(m_value.b) return 1;
	else return 0;
    case INT:
	return m_value.i;
    case DOUBLE:
	return static_cast<int>(m_value.d);
    case STRING:
	return strInt();
    }
}

SteelType::operator double () const
{
    switch(m_storage)
    {
    case ARRAY:
	throw TypeMismatch();
    case BOOL:
	if(m_value.b) return 1.0;
	else  return 0.0;
    case INT:
	return m_value.i;
    case DOUBLE:
	return m_value.d;
    case STRING:
	return strDouble();
    }
}

SteelType::operator std::string () const
{
    switch(m_storage)
    {
    case ARRAY:
	throw TypeMismatch();
    case BOOL:
	if(m_value.b) return "TRUE";
	else return "FALSE";
    case INT:
	return strToInt(m_value.i);
    case DOUBLE:
	return strToDouble(m_value.d);
    case STRING:
	return *m_value.s;
    }
    return "";
}

SteelType::operator bool () const
{
    switch(m_storage)
    {
    case ARRAY:
	return true;
    case BOOL:
	return m_value.b;
    case INT:
	return m_value.i != 0;
    case DOUBLE:
	return m_value.d != 0.0;
    case STRING:
	return m_value.s->size() > 0;
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
    m_storage = INT;
}

void SteelType::set(double d)
{
    m_value.d = d;
    m_storage = DOUBLE;
}

void SteelType::set(bool b)
{
    m_value.b = b;
    m_storage = BOOL;
}

void SteelType::set(const std::string &str)
{
    m_value.s  = new std::string(str);
    m_storage = STRING;
}

void SteelType::set(const SteelArrayRef &ref)
{
    m_value.a = new SteelArrayRef();
    *m_value.a = ref;
    m_storage = ARRAY;
}

SteelType & SteelType::operator=(const SteelType &rhs)
{
    if(&rhs == this) return *this;

    if(m_storage == STRING)
	delete m_value.s;
    else if (m_storage == ARRAY)
	delete m_value.a;
    m_value = rhs.m_value;
    m_storage = rhs.m_storage;

    return *this;
}


SteelType  SteelType::operator+(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( (int)*this + (int)rhs );
	return val;
    case DOUBLE:
	val.set ( (double)*this + (double)rhs);
	return val;
    case STRING:
	val.set ( (std::string)*this + (std::string)rhs);
	return val;
    }
}

SteelType  SteelType::operator-(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( (int)*this - (int)rhs );
	return val;
    case DOUBLE:
	val.set ( (double)*this - (double)rhs);
	return val;
    case STRING:
	throw OperationMismatch();
	return val;
    }
}

SteelType  SteelType::operator*(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( (int)*this * (int)rhs );
	return val;
    case DOUBLE:
	val.set ( (double)*this * (double)rhs);
	return val;
    case STRING:
	throw OperationMismatch();
	return val;
    }
}

SteelType  SteelType::operator^(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( pow(*this,rhs) );
	return val;
    case DOUBLE:
	val.set ( pow(*this,rhs) );
	return val;
    case STRING:
	throw OperationMismatch();
	return val;
    }
}

SteelType  SteelType::operator/(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( (int)*this / (int)rhs );
	return val;
    case DOUBLE:
	val.set ( (double)*this / (double)rhs);
	return val;
    case STRING:
	throw OperationMismatch();
	return val;
    }
}

SteelType  SteelType::operator%(const SteelType &rhs)
{
    SteelType val;
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
	return val;
    case INT:
	val.set ( (int)*this % (int)rhs );
	return val;
    case DOUBLE:
	val.set ( (int)*this % (int)rhs);
	return val;
    case STRING:
	throw OperationMismatch();
	return val;
    }
}

SteelType  SteelType::operator==(const SteelType &rhs)
{
    SteelType val;
    val.set(false);
    storage s = std::max(m_storage,rhs.m_storage);

    switch(s)
    {
    case ARRAY:
	if( m_value.a == rhs.m_value.a)
	    val.set(true);
	break;
    case BOOL:
	if((bool)*this == (bool)rhs)
	    val.set(true);
	break;
    case INT:
	if((int)*this == (int)rhs)
	    val.set(true);
	break;
    case DOUBLE:
	if((double)*this == (double)rhs)
	    val.set(true);
	break;
    case STRING:
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
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
    case INT:
	if((int)*this < (int)rhs)
	    val.set(true);
    case DOUBLE:
	if((double)*this < (double)rhs)
	    val.set(true);
    case STRING:
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
    case ARRAY:
    case BOOL:
	throw OperationMismatch();
    case INT:
	if((int)*this <= (int)rhs)
	    val.set(true);
    case DOUBLE:
	if((double)*this <= (double)rhs)
	    val.set(true);
    case STRING:
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
	total +=  1+ (rand() % sides);
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
    case BOOL:
    case STRING:
    case ARRAY:
	throw OperationMismatch();
    case INT:
	var.set(0 - m_value.i);
	break;
    case DOUBLE:
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
    case BOOL:
	if(m_value.b)
	    var.set ( false );
	else var.set( true );
    case INT:	
    case STRING:
    case DOUBLE:
    case ARRAY:
	throw OperationMismatch();
    }

    return var;
}

int SteelType::strInt() const
{
    if(m_storage != STRING) return 0;

    std::istringstream str(*m_value.s);

    int i;
    str >> i;

    return i;
}

double SteelType::strDouble() const
{
    if(m_storage != STRING) return 0.0;
    std::istringstream str(*m_value.s);
    
    double d;
    str >> d;
    
    return d;
}

bool SteelType::strBool() const
{
    if(m_storage == STRING && *m_value.s == "TRUE") return true;
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


ParamList::ParamList()
{
}

ParamList::~ParamList()
{
}

ParamList::ParamList(const ParamList &rhs)
{
    *this = rhs;
}

ParamList & ParamList::operator=(const ParamList &rhs)
{
    if( &rhs == this ) return *this;
    m_params = rhs.m_params;

    return *this;
}
    
SteelType ParamList::next()
{
    if(m_params.size() < 1) 
    {
	throw ParamMismatch();
    }

    SteelType var = m_params.front();
    m_params.pop();
    return var;
}

void ParamList::enqueue(const SteelType &type)
{
    m_params.push(type);
}



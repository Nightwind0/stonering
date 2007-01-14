#include <string>
#include <queue>
#include "SteelFunctor.h"

SteelType::SteelType()
{
}

SteelType::SteelType(const SteelType &)
{
}

SteelType::~SteelType()
{
}


SteelType::operator int ()
{
    return 0;
}

SteelType::operator double ()
{
    return 0.0;
}

SteelType::operator std::string ()
{
    return "";
}

SteelType::operator bool ()
{
    return true;
}


SteelType & SteelType::operator=(const SteelType &rhs)
{
    return *this;
}





ParamList::ParamList()
{
}

ParamList::~ParamList()
{
}
    
SteelType ParamList::next()
{
    // TODO: Throw mismatched param number exception
    // Interim: just return an uninitlized var
    if(m_params.size() < 1) return SteelType();

    SteelType var = m_params.front();
    m_params.pop();
    return var;
}

void ParamList::enqueue(const SteelType &type)
{
    m_params.push(type);
}



SteelFunctor::SteelFunctor()
{
}

SteelFunctor::~SteelFunctor()
{
}


#include "MonsterRef.h"

using StoneRing::MonsterRef;

MonsterRef::MonsterRef():m_nCount(1)
{
}

MonsterRef::~MonsterRef()
{
}


std::string MonsterRef::GetName() const
{
    return m_name;
}

int MonsterRef::GetCount() const
{
    return m_nCount;
}


void MonsterRef::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);
    m_nCount = get_implied_int("count",pAttributes,1);
    m_nCellX = get_implied_int("cellX",pAttributes,0);
    m_nCellY = get_implied_int("cellY",pAttributes,0);
    m_nColumns = get_implied_int("cols",pAttributes,1);
    m_nRows = get_implied_int("rows",pAttributes,1);

#ifndef NDEBUG
    std::cout << "Count = " << m_nCount << " Cols = " << m_nColumns << " Rows = " << m_nRows << std::endl;
#endif

    if(m_nColumns * m_nRows < m_nCount)
        throw CL_Error("MonsterRef: " + m_name + " has too few rows and columns for count.");
}

void MonsterRef::load_finished()
{
}
        
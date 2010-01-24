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


void MonsterRef::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nCount = get_implied_int("count",attributes,1);
    m_nCellX = get_implied_int("cellX",attributes,0);
    m_nCellY = get_implied_int("cellY",attributes,0);
    m_nColumns = get_implied_int("cols",attributes,1);
    m_nRows = get_implied_int("rows",attributes,1);

#ifndef NDEBUG
    std::cout << "Count = " << m_nCount << " Cols = " << m_nColumns << " Rows = " << m_nRows << std::endl;
#endif

    if(m_nColumns * m_nRows < m_nCount)
        throw CL_Exception("MonsterRef: " + m_name + " has too few rows and columns for count.");
}

void MonsterRef::load_finished()
{
}

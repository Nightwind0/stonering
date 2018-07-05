#include "MonsterRef.h"

using StoneRing::MonsterRef;
using StoneRing::DynamicMonsterRef;

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


void MonsterRef::load_attributes(clan::DomNamedNodeMap attributes)
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
        throw XMLException("MonsterRef: " + m_name + " has too few rows and columns for count.");
}

void MonsterRef::load_finished()
{
}


DynamicMonsterRef::DynamicMonsterRef()
{
}

DynamicMonsterRef::~DynamicMonsterRef()
{
}

void DynamicMonsterRef::SetName(const std::string &name)
{
    m_name = name;
}

void DynamicMonsterRef::SetCount(int count)
{
    m_nCount = count;
}

void DynamicMonsterRef::SetCellX(int cell_x)
{
    m_nCellX = cell_x;
}

void DynamicMonsterRef::SetCellY(int cell_y)
{
    m_nCellY = cell_y;
}

void DynamicMonsterRef::SetColumns(int cols)
{
    m_nColumns = cols;
}

void DynamicMonsterRef::SetRows(int rows)
{
    m_nRows = rows;
}

#if SR2_EDITOR
clan::DomElement MonsterRef::CreateDomElement(clan::DomDocument& doc) const 
{
    clan::DomElement element(doc,"monsterRef");
    element.set_attribute("name",m_name);
    element.set_attribute("count",IntToString(m_nCount));
    element.set_attribute("cellX",IntToString(m_nCellX));
    element.set_attribute("cellY",IntToString(m_nCellY));
    element.set_attribute("cols",IntToString(m_nColumns));
    element.set_attribute("rows",IntToString(m_nRows));
 
    return element;
}
#endif

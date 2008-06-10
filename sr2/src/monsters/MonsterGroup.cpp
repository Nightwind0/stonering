#include "MonsterGroup.h"
#include "MonsterRef.h"


using StoneRing::MonsterGroup;


MonsterGroup::MonsterGroup()
{
}

MonsterGroup::~MonsterGroup()
{
    std::for_each(m_monsters.begin(),m_monsters.end(),del_fun<MonsterRef>());
}
        
bool MonsterGroup::handle_element(StoneRing::Element::eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case EMONSTERREF:
        m_monsters.push_back ( dynamic_cast<MonsterRef*>(pElement) );
        return true;
    default:
        return false;
    }
}

void MonsterGroup::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
     m_nWeight = get_required_int("weight",pAttributes);
     m_nCellCols = get_implied_int("cellColumns",pAttributes, 4); // TODO: get system default
     m_nCellRows = get_implied_int("cellRows",pAttributes,4); // TODO: system default
}

void MonsterGroup::load_finished()
{
    if(m_monsters.empty()) throw CL_Error("At least one MonsterRef is required on a MonsterGroup");
}


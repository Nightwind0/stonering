#include "MonsterGroup.h"
#include "MonsterRef.h"
#include <algorithm>

using StoneRing::MonsterGroup;


MonsterGroup::MonsterGroup()
{
	m_nWeight = 0;
	m_nCellCols = 1;
	m_nCellRows = 1;
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

void MonsterGroup::load_attributes(clan::DomNamedNodeMap attributes)
{
     m_nWeight = get_required_int("weight",attributes);
     m_nCellCols = get_implied_int("cellColumns",attributes, 4); // TODO: get system default
     m_nCellRows = get_implied_int("cellRows",attributes,4); // TODO: system default
}

void MonsterGroup::load_finished()
{
    if(m_monsters.empty()) throw XMLException("At least one MonsterRef is required on a MonsterGroup");
}


#if SR2_EDITOR
clan::DomElement MonsterGroup::CreateDomElement(clan::DomDocument &doc) const 
{
    clan::DomElement element(doc,"monsterGroup");
    element.set_attribute("weight",IntToString(m_nWeight));
    element.set_attribute("cellColumns",IntToString(m_nCellCols));
    element.set_attribute("cellRows",IntToString(m_nCellRows));
    
    for(std::vector<MonsterRef*>::const_iterator iter = m_monsters.begin();
        iter != m_monsters.end(); iter++){
        element.append_child((*iter)->CreateDomElement(doc));
    }
    return element;
}
#endif

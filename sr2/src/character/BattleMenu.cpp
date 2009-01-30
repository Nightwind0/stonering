#include <list>
#include "BattleMenu.h"     
#include "BattleMenuOption.h"

using StoneRing::BattleMenu;
using StoneRing::BattleMenuOption;

BattleMenu::BattleMenu()
{
}

BattleMenu::~BattleMenu()
{
}


BattleMenu::eType BattleMenu::GetType ( void ) const
{
    return m_eType;
}


std::list<BattleMenuOption*>::iterator 
BattleMenu::GetOptionsBegin() 
{
    return m_options.begin();
}


std::list<BattleMenuOption*>::iterator 
BattleMenu::GetOptionsEnd()
{
    return m_options.end();
}


bool BattleMenu::handle_element(Element::eElement element, Element *pElement)
{
    if(element == EBATTLEMENUOPTION)
        m_options.push_back ( dynamic_cast<BattleMenuOption*>(pElement) );
    else return false;

    return true;
}

void BattleMenu::load_finished()
{
    m_current = m_options.begin();
}


std::list<BattleMenuOption*>::iterator BattleMenu::GetSelectedOption()
{
    return m_current;
}

void BattleMenu::SelectNext()
{
    if(++m_current == m_options.end())
        m_current = m_options.begin();
}

void BattleMenu::SelectPrevious()
{
    if(m_current == m_options.begin())
    {
        m_current = m_options.end();
    }

    --m_current;
    
}



void BattleMenu::load_attributes(CL_DomNamedNodeMap *pAttr)
{
    std::string type = get_required_string("type",pAttr);

    if(type == "popup")
        m_eType = POPUP;
    else if(type == "skills")
        m_eType = SKILLS;
    else if(type == "spells")
        m_eType = SPELLS;
    else if(type == "items")
        m_eType = ITEMS;
    else if(type == "custom")
        m_eType = CUSTOM;
    else throw CL_Error("Bad BattleMenu type of " + type);
}


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


std::list<BattleMenuOption*>::const_iterator 
BattleMenu::GetOptionsBegin() const
{
    return m_options.begin();
}


std::list<BattleMenuOption*>::const_iterator 
BattleMenu::GetOptionsEnd() const
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

void BattleMenu::load_attributes(CL_DomNamedNodeMap *)
{
}


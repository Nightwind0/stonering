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
BattleMenu::getOptionsBegin() const
{
    return mOptions.begin();
}


std::list<BattleMenuOption*>::const_iterator 
BattleMenu::getOptionsEnd() const
{
    return mOptions.end();
}


bool BattleMenu::handleElement(eElement element, Element *pElement)
{
    if(element == EBATTLEMENUOPTION)
        mOptions.push_back ( dynamic_cast<BattleMenuOption*>(pElement) );
    else return false;

    return true;
}

void BattleMenu::loadAttributes(CL_DomNamedNodeMap *)
{
}


#include "MonsterGroup.h"
#include "MonsterRef.h"


using StoneRing::MonsterGroup;


MonsterGroup::MonsterGroup()
{
}

MonsterGroup::~MonsterGroup()
{
    std::for_each(mMonsters.begin(),mMonsters.end(),del_fun<MonsterRef>());
}
        
bool MonsterGroup::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EMONSTERREF:
        mMonsters.push_back ( dynamic_cast<MonsterRef*>(pElement) );
        return true;
    default:
        return false;
    }
}

void MonsterGroup::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
     mnWeight = getRequiredInt("weight",pAttributes);
     mnCellWidth = getImpliedInt("cellWidth",pAttributes, 4); // TODO: get system default
     mnCellHeight = getImpliedInt("cellHeight",pAttributes,4); // TODO: system default
}

void MonsterGroup::loadFinished()
{
    if(mMonsters.empty()) throw CL_Error("At least one MonsterRef is required on a MonsterGroup");
}


#include "MonsterRegion.h"

using namespace StoneRing;


MonsterRegion::MonsterRegion()
{
    
}

MonsterRegion::~MonsterRegion()
{
    
}


bool MonsterRegion::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EMONSTERGROUP:
        mMonsterGroups.push_back ( dynamic_cast<MonsterGroup*>(pElement) );
        break;
    default:
        return false;        
    }
    
    return true;
}

void Monster::loadAttributes(CL_DomNamedNodeMap *pAttr)
{
    mName = getRequiredString("name",pAttr);
    mLevelX = getRequiredInt("levelX", pAttr);
    mLevelY = getRequiredInt("levelY", pAttr);
    mWidth = getRequiredInt("width", pAttr);
    mHeight = getRequiredInt("height", pAttr);

    if(hasAttr(encounterRate, pAttr))
    {
        mEncounterRate = getFloat(encounterRate, pAttr);
    }
    else
    {
        //TODO: Get game wide default encounter rate.
    }

                                    
}

void MonsterRegion::loadFinished()
{
    //if monster groups is empty throw an error
    if(mMonsterGroups->empty())
    {
        throw CL_Error("Didn't provide a MonsterGroup"); 
    }

}

MonsterGroup MonsterRegion::getMonsterGroup()
{
    //Return a semi random monster group based on weights
        
    //you add up all the weights of all the groups in the region, 
    //and thats your total, and then your chance for each is the 
    //weight of that one divided by the total weight....

}



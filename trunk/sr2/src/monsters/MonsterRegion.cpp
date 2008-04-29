#include "MonsterRegion.h"
#include "MonsterGroup.h"
#include <cassert>

using StoneRing::MonsterRegion;
using StoneRing::MonsterRegions;
using StoneRing::MonsterGroup;


MonsterRegion::MonsterRegion():mLevelX(0), mLevelY(0), mWidth(0), mHeight(0),mnTotalWeight(0)
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
        {
        MonsterGroup * pGroup = dynamic_cast<MonsterGroup*>(pElement);
        assert(pGroup);
        mnTotalWeight += pGroup->getEncounterWeight();
        mMonsterGroups.push_back ( pGroup );
        break;
        }
    default:
        return false;        
    }
    
    return true;
}

void MonsterRegion::loadAttributes(CL_DomNamedNodeMap *pAttr)
{
    mLevelX = getRequiredInt("levelX", pAttr);
    mLevelY = getRequiredInt("levelY", pAttr);
    mWidth = getRequiredInt("width", pAttr);
    mHeight = getRequiredInt("height", pAttr);
    mBackdrop = getRequiredString("backdrop", pAttr);

    //TODO: Make sure this backdrop exists.

    if(hasAttr("encounterRate", pAttr))
    {
        mEncounterRate = getFloat("encounterRate", pAttr);
    }
    else
    {
        //TODO: Get game wide default encounter rate.
    }

                                    
}

void MonsterRegion::loadFinished()
{
    //if monster groups is empty throw an error
    if(mMonsterGroups.empty())
    {
        throw CL_Error("Didn't provide a MonsterGroup"); 
    }

}

MonsterGroup * MonsterRegion::getMonsterGroup() const
{
    //Return a semi random monster group based on weights
        
    //you add up all the weights of all the groups in the region, 
    //and thats your total, and then your chance for each is the 
    //weight of that one divided by the total weight....

    int n = (int)(rand() / (((double)RAND_MAX + 1)/ mnTotalWeight));
    int totalSoFar = 0;

    for(std::list<MonsterGroup*>::const_iterator it = mMonsterGroups.begin();
        it != mMonsterGroups.end(); it++)
    {
        MonsterGroup * pGroup = *it;
        totalSoFar += pGroup->getEncounterWeight();

        if(n < totalSoFar) 
            return pGroup;
    }

    assert( 0 );
    return NULL;
}


MonsterRegions::MonsterRegions()
{
}

MonsterRegions::~MonsterRegions()
{
}


MonsterRegion * MonsterRegions::getApplicableRegion(uint x, uint y) const 
{
    for(std::list<MonsterRegion*>::const_iterator it = mMonsterRegions.begin();
        it != mMonsterRegions.end(); 
        it++)
    {
        MonsterRegion * pRegion = *it;
        if(x >= pRegion->getLevelX() && x <= pRegion->getLevelX() + pRegion->getWidth()
            && y >= pRegion->getLevelY() && y <= pRegion->getLevelY() + pRegion->getHeight())
        {
            return pRegion;
        }
    }

    return NULL;
}


bool MonsterRegions::handleElement(StoneRing::Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case EMONSTERREGION:
        {
            MonsterRegion *pRegion = dynamic_cast<MonsterRegion*>(pElement);
            assert(pRegion);
            mMonsterRegions.push_back ( pRegion);
            return true;
        }
    default:
        return false;
    }
}

void MonsterRegions::loadFinished()
{
}


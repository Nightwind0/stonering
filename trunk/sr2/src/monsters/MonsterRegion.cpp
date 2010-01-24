#include "MonsterRegion.h"
#include "MonsterGroup.h"
#include <cassert>

using StoneRing::MonsterRegion;
using StoneRing::MonsterRegions;
using StoneRing::MonsterGroup;


MonsterRegion::MonsterRegion():m_LevelX(0), m_LevelY(0), m_Width(0), m_Height(0),m_nTotalWeight(0)
{

}

MonsterRegion::~MonsterRegion()
{

}


bool MonsterRegion::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EMONSTERGROUP:
        {
        MonsterGroup * pGroup = dynamic_cast<MonsterGroup*>(pElement);
        assert(pGroup);
        m_nTotalWeight += pGroup->GetEncounterWeight();
        m_monster_groups.push_back ( pGroup );
        break;
        }
    default:
        return false;
    }

    return true;
}

void MonsterRegion::load_attributes(CL_DomNamedNodeMap attr)
{
    m_LevelX = get_required_int("levelX", attr);
    m_LevelY = get_required_int("levelY", attr);
    m_Width = get_required_int("width", attr);
    m_Height = get_required_int("height", attr);
    m_backdrop = get_required_string("backdrop", attr);

    //TODO: Make sure this backdrop exists.

    if(has_attribute("encounterRate", attr))
    {
        m_encounter_rate = get_float("encounterRate", attr);
    }
    else
    {
        //TODO: Get game wide default encounter rate.
    }


}

void MonsterRegion::load_finished()
{
    //if monster groups is empty throw an error
    if(m_monster_groups.empty())
    {
        throw CL_Exception("Didn't provide a MonsterGroup");
    }

}

MonsterGroup * MonsterRegion::GetMonsterGroup() const
{
    //Return a semi random monster group based on weights

    //you add up all the weights of all the groups in the region,
    //and thats your total, and then your chance for each is the
    //weight of that one divided by the total weight....

    int n = (int)(rand() / (((double)RAND_MAX + 1)/ m_nTotalWeight));
    int totalSoFar = 0;

    for(std::list<MonsterGroup*>::const_iterator it = m_monster_groups.begin();
        it != m_monster_groups.end(); it++)
    {
        MonsterGroup * pGroup = *it;
        totalSoFar += pGroup->GetEncounterWeight();

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


MonsterRegion * MonsterRegions::GetApplicableRegion(uint x, uint y) const
{
    for(std::list<MonsterRegion*>::const_iterator it = m_monster_regions.begin();
        it != m_monster_regions.end();
        it++)
    {
        MonsterRegion * pRegion = *it;
        if(x >= pRegion->GetLevelX() && x <= pRegion->GetLevelX() + pRegion->GetWidth()
            && y >= pRegion->GetLevelY() && y <= pRegion->GetLevelY() + pRegion->GetHeight())
        {
            return pRegion;
        }
    }

    return NULL;
}


bool MonsterRegions::handle_element(StoneRing::Element::eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case EMONSTERREGION:
        {
            MonsterRegion *pRegion = dynamic_cast<MonsterRegion*>(pElement);
            assert(pRegion);
            m_monster_regions.push_back ( pRegion);
            return true;
        }
    default:
        return false;
    }
}

void MonsterRegions::load_finished()
{
}


#include "MonsterRegion.h"
#include "MonsterGroup.h"
#include <cassert>

using StoneRing::MonsterRegion;
using StoneRing::MonsterRegions;
using StoneRing::MonsterGroup;


MonsterRegion::MonsterRegion():m_nTotalWeight(0)
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

void MonsterRegion::load_attributes(clan::DomNamedNodeMap attr)
{
    m_backdrop = get_required_string("backdrop", attr);
	m_id = get_required_int("id",attr);

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
        throw XMLException("Didn't provide a MonsterGroup");
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




bool MonsterRegions::handle_element(StoneRing::Element::eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case EMONSTERREGION:
        {
            MonsterRegion *pRegion = dynamic_cast<MonsterRegion*>(pElement);
            assert(pRegion);
            m_monster_regions[pRegion->GetId()] =  pRegion;
            return true;
        }
    default:
        return false;
    }
}

void MonsterRegions::load_finished()
{
}

#if SR2_EDITOR
clan::DomElement MonsterRegion::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement element(doc,"monsterRegion");
    element.set_attribute("backdrop",m_backdrop);
    element.set_attribute("encounterRate",FloatToString(m_encounter_rate));
	element.set_attribute("id",IntToString(m_id));

    for(std::list<MonsterGroup*>::const_iterator it = m_monster_groups.begin();
        it != m_monster_groups.end(); it++) {
            element.append_child((*it)->CreateDomElement(doc));
    }
    return element;
}
clan::DomElement MonsterRegions::CreateDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"monsterRegions");
    for(std::map<char,MonsterRegion*>::const_iterator it = m_monster_regions.begin();
        it != m_monster_regions.end(); it++){
        element.append_child((it->second)->CreateDomElement(doc));
    }
    return element;
}

char MonsterRegions::get_next_id() const
{
	char id = 0;
	while(0 != m_monster_regions.count(id)){
		++id;
	}
	return id;
}

#endif


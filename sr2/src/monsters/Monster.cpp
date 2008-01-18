#include "Monster.h"
#include "IApplication.h"

using namespace StoneRing;

Monster::Monster()
{
}

Monster::~Monster()
{
}

std::list<ItemRef*> Monster::getDrops() const
{
    return mItems;
}

/** Element Stuff **/
bool Monster::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESTAT:
        {
            Stat * pStat = dynamic_cast<Stat*>(pElement);
            mStatMap[pStat->getAttribute()] = pStat;
            break;
        }
    case EITEMREF:
        mItems.push_back ( dynamic_cast<ItemRef*>(pElement) );
        break;
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

void Monster::loadAttributes(CL_DomNamedNodeMap *pAttr)
{
    mName = getRequiredString("name",pAttr);
    mSpriteResources = getRequiredString("spriteResources",pAttr);
    std::string mode = getRequiredString("mode",pAttr);
    mnLevel = getRequiredInt("level",pAttr);
    std::string type = getImpliedString("type",pAttr,"living");

    if(mode == "manual")
    {
        mbClass = false;
    }
    else if(mode=="class")
    {
        mbClass = true;
    }
    else throw CL_Error("Unknown monster mode");

    if(type == "living")
        meType = LIVING;
    else if(type == "nonliving")
        meType = NONLIVING;
    else if(type == "magical")
        meType = MAGICAL;

    if(mbClass)
    {
        std::string classname = getRequiredString("class",pAttr);
        mpClass = IApplication::getInstance()->getAbilityManager()->getClass(classname);
    }

}

void Monster::handleText(const std::string &)
{
}

void Monster::loadFinished()
{
    // Check for all the required attributes
    if(!mbClass)
    {
    } 

}

ICharacter::eGender Monster::getGender() const
{
    return NEUTER;
}

std::string Monster::getName() const
{
    return "";
}


// For boolean values.
void Monster::fixAttribute(eCharacterAttribute attr, bool state)
{
}


double Monster::getSpellResistance(Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::getAttribute(eCharacterAttribute attr) const 
{
    return 1.0;
}


bool Monster::getToggle(eCharacterAttribute attr) const
{
    return false;
}

void Monster::fixAttribute(eCharacterAttribute attr, double value)
{
    mAttributes.fixAttribute(attr,value);
}

void Monster::attachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.attachMultiplication(attr,factor);
}

void Monster::attachAddition(eCharacterAttribute attr, double value) 
{
    mAttributes.attachAddition(attr,value);
}

void Monster::detachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.detachMultiplication(attr,factor);
}

void Monster::detachAddition(eCharacterAttribute attr, double value) 
{
    mAttributes.detachAddition(attr,value);
}


void Monster::addStatusEffect(StatusEffect *pEffect)
{
    mStatusEffects.insert(StatusEffectMap::value_type(pEffect->getName(),pEffect));
}

void Monster::removeEffects(const std::string &name)
{
    StatusEffectMap::iterator start = mStatusEffects.lower_bound(name);
    StatusEffectMap::iterator end   = mStatusEffects.upper_bound(name);

    mStatusEffects.erase(start,end);
}

void Monster::statusEffectRound()
{
}


ICharacter::eType Monster::getType() const
{
    return meType;
}

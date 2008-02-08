#include "Monster.h"
#include "IApplication.h"
#include "CharacterManager.h"

using namespace StoneRing;

Monster::Monster():mpOnInvoke(NULL),mpOnRound(NULL),mpOnRemove(NULL),mnLevel(1)
{
}

Monster::~Monster()
{
    delete mpOnRound;
    delete mpOnInvoke;
    delete mpOnRemove;
}

void Monster::invoke()
{
    if(mpOnInvoke)
        mpOnInvoke->executeScript();
}
void Monster::round()
{
    if(mpOnRound)
        mpOnRound->executeScript();
}
void Monster::die()
{
    if(mpOnRemove)
        mpOnRemove->executeScript();
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
    case EONROUND:
        mpOnRound = dynamic_cast<NamedScript*>(pElement);
        assert(mpOnRound != NULL);
        break;
    case EONINVOKE:
        mpOnInvoke = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONREMOVE:
        mpOnRemove = dynamic_cast<NamedScript*>(pElement);
        break;
    case ESPRITEDEFINITION:
        {
            SpriteDefinition * pSpriteDef = dynamic_cast<SpriteDefinition*>(pElement);
            mSpriteDefinitionsMap[pSpriteDef->getName()] = pSpriteDef;
            break;
        }
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
        mpClass = IApplication::getInstance()->getCharacterManager()->getClass(classname);
    }

}

void Monster::handleText(const std::string &)
{
}

void Monster::loadFinished()
{
    if(!mpOnRound) throw CL_Error("Missing onRound on monster " + mName);

    // Check for all the required stats
    if(!mbClass)
    {
    } 
#ifndef NDEBUG
    std::cout << '\t' << mName << std::endl;
#endif

}

ICharacter::eGender Monster::getGender() const
{
    return NEUTER;
}

std::string Monster::getName() const
{
    return mName;
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

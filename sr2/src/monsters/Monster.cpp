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
    if(mode == "manual")
    {
        mbClass = false;
    }
    else if(mode=="class")
    {
        mbClass = true;
    }
    else throw CL_Error("Unknown monster mode");

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
void Monster::attributeMultiply(eCharacterAttribute attr, double mult)
{
}

void Monster::attributeAdd(eCharacterAttribute attr, double add)
{
}

// For boolean values.
void Monster::toggleAttribute(eCharacterAttribute attr, bool state)
{
}

double Monster::getMaxAttribute(eCharacterAttribute attr) const 
{
    return 100.0;
}

double Monster::getMinAttribute(eCharacterAttribute attr) const
{
    return 0.0;
}

double Monster::getSpellResistance(Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::getAttribute(eCharacterAttribute attr) const 
{
    return 1.0;
}

int Monster::getAttributeInt(eCharacterAttribute attr) const
{
    return 0;
}

bool Monster::getToggle(eCharacterAttribute attr) const
{
    return false;
}
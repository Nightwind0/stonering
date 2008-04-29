#include "MonsterElement.h"
#include "IApplication.h"
#include "CharacterManager.h"
#include "NamedScript.h"
#include "Stat.h"
#include "SpriteDefinition.h"

using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::SpriteDefinition;

MonsterElement::MonsterElement()
:mpOnInvoke(NULL),mpOnRound(NULL),mpOnRemove(NULL),mnLevel(1)
{
}

MonsterElement::~MonsterElement()
{
    delete mpOnRound;
    delete mpOnInvoke;
    delete mpOnRemove;
}

void MonsterElement::invoke()
{
    if(mpOnInvoke)
        mpOnInvoke->executeScript();
}
void MonsterElement::round()
{
    if(mpOnRound)
        mpOnRound->executeScript();
}
void MonsterElement::die()
{
    if(mpOnRemove)
        mpOnRemove->executeScript();
}

std::list<ItemRef*>::const_iterator MonsterElement::getItemRefsBegin() const
{
    return mItems.begin();
}

std::list<ItemRef*>::const_iterator MonsterElement::getItemRefsEnd() const
{
    return mItems.end();
}

#if 0
SpriteDefinition * MonsterElement::getSpriteDefinition(const std::string &name) const
{
    // Throw error if missing?
    std::map<std::string,SpriteDefinition*>::const_iterator it = mSpriteDefinitionsMap.find(name);

    if(it == mSpriteDefinitionsMap.end()) throw CL_Error("Sprite definition: " + name + " missing");
    return it->second;
}
#endif

/** Element Stuff **/
bool MonsterElement::handleElement(eElement element, Element * pElement)
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

void MonsterElement::loadAttributes(CL_DomNamedNodeMap *pAttr)
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
        meType = ICharacter::LIVING;
    else if(type == "nonliving")
        meType = ICharacter::NONLIVING;
    else if(type == "magical")
        meType = ICharacter::MAGICAL;

    if(mbClass)
    {
        std::string classname = getRequiredString("class",pAttr);
        mpClass = IApplication::getInstance()->getCharacterManager()->getClass(classname);
    }

}

void MonsterElement::handleText(const std::string &)
{
}

void MonsterElement::loadFinished()
{
    if(!mpOnRound) throw CL_Error("Missing onRound on monster " + mName);

    // Check for all the required stats
    if(!mbClass)
    {
    } 

    // TODO: Make sure the required sprites are available
#ifndef NDEBUG
    std::cout << '\t' << mName << std::endl;
#endif

}

std::string MonsterElement::getName() const
{
    return mName;
}

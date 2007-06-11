#include "Monster.h"

using namespace StoneRing;

Monster::Monster()
{
}

Monster::~Monster()
{
}

/** Element Stuff **/
bool Monster::handleElement(eElement, Element * )
{
    return true;
}

void Monster::loadAttributes(CL_DomNamedNodeMap *)
{
}

void Monster::handleText(const std::string &)
{
}

void Monster::loadFinished()
{
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
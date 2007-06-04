
#include "IApplication.h"
#include "GraphicsManager.h"

#include "Level.h"
#include "Animation.h"
#include "Character.h"

using namespace StoneRing;



ICharacter::eGender Character::getGender() const
{
    return NEUTER;
}

void Character::attributeMultiply(eCharacterAttribute attr, double mult)
{
}

void Character::attributeAdd(eCharacterAttribute attr, double add)
{
}

// For boolean values.
void Character::toggleAttribute(eCharacterAttribute attr, bool state)
{
}

double Character::getMaxAttribute(eCharacterAttribute attr) const
{
    return 0.0;
}

double Character::getMinAttribute(eCharacterAttribute attr) const
{
    return 0.0;
}
double Character::getAttribute(eCharacterAttribute attr) const
{
    return 0.0;
}

bool Character::getToggle(eCharacterAttribute attr) const
{
    return false;
}

int Character::getAttributeInt(eCharacterAttribute attr) const
{
    return 0;
}





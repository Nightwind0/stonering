
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"
#include "Animation.h"
#include "Character.h"

using namespace StoneRing;



void StoneRing::Character::modifyAttribute(eCharacterAttribute attr, int add, float multiplier){}
int StoneRing::Character::getMaxAttribute(eCharacterAttribute attr) const { return 1; }
int StoneRing::Character::getMinAttribute(eCharacterAttribute attr) const { return 1; }
int StoneRing::Character::getAttribute(eCharacterAttribute attr) const{ return 1;}



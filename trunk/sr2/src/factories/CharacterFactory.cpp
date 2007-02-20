 
#include "CharacterFactory.h"
#include "CharacterClass.h"
#include "CharacterDefinition.h"

using namespace StoneRing;


bool CharacterFactory::canCreate( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    if(method == NULL) return false;
    else return true;
}

Element * CharacterFactory::createElement( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    Element * pElement = (this->*method)();

    return pElement;
}

CharacterFactory::factoryMethod CharacterFactory::getMethod(Element::eElement element) const
{
    switch(element)
    {
    case Element::ECHARACTERCLASS:
        return &CharacterFactory::createCharacterClass;
    case Element::EANIMATIONDEFINITION:
        return &CharacterFactory::createAnimationDefinition;
    case Element::ECHARACTER:
        return &CharacterFactory::createCharacterDefinition;
    case Element::EWEAPONTYPESPRITE:
        return &CharacterFactory::createWeaponTypeSprite;
    default:
        return NULL;
    }
}


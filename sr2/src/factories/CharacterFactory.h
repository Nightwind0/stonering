#ifndef SR_CHARACTER_FACTORY_H
#define SR_CHARACTER_FACTORY_H

#include "IFactory.h"

#include <ClanLib/core.h>

namespace StoneRing
{


    class CharacterFactory : public IFactory
	{
	public:
	    CharacterFactory(){}
	    virtual ~CharacterFactory(){}

	    virtual bool canCreate( Element::eElement element );
	    virtual Element * createElement( Element::eElement element );
	private:
	    typedef Element * (CharacterFactory::*factoryMethod)() const;
		Element * createCharacterClass() const;
		Element * createAnimationDefinition() const;
		Element * createCharacterDefinition() const;
		Element * createWeaponTypeSprite() const;

	    factoryMethod getMethod(Element::eElement element) const;
	};
};

#endif


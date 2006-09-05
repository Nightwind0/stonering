#include "Rune.h"
#include "SpellRef.h"

using namespace StoneRing;

Rune::Rune():mpSpellRef(NULL)
{
}

Rune::~Rune()
{
	delete mpSpellRef;
}


uint Rune::getValue() const 
{
	//@todo: When we have spells implemented, we will have to look up their value here.
	return 0;
}

uint Rune::getSellValue() const 
{
	//@todo: When we have spells implemented, we will have to look up their value here.
	return 0;
}

SpellRef * Rune::getSpellRef() const
{
	return mpSpellRef;
}

void Rune::loadAttributes(CL_DomNamedNodeMap*)
{
}

bool Rune::handleElement(eElement element, Element * pElement)
{
	if(element == ESPELLREF)
	{
		mpSpellRef = dynamic_cast<SpellRef*>(pElement);
		return true;
	}
	else return false;
}

void Rune::loadFinished()
{
	if(!mpSpellRef) throw CL_Error("Rune without spellref.");
}

CL_DomElement  
Rune::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"rune");
}


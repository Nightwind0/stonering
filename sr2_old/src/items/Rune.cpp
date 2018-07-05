#include "Rune.h"


using namespace StoneRing;

Rune::Rune()
{
}

Rune::~Rune()
{
}


uint Rune::GetValue() const
{
    //@todo: When we have spells implemented, we will have to look up their value here.
    return 0;
}

uint Rune::GetSellValue() const
{
    //@todo: When we have spells implemented, we will have to look up their value here.
    return 0;
}


bool Rune::handle_element(Element::eElement element, Element * pElement)
{
    NamedItemElement::handle_element(element,pElement);

    return false;
}

void Rune::load_finished()
{
    NamedItemElement::load_finished();
}

bool Rune::operator==(const StoneRing::ItemRef& ref)
{
    if(ref.GetType() == ItemRef::NAMED_ITEM &&
	ref.GetItemName() == GetName())
	return true;
    return false;
}




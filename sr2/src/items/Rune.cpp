#include "Rune.h"
#include "SpellRef.h"

using namespace StoneRing;

Rune::Rune():m_pSpellRef(NULL)
{
}

Rune::~Rune()
{
    delete m_pSpellRef;
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

SpellRef * Rune::GetSpellRef() const
{
    return m_pSpellRef;
}

bool Rune::handle_element(Element::eElement element, Element * pElement)
{
    NamedItemElement::handle_element(element,pElement);
    if(element == ESPELLREF)
    {
        m_pSpellRef = dynamic_cast<SpellRef*>(pElement);
        return true;
    }
    else return false;
}

void Rune::load_finished()
{
    NamedItemElement::load_finished();
    if(!m_pSpellRef) throw CL_Exception("Rune without spellref.");
}

bool Rune::operator==(const StoneRing::ItemRef& ref)
{
    if(ref.GetType() == ItemRef::NAMED_ITEM &&
	ref.GetItemName() == GetName())
	return true;
    return false;
}




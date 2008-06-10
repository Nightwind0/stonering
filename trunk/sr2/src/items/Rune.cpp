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

void Rune::load_attributes(CL_DomNamedNodeMap*)
{
}

bool Rune::handle_element(Element::eElement element, Element * pElement)
{
    if(element == ESPELLREF)
    {
        m_pSpellRef = dynamic_cast<SpellRef*>(pElement);
        return true;
    }
    else return false;
}

void Rune::load_finished()
{
    if(!m_pSpellRef) throw CL_Error("Rune without spellref.");
}






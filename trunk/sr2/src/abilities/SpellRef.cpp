#include "SpellRef.h"

using namespace StoneRing;

void SpellRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string spellType = getRequiredString("type",pAttributes);

    if(spellType == "elemental")
        meSpellType = ELEMENTAL;
    else if (spellType == "white")
        meSpellType = WHITE;
    else if (spellType == "status")
        meSpellType = STATUS;
    else if (spellType == "other")
        meSpellType = OTHER;
    else throw CL_Error("Bad spell type in spell ref.");

}

void SpellRef::handleText(const std::string &text)
{
    mName = text;
}

SpellRef::SpellRef(  )
{
}

SpellRef::~SpellRef()
{
}


bool SpellRef::operator==(const SpellRef &lhs )
{
    if ( meSpellType == lhs.meSpellType &&
         mName == lhs.mName )
    {
        return true;
    }
    else return false;
}


SpellRef::eSpellType SpellRef::getSpellType() const
{
    return meSpellType;
}

std::string SpellRef::getName() const
{
    return mName;
}

CL_DomElement 
SpellRef::createDomElement ( CL_DomDocument &doc) const
{

    CL_DomElement element(doc,"spellRef");

    std::string spellType;

    switch(meSpellType)
    {
    case ELEMENTAL:
        spellType = "elemental";
        break;
    case WHITE:
        spellType = "white";
        break;
    case STATUS:
        spellType = "status";
        break;
    case OTHER:
        spellType = "other";
        break;
    }

    element.set_attribute("type", spellType );

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;

}





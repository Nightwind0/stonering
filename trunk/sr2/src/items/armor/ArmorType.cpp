#include "ArmorType.h"


using namespace StoneRing;


ArmorType::ArmorType()
{
}


bool ArmorType::operator==(const ArmorType &lhs )
{
    return mName == lhs.mName;
}

void ArmorType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnBasePrice = getRequiredInt("basePrice",pAttributes);
    mnBaseAC = getRequiredInt("baseArmorClass",pAttributes);
    mnBaseRST = getRequiredInt("baseResist",pAttributes);

    std::string slot = getRequiredString("slot",pAttributes);

    if(slot == "head")
        meSlot = HEAD;
    else if (slot == "shield")
        meSlot = SHIELD;
    else if (slot == "body")
        meSlot = BODY;
    else if (slot == "feet")
        meSlot = FEET;
    else if (slot == "hands")
        meSlot = HANDS;

}

bool ArmorType::handleElement(eElement element, Element * pElement)
{
    if(element == EICONREF)
    {
        //@todo
        return true;
    }
    else return false;
}

ArmorType::~ArmorType()
{
}


std::string 
ArmorType::getName() const
{
    return mName;
}

std::string ArmorType::getIconRef() const
{
    return mIconRef;
}

uint ArmorType::getBasePrice() const
{
    return mnBasePrice;
}

int ArmorType::getBaseAC() const
{
    return mnBaseAC;
}

int ArmorType::getBaseRST() const
{
    return mnBaseRST;
}


ArmorType::eSlot ArmorType::getSlot() const
{
    return meSlot;
}





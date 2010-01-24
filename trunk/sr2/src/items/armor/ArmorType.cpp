#include "ArmorType.h"


using namespace StoneRing;


ArmorType::ArmorType()
{
}


bool ArmorType::operator==(const ArmorType &lhs )
{
    return m_name == lhs.m_name;
}

void ArmorType::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nBasePrice = get_required_int("basePrice",attributes);
    m_nBaseAC = get_required_int("baseArmorClass",attributes);
    m_nBaseRST = get_required_int("baseResist",attributes);

    std::string slot = get_required_string("slot",attributes);

    if(slot == "head")
        m_eSlot = Equipment::EHEAD;
    else if (slot == "shield")
        m_eSlot = Equipment::EOFFHAND;
    else if (slot == "body")
        m_eSlot = Equipment::EBODY;
    else if (slot == "feet")
        m_eSlot = Equipment::EFEET;
    else if (slot == "hands")
        m_eSlot = Equipment::EHANDS;
    else if (slot == "finger")
        m_eSlot = Equipment::EANYFINGER;

}

bool ArmorType::handle_element(eElement element, Element * pElement)
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
ArmorType::GetName() const
{
    return m_name;
}

std::string ArmorType::GetIconRef() const
{
    return m_icon_ref;
}

uint ArmorType::GetBasePrice() const
{
    return m_nBasePrice;
}

int ArmorType::GetBaseAC() const
{
    return m_nBaseAC;
}

int ArmorType::GetBaseRST() const
{
    return m_nBaseRST;
}


Equipment::eSlot ArmorType::GetSlot() const
{
    return m_eSlot;
}





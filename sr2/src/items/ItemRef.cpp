#include "ItemRef.h"
#include "WeaponRef.h"
#include "ArmorRef.h"
#include "ItemManager.h"




bool StoneRing::operator<(const StoneRing::ItemRef &lhs,const StoneRing::ItemRef &rhs)
{
    return lhs.GetItemName() < rhs.GetItemName();
}

bool StoneRing::ItemRef::handle_element(eElement element, StoneRing::Element * pElement )
{
    switch(element)
    {
    case ENAMEDITEMREF:
        m_eType = NAMED_ITEM;
        m_ref.mpNamedItemRef = dynamic_cast<NamedItemRef*>(pElement);
        break;
    case EWEAPONREF:
        m_eType = WEAPON_REF;
        m_ref.mpWeaponRef = dynamic_cast<WeaponRef*>(pElement);
        break;
    case EARMORREF:
        m_eType = ARMOR_REF;
        m_ref.mpArmorRef = dynamic_cast<ArmorRef*>(pElement);
        break;
    default:

        return false;
    }

    return true;
}

void StoneRing::ItemRef::load_attributes(CL_DomNamedNodeMap attributes)
{

}

void StoneRing::ItemRef::load_finished()
{

    if(m_eType == INVALID)
    {
        throw CL_Exception("Item Ref with no child");
    }

    m_pItem = ItemManager::GetItem ( *this );

}


StoneRing::ItemRef::ItemRef( ):m_eType(INVALID)
{
    memset(&m_ref,0,sizeof(m_ref));
}


StoneRing::ItemRef::~ItemRef()
{

}

std::string StoneRing::ItemRef::GetItemName() const
{
    switch ( m_eType )
    {
    case NAMED_ITEM:
        return m_ref.mpNamedItemRef->GetItemName();
    case WEAPON_REF:
        return m_ref.mpWeaponRef->GetName();
    case ARMOR_REF:
        return m_ref.mpArmorRef->GetName();
    default:
        assert(0);
        return "";
    }
}

StoneRing::ItemRef::eRefType StoneRing::ItemRef::GetType() const
{
    return m_eType;
}

StoneRing::NamedItemRef * StoneRing::ItemRef::GetNamedItemRef() const
{
    assert(m_eType == NAMED_ITEM);
    return m_ref.mpNamedItemRef;
}

StoneRing::WeaponRef * StoneRing::ItemRef::GetWeaponRef() const
{
    assert(m_eType == WEAPON_REF);
    return m_ref.mpWeaponRef;
}

StoneRing::ArmorRef * StoneRing::ItemRef::GetArmorRef() const
{
    assert(m_eType == ARMOR_REF);
    return m_ref.mpArmorRef;
}



StoneRing::NamedItemRef::NamedItemRef()
{
}

StoneRing::NamedItemRef::~NamedItemRef()
{
}


std::string StoneRing::NamedItemRef::GetItemName()
{
    return m_name;
}


void StoneRing::NamedItemRef::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
}


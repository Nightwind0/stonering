#include "WeaponRef.h"
#include "IApplication.h"
#include "WeaponTypeRef.h"
#include "WeaponClassRef.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "ItemManager.h"

using namespace StoneRing;

WeaponRef::WeaponRef():m_pWeaponType(NULL), m_pWeaponClass(NULL),
                       m_pSpellRef(NULL),m_pRuneType(NULL)
{
}

std::string WeaponRef::GetName() const
{
    return m_name;
}

bool WeaponRef::handle_element(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::GetInstance()->GetItemManager();

    switch(element)
    {
    case EWEAPONTYPEREF:
        m_pType =  dynamic_cast<WeaponTypeRef*>(pElement);
        m_pWeaponType = pItemManager->GetWeaponType(*m_pType);
        break;
    case EWEAPONCLASSREF:
        m_pClass = dynamic_cast<WeaponClassRef*>(pElement);
        m_pWeaponClass = pItemManager->GetWeaponClass ( *m_pClass );
        break;
    case ESPELLREF:
        m_pSpellRef = dynamic_cast<SpellRef*>(pElement);
        break;
    case ERUNETYPE:
        m_pRuneType = dynamic_cast<RuneType*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

WeaponRef::~WeaponRef()
{
}


WeaponRef::WeaponRef ( WeaponType *pType, WeaponClass *pClass, 
                       SpellRef * pSpell, RuneType *pRune )
                       :m_pWeaponType(pType), m_pWeaponClass(pClass),
                       m_pSpellRef(pSpell), m_pRuneType(pRune)

{

}



void WeaponRef::load_finished()
{
    m_name = Weapon::CreateWeaponName(m_pWeaponType,m_pWeaponClass,m_pSpellRef,m_pRuneType);
}


WeaponType * 
WeaponRef::GetWeaponType() const
{
    return m_pWeaponType;
}

WeaponClass * WeaponRef::GetWeaponClass() const
{
    return m_pWeaponClass;
}

SpellRef * WeaponRef::GetSpellRef() const
{
    return m_pSpellRef;
}

RuneType * WeaponRef::GetRuneType() const
{
    return m_pRuneType;
}






#include "ArmorRef.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "ItemManager.h"
#include "IApplication.h"



using namespace StoneRing;

ArmorRef::ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
                     SpellRef * pSpell, RuneType *pRune )
                     :m_pArmorType(pType), m_pArmorClass(pClass),
                     m_pSpellRef(pSpell), m_pRuneType(pRune)

{

}

std::string ArmorRef::GetName() const
{
    return  m_name;
}

bool ArmorRef::handle_element(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::GetInstance()->GetItemManager();

    switch(element)
    {
    case EARMORTYPEREF:
        m_pType = (dynamic_cast<ArmorTypeRef*>(pElement));
        m_pArmorType = pItemManager->GetArmorType(*m_pType);
        break;
    case EARMORCLASSREF:
        m_pClass =  (dynamic_cast<ArmorClassRef*>(pElement));
        m_pArmorClass = pItemManager->GetArmorClass (*m_pClass);
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

void ArmorRef::load_finished()
{
    m_name = Armor::CreateArmorName(m_pArmorType,m_pArmorClass,m_pSpellRef,m_pRuneType);
}

ArmorRef::ArmorRef():m_pArmorType(NULL),
                     m_pArmorClass(NULL),
                     m_pSpellRef(NULL),
                     m_pRuneType(NULL)
{

}

ArmorRef::~ArmorRef()
{
}

ArmorType * ArmorRef::GetArmorType() const
{
    return m_pArmorType;
}

ArmorClass * ArmorRef::GetArmorClass() const
{
    return m_pArmorClass;
}

SpellRef * ArmorRef::GetSpellRef() const
{
    return m_pSpellRef;
}

RuneType * ArmorRef::GetRuneType() const
{
    return m_pRuneType;
}






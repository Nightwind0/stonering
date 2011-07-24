
#include "ArmorRef.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "RuneType.h"
#include "ItemManager.h"
#include "IApplication.h"



using namespace StoneRing;

ArmorRef::ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
                     ArmorClass *pImbuement, RuneType *pRune )
                     :m_pArmorType(pType), m_pArmorClass(pClass),
                     m_pImbuement(pImbuement), m_pRuneType(pRune)

{

}

std::string ArmorRef::GetName() const
{
    return  m_name;
}

bool ArmorRef::handle_element(eElement element, Element * pElement)
{

    switch(element)
    {
    case EARMORTYPEREF:
        m_pType = (dynamic_cast<ArmorTypeRef*>(pElement));
        m_pArmorType = ItemManager::GetArmorType(*m_pType);
        break;
    case EARMORCLASSREF:
        m_pClass =  (dynamic_cast<ArmorClassRef*>(pElement));
        m_pArmorClass = ItemManager::GetArmorClass (*m_pClass);
        break;
    case EARMORIMBUEMENTREF:
        m_pImbuement = ItemManager::GetArmorImbuement(* dynamic_cast<ArmorImbuementRef*>(pElement));
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
    m_name = Armor::CreateArmorName(m_pArmorType,m_pArmorClass,m_pImbuement,m_pRuneType);
}

ArmorRef::ArmorRef():m_pArmorType(NULL),
                     m_pArmorClass(NULL),
                     m_pImbuement(NULL),
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

ArmorClass * ArmorRef::GetArmorImbuement() const
{
    return m_pImbuement;
}

RuneType * ArmorRef::GetRuneType() const
{
    return m_pRuneType;
}






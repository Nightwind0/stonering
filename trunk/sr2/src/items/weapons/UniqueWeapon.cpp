#include "UniqueWeapon.h"
#include "WeaponType.h"
#include "ItemManager.h"
#include "WeaponTypeRef.h"
#include "WeaponEnhancer.h"
#include "AttributeEnhancer.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;

UniqueWeapon::UniqueWeapon():mpWeaponType(NULL),mpScript(NULL)
{
}

UniqueWeapon::~UniqueWeapon()
{ 
    delete mpScript;
    delete mpEquipScript;
    delete mpUnequipScript;
    delete mpConditionScript;
}

void UniqueWeapon::executeScript()
{
    if(mpScript) mpScript->executeScript();
}

bool UniqueWeapon::equipCondition()
{
    if(mpConditionScript)
        return mpConditionScript->evaluateCondition();
    else return true;
}

void UniqueWeapon::onEquipScript()
{
    mpEquipScript->executeScript();
}

void UniqueWeapon::onUnequipScript()
{
    mpUnequipScript->executeScript();
}

uint UniqueWeapon::getValue() const 
{
    return mnValue;
}

uint UniqueWeapon::getSellValue() const 
{
    return mnValue / 2;
}

CL_DomElement  
UniqueWeapon::createDomElement(CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"uniqueWeapon");
}



WeaponType * UniqueWeapon::getWeaponType() const 
{
    return mpWeaponType;
}

bool UniqueWeapon::isRanged() const 
{
    return mpWeaponType->isRanged();
}

bool UniqueWeapon::isTwoHanded() const
{
    return mpWeaponType->isTwoHanded();
}

void UniqueWeapon::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);

}

void UniqueWeapon::loadFinished()
{
    cl_assert ( mpWeaponType );
    mnValue = (int)(mpWeaponType->getBasePrice() * mValueMultiplier);
}
bool UniqueWeapon::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EWEAPONTYPEREF:
    {
        const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

        WeaponTypeRef * pType = dynamic_cast<WeaponTypeRef*>(pElement);
        mpWeaponType = pItemManager->getWeaponType( *pType );
        break;
    }
    case EWEAPONENHANCER:
        addWeaponEnhancer( dynamic_cast<WeaponEnhancer*>(pElement) );
        break;
    case EATTRIBUTEENHANCER:
        addAttributeEnhancer( dynamic_cast<AttributeEnhancer*>(pElement) );
        break;
    case ESPELLREF:
        setSpellRef ( dynamic_cast<SpellRef*>(pElement) );
        break;
    case ERUNETYPE:
        setRuneType( dynamic_cast<RuneType*>(pElement) );
        break;
    case ESTATUSEFFECTMODIFIER:
        addStatusEffectModifier( dynamic_cast<StatusEffectModifier*>(pElement) );
        break;
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONEQUIP:
        mpEquipScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONUNEQUIP:
        mpUnequipScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        mpConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;

    }

    return true;
}



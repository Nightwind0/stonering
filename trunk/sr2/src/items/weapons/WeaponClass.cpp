#include "WeaponClass.h"
#include "AttributeEnhancer.h"
#include "WeaponClass.h"
#include "WeaponEnhancer.h"
#include "StatusEffectModifier.h"
#include "WeaponTypeExclusionList.h"

using namespace StoneRing;

WeaponClass::WeaponClass():mpScript(NULL),mpEquipScript(NULL),
mpUnequipScript(NULL),mpConditionScript(NULL)
{
}

void WeaponClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes );
    mfValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);
    mnValueAdd = getImpliedInt("valueAdd",pAttributes,0);
}

bool WeaponClass::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONUNEQUIP:
        mpUnequipScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONEQUIP:
        mpEquipScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        mpConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EATTRIBUTEENHANCER:
        mAttributeEnhancers.push_back( dynamic_cast<AttributeEnhancer*>(pElement) );
        break;
    case EWEAPONENHANCER:
        mWeaponEnhancers.push_back( dynamic_cast<WeaponEnhancer*>(pElement) );
        break;
    case EWEAPONTYPEEXCLUSIONLIST:
    {
        WeaponTypeExclusionList * pList = dynamic_cast<WeaponTypeExclusionList*>(pElement);
        std::copy(pList->getWeaponTypeRefsBegin(),pList->getWeaponTypeRefsEnd(), 
                  std::back_inserter(mExcludedTypes));

        delete pList;
        break;
    }
    case ESTATUSEFFECTMODIFIER:
        addStatusEffectModifier (dynamic_cast<StatusEffectModifier*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

void WeaponClass::executeScript()
{
    if(mpScript) mpScript->executeScript();
}

bool WeaponClass::equipCondition()
{
    if(mpConditionScript)
        return mpConditionScript->evaluateCondition();
    else return true;
}

void WeaponClass::onEquipScript()
{
    mpEquipScript->executeScript();
}

void WeaponClass::onUnequipScript()
{
    mpUnequipScript->executeScript(); 
}

WeaponClass::~WeaponClass()
{
    std::for_each(mAttributeEnhancers.begin(),mAttributeEnhancers.end(),del_fun<AttributeEnhancer>());
    std::for_each(mWeaponEnhancers.begin(),mWeaponEnhancers.end(),del_fun<WeaponEnhancer>());
    std::for_each(mExcludedTypes.begin(),mExcludedTypes.end(),del_fun<WeaponTypeRef>());

    delete mpScript;
    delete mpUnequipScript;
    delete mpEquipScript;
    delete mpConditionScript;
}

bool WeaponClass::operator==(const WeaponClass &lhs)
{
    return mName == lhs.mName;
}

std::string WeaponClass::getName() const
{
    return mName;
}

int WeaponClass::getValueAdd() const
{
    return mnValueAdd;
}

float WeaponClass::getValueMultiplier() const
{
    return mfValueMultiplier;
}

std::list<AttributeEnhancer*>::const_iterator 
WeaponClass::getAttributeEnhancersBegin()
{
    return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator 
WeaponClass::getAttributeEnhancersEnd()
{
    return mAttributeEnhancers.end();
}

std::list<WeaponEnhancer*>::const_iterator 
WeaponClass::getWeaponEnhancersBegin()
{
    return mWeaponEnhancers.begin();
}

std::list<WeaponEnhancer*>::const_iterator 
WeaponClass::getWeaponEnhancersEnd()
{
    return mWeaponEnhancers.end();
}

bool WeaponClass::isExcluded ( const WeaponTypeRef &weaponType )
{
    for(std::list<WeaponTypeRef*>::const_iterator iter = mExcludedTypes.begin();
        iter != mExcludedTypes.end();
        iter++)
    {
        WeaponTypeRef * pRef = *iter;

        if( pRef->getName() == weaponType.getName() ) 
            return true;
    }

    return false;

}



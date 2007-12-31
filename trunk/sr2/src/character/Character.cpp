
#include "IApplication.h"
#include "GraphicsManager.h"

#include "Level.h"
#include "Animation.h"
#include "Character.h"
#include "StatusEffect.h"
#include <functional>
using namespace StoneRing;



void AttributeFile::assert_real(eCharacterAttribute attr)
{
    if (attr != CA_INVALID && attr < _START_OF_TOGGLES)
        return;
    throw CL_Error("Attribute was not a real value");
}

void AttributeFile::assert_bool(eCharacterAttribute attr)
{
    if(attr > _START_OF_TOGGLES && attr < _END_OF_TOGGLES)
        return;
    throw CL_Error("Attribute was not a real value");
}

AttributeFile::attr_doubles::const_iterator 
AttributeFile::find_attr(eCharacterAttribute attr) const
{
    assert_real(attr);
    return mRealAttributes.find(attr);
}

double 
AttributeFile::find_multiplier(eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator iter = mAttrMultipliers.find(attr);
    if(iter != mAttrMultipliers.end())
    {
        return iter->second; 
    }
    else return 1.0;
}

double
AttributeFile::find_addition(eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator iter = mAttrAdditions.find(attr);
    if(iter != mAttrAdditions.end())
    {
        return iter->second; 
    }
    else return 0;
}

AttributeFile::attr_bools::const_iterator 
AttributeFile::find_toggle(eCharacterAttribute attr)const
{
    assert_bool(attr);
    return mToggles.find(attr);
}

double AttributeFile::getAttribute(eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator 
        iter = mRealAttributes.find(attr);
    if(iter != mRealAttributes.end())
    {
        double base = iter->second; 
        return base * find_multiplier(attr) + find_addition(attr);
    }
    throw CL_Error("Attribute not set");
    return 0.0;
}

bool AttributeFile::getToggle(eCharacterAttribute attr)const
{
    assert_bool(attr);
    attr_bools::const_iterator 
        iter = mToggles.find(attr);
    if(iter != mToggles.end())
    {
        return iter->second; 
    }
    throw CL_Error("Attribute not set");
    return false;
}
void AttributeFile::attachMultiplication(eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] *= value;
    }
}

void AttributeFile::attachAddition(eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] += value;
    }
}

void AttributeFile::detachMultiplication(eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(value != 0.0)
    {
        if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
        {
            throw CL_Error("Attempt to detach when there was no attach!");
        }
        else
        {
            mAttrMultipliers[attr] /= value;
        }
    }
}

void AttributeFile::detachAddition(eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] -= value;
    }
}


void AttributeFile::fixAttribute(eCharacterAttribute attr, double value)
{
    assert_real(attr);
    mRealAttributes[attr] = value;
}

void AttributeFile::fixAttribute(eCharacterAttribute attr, bool toggle)
{
    assert_bool(attr);
    mToggles[attr] = toggle;
}



ICharacter::eGender Character::getGender() const
{
    return NEUTER;
}


double Character::getAttribute(eCharacterAttribute attr) const
{
    return mAttributes.getAttribute(attr);
}

bool Character::getToggle(eCharacterAttribute attr) const
{
    return mAttributes.getToggle(attr);
}


void Character::fixAttribute(eCharacterAttribute attr, bool value)
{
    mAttributes.fixAttribute(attr,value);
}

void Character::fixAttribute(eCharacterAttribute attr, double value)
{
    mAttributes.fixAttribute(attr,value);
}

void Character::attachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.attachMultiplication(attr,factor);
}

void Character::attachAddition(eCharacterAttribute attr, double value)
{
    mAttributes.attachAddition(attr,value);
}

void Character::detachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.detachMultiplication(attr,factor);
}

void Character::detachAddition(eCharacterAttribute attr, double value)
{
    mAttributes.detachAddition(attr,value);
}

void Character::addStatusEffect(StatusEffect *pEffect)
{
    mStatusEffects.push_back(pEffect);
}

static bool EffectMatches(const StatusEffect *pEffect, std::string name)
{
    return (pEffect->getName() == name);
}
void Character::removeEffects(const std::string &name)
{
    mStatusEffects.erase(std::remove_if(mStatusEffects.begin(),mStatusEffects.end(),
        std::bind2nd(std::ptr_fun(EffectMatches),name))); 
}



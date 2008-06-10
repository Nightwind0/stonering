#include "AttributeEnhancer.h"
#include "IApplication.h"
#include "SpriteDefinition.h"

using namespace StoneRing;

AttributeEnhancer::AttributeEnhancer():m_nAdd(0),m_fMultiplier(1)
{
}

void AttributeEnhancer::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_nAttribute = ICharacter::CAFromString(get_required_string("attribute", pAttributes));
    m_eType = static_cast<eType>( get_implied_int("type",pAttributes, EAUTO) );
    m_fMultiplier = get_implied_float("multiplier",pAttributes,1);
    m_nAdd = get_implied_int("add",pAttributes,0);
    m_bToggle = get_implied_bool("toggle",pAttributes,false);

    if(m_eType == EAUTO)
    {
        // Lets calculate the real type based on what is supplied
        if(has_attribute("multiplier",pAttributes))
            m_eType = static_cast<eType>(m_eType | EMULTIPLY);
        if(has_attribute("add",pAttributes))
            m_eType = static_cast<eType>(m_eType | EADD);
        if(has_attribute("toggle",pAttributes))
            m_eType = ETOGGLE; // No or, we can't combine this
    }
}


AttributeEnhancer::~AttributeEnhancer()
{
}


uint AttributeEnhancer::GetAttribute() const
{
    return m_nAttribute;
}

int AttributeEnhancer::GetAdd() const
{
    return m_nAdd;
}

float AttributeEnhancer::GetMultiplier() const
{
    return m_fMultiplier;
}

bool AttributeEnhancer::GetToggle() const
{
    return m_bToggle;
}

AttributeEnhancer::eType AttributeEnhancer::GetType() const
{
    return m_eType;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when invoking. (By calling equip on the armor/weapon...)
void AttributeEnhancer::Invoke()
{
    ICharacter * pCharacter = 
        IApplication::GetInstance()->GetTargetCharacterGroup()->GetTargetCharacter();

    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(m_nAttribute);
    if(m_eType & EMULTIPLY)
        pCharacter->AttachMultiplication(attr, m_fMultiplier);
    if(m_eType & EADD)
        pCharacter->AttachAddition(attr, m_nAdd);
    if(m_eType & ETOGGLE)
        pCharacter->FixAttribute(attr,m_bToggle);
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::Revoke()
{

    ICharacter * pCharacter = IApplication::GetInstance()
        ->GetTargetCharacterGroup()
        ->GetTargetCharacter();

    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(m_nAttribute);

    if(m_eType & EADD)
        pCharacter->DetachAddition(attr, m_nAdd);
    if(m_eType & EMULTIPLY)
        pCharacter->DetachMultiplication(attr, m_fMultiplier);
    if(m_eType & ETOGGLE)
        pCharacter->FixAttribute(attr, ! m_bToggle );

}





#include "AttributeModifier.h"
#include "IApplication.h"
#include "SpriteDefinition.h"

using namespace StoneRing;

AttributeModifier::AttributeModifier():m_pScript(NULL)
{
}

void AttributeModifier::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_nAttribute = ICharacter::CAFromString(get_required_string("attribute", pAttributes));
    m_eType = static_cast<eType>( get_required_int("type",pAttributes));

    if(has_attribute("value",pAttributes))
    {
        m_has_value = true;
        switch(m_eType)
        {
        case EADD:
        case EMULTIPLY:
            m_value.m_float = get_float("value",pAttributes);
            break;
        case ETOGGLE:
            m_value.m_toggle = get_bool("value",pAttributes);
            break;
        }
    }
    else
    {
        m_has_value = false;
    }

}

void AttributeModifier::load_finished()
{
    if(m_pScript == NULL && !m_has_value)
    {
        throw CL_Error("AttributeModifier needs value or script");
    }
}


AttributeModifier::~AttributeModifier()
{
}


uint AttributeModifier::GetAttribute() const
{
    return m_nAttribute;
}

double AttributeModifier::GetAdd() const
{
    if(m_eType == EADD)
    {
        // Having a script takes priority over setting a value
        if(m_pScript)
        {
            return m_pScript->ExecuteScript();
        }
        else
        {
            return m_value.m_float;
        }
    }
    else
    {
        return 0.0;
    }
}

double AttributeModifier::GetMultiplier() const
{
    if(m_eType == EMULTIPLY)
    {
        // Having a script takes priority over setting a value
        if(m_pScript)
        {
            return m_pScript->ExecuteScript();
        }
        else
        {
            return m_value.m_float;
        }
    }
    else
    {
        return 1.0;
    }
}

bool AttributeModifier::GetToggle() const
{
    if(m_eType == ETOGGLE)
    {
        // Having a script takes priority over setting a value
        if(m_pScript)
        {
            return m_pScript->ExecuteScript();
        }
        else
        {
            return m_value.m_toggle;
        }
    }
    else
    {
        cl_assert(0);
        return false;
    }
}

AttributeModifier::eType AttributeModifier::GetType() const
{
    return m_eType;
}

bool AttributeModifier::handle_element(Element::eElement element, Element * pElement)
{
    if(element == Element::ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }

    return false;
}
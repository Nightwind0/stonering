#include "AttributeModifier.h"
#include "IApplication.h"
#include "SpriteDefinition.h"

using namespace StoneRing;



AttributeModifier::eType AttributeModifier::GetType() const
{
    return m_eType;
}

uint AttributeModifier::GetAttribute() const
{
    return m_nAttribute;
}


AttributeModifier::AttributeModifier()
{
    m_value.m_int = 0;
}

AttributeModifier::~AttributeModifier()
{

}

void AttributeModifier::SetAdd ( double value )
{
    m_eType = EADD;
    m_value.m_float = value;
}
void AttributeModifier::SetMultiplier ( double value )
{
    m_eType = EMULTIPLY;
    m_value.m_float = value;
}
void AttributeModifier::SetToggle ( bool toggle )
{
    m_eType = ETOGGLE;
    m_value.m_toggle = toggle;
}

void AttributeModifier::SetAttribute ( uint attribute )
{
    m_nAttribute = attribute;
}



double AttributeModifier::GetAdd() const
{
    if(m_eType == EADD)
        return m_value.m_float;
    else return 0.0;
}

double AttributeModifier::GetMultiplier() const
{
    if(m_eType == EMULTIPLY)
        return m_value.m_float;
    else return 1.0;
}

bool AttributeModifier::GetToggle() const
{
    return m_value.m_toggle;
}



AttributeModifierElement::AttributeModifierElement():m_pScript(NULL)
{
}

void AttributeModifierElement::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_nAttribute = ICharacter::CAFromString(get_required_string("attribute", attributes));
    
    std::string type = get_required_string("type",attributes);
    
    if(type == "add")
        m_eType = EADD;
    else if(type == "multiply")
        m_eType = EMULTIPLY;
    else if(type == "toggle")
        m_eType = ETOGGLE;
    else throw CL_Exception("Bad Attribute Modifier type: " + type);
    
    if(has_attribute("value",attributes))
    {
        m_has_value = true;
        switch(m_eType)
        {
        case EADD:
        case EMULTIPLY:
            m_value.m_float = get_float("value",attributes);
            break;
        case ETOGGLE:
            m_value.m_toggle = get_bool("value",attributes);
            break;
        }
    }
    else
    {
        m_has_value = false;
    }

}

void AttributeModifierElement::load_finished()
{
    if(m_pScript == NULL && !m_has_value)
    {
        throw CL_Exception("AttributeModifier needs value or script");
    }
}


AttributeModifierElement::~AttributeModifierElement()
{
}



double AttributeModifierElement::GetAdd() const
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

double AttributeModifierElement::GetMultiplier() const
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

bool AttributeModifierElement::GetToggle() const
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
        assert(0);
        return false;
    }
}


bool AttributeModifierElement::handle_element(Element::eElement element, Element * pElement)
{
    if(element == Element::ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }

    return false;
}

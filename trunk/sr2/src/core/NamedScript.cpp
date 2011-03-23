#include "NamedScript.h"

StoneRing::NamedScript::NamedScript()
:m_pScript(NULL)
{
}


SteelType StoneRing::NamedScript::ExecuteScript(const ParameterList &params)
{
    return m_pScript->ExecuteScript(params);
}

SteelType StoneRing::NamedScript::ExecuteScript()
{
    return m_pScript->ExecuteScript();
}


bool StoneRing::NamedScript::handle_element(Element::eElement element,Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}
    
void StoneRing::NamedScript::load_finished()
{
    if(!m_pScript)
	throw CL_Exception("Script missing on named script element");
}


StoneRing::NamedScript::~NamedScript()
{
    delete m_pScript;
}


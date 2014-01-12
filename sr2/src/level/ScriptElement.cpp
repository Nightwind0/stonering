#include "ScriptElement.h"
#include "IApplication.h"

#ifdef _WINDOWS_
#include "Ast.h"
#else
#include <steel/Ast.h>
#endif


using StoneRing::ScriptElement;
using StoneRing::Element;

ScriptElement::ScriptElement(bool isCondition)
:mp_script(NULL),m_bIsCondition(isCondition)
{
}

ScriptElement::~ScriptElement()
{
    delete mp_script;
}


bool ScriptElement::EvaluateCondition() const
{
    if(mp_script == NULL) return true;

    IApplication *pApp = IApplication::GetInstance();
    return (bool) pApp->RunScript ( mp_script );
}

bool ScriptElement::EvaluateCondition(const ParameterList &params) const
{
    if(mp_script == NULL) return true;
    IApplication *pApp = IApplication::GetInstance();
    return pApp->RunScript ( mp_script, params );
}


SteelType ScriptElement::ExecuteScript() const
{
   if( NULL != mp_script )
   {
       IApplication *pApp = IApplication::GetInstance();
       return pApp->RunScript ( mp_script );
   }
   else return SteelType();
}

SteelType ScriptElement::ExecuteScript(const ParameterList &params)
{
    if ( NULL != mp_script)
    {
        IApplication *pApp = IApplication::GetInstance();
        return pApp->RunScript ( mp_script, params );
    }
    else return SteelType();
}

void ScriptElement::load_attributes(clan::DomNamedNodeMap attr)
{
    // Nothing for now
    m_id = get_required_string("id",attr);
}

void ScriptElement::handle_text(const std::string &text)
{
    IApplication *pApp = IApplication::GetInstance();
    mp_script = pApp->LoadScript(m_id, text);
    //m_id.clear(); // The script will remember it.. I don't want to.
#if SR2_EDITOR
    m_script = text;
#endif
}

#if SR2_EDITOR
clan::DomElement ScriptElement::CreateDomElement(clan::DomDocument &doc)const
{
    std::string name = "script";
    if(IsConditionScript()){
        name = "conditionScript";
    }
    clan::DomElement element(doc,name);
    
    element.set_attribute("id",m_id);
    clan::DomCDATASection text(doc,m_script);
    //text.set_node_value(m_script)
    element.append_child(text);
    return element;
}
ScriptElement::ScriptElement(const ScriptElement& other)
{
	m_id = other.m_id;
	m_script = other.m_script;
}


#endif



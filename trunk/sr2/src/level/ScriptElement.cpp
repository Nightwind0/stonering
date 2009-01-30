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

void ScriptElement::load_attributes(CL_DomNamedNodeMap *pAttr)
{
    // Nothing for now
    m_id = get_required_string("id",pAttr);
}

void ScriptElement::handle_text(const std::string &text)
{
    IApplication *pApp = IApplication::GetInstance();
    mp_script = pApp->LoadScript(m_id, text);
    m_id.clear(); // The script will remember it.. I don't want to.
}



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
:mpScript(NULL),mbIsCondition(isCondition)
{
}

ScriptElement::~ScriptElement()
{
    delete mpScript;
}


bool ScriptElement::evaluateCondition() const
{
    if(mpScript == NULL) return true;

    IApplication *pApp = IApplication::getInstance();
    return (bool) pApp->runScript ( mpScript );
}

SteelType ScriptElement::executeScript() const
{
   if( NULL != mpScript )
   {
       IApplication *pApp = IApplication::getInstance();
       return pApp->runScript ( mpScript );
   }
   else return SteelType();
}

SteelType ScriptElement::executeScript(const ParameterList &params)
{
    if ( NULL != mpScript)
    {
        IApplication *pApp = IApplication::getInstance();
        return pApp->runScript ( mpScript, params );
    }
    else return SteelType();
}

void ScriptElement::loadAttributes(CL_DomNamedNodeMap *pAttr)
{
    // Nothing for now
    mId = getRequiredString("id",pAttr);
}

void ScriptElement::handleText(const std::string &text)
{
    IApplication *pApp = IApplication::getInstance();
    mpScript = pApp->loadScript(mId, text);
    mId.clear(); // The script will remember it.. I don't want to.
}



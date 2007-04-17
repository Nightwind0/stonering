#include "ScriptElement.h"
#include "IApplication.h"
#include "Ast.h"

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

CL_DomElement ScriptElement::createDomElement(CL_DomDocument&) const
{
    return CL_DomElement();
}


bool ScriptElement::evaluateCondition() const
{
    if(mpScript == NULL) return true;

    IApplication *pApp = IApplication::getInstance();
    return (bool) pApp->runScript ( mpScript );
}

void ScriptElement::executeScript() const
{
   if( NULL != mpScript )
   {
       IApplication *pApp = IApplication::getInstance();
       pApp->runScript ( mpScript );
   }
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



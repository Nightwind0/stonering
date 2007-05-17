#include "NamedScript.h"

StoneRing::NamedScript::NamedScript()
:mpScript(NULL)
{
}


bool StoneRing::NamedScript::handleElement(eElement element,Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}
    


StoneRing::NamedScript::~NamedScript()
{
    delete mpScript;
}

#include "Event.h"
#include "IApplication.h"


StoneRing::Event::Event():mbRepeatable(true),mbRemember(false),mpCondition(NULL),mpScript(NULL)
{
}


void StoneRing::Event::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string triggertype = getString("triggerType",pAttributes);

    if(triggertype == "step")
        meTriggerType = STEP;
    else if (triggertype == "talk")
        meTriggerType = TALK;
    else if (triggertype == "act")
        meTriggerType = ACT;
    else throw CL_Error(" Bad trigger type on event " + mName );

    mbRepeatable = getImpliedBool("repeatable",pAttributes,true);
    mbRemember = getImpliedBool("remember",pAttributes,false);
}

bool StoneRing::Event::handleElement(eElement element, Element *pElement)
{
    if(element == ESCRIPT)
    {
        mpScript = dynamic_cast<ScriptElement*>(pElement);
    }
    else if(element == ECONDITIONSCRIPT)
    {
        mpCondition = dynamic_cast<ScriptElement*>(pElement);
    }
    else 
    {
        return false;
    }

    return true;
}


StoneRing::Event::~Event()
{
    delete mpScript;
    delete mpCondition;
}

std::string StoneRing::Event::getName() const
{
    return mName;
}

StoneRing::Event::eTriggerType StoneRing::Event::getTriggerType()
{
    return meTriggerType;
}

bool StoneRing::Event::repeatable()
{
    return mbRepeatable;
}

bool StoneRing::Event::remember()
{
    return mbRemember;
}
 
bool StoneRing::Event::invoke()
{
    
    if(mpCondition && !mpCondition->evaluateCondition() )
        return false;

    StoneRing::IApplication::getInstance()->getParty()->doEvent ( mName, mbRemember );

    if(mpScript)
        mpScript->executeScript();

    return true;
}
      


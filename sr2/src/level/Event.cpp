#include "Event.h"
#include "IApplication.h"
#include "IParty.h"


StoneRing::Event::Event():m_bRepeatable(true),m_bRemember(false),m_pCondition(NULL),m_pScript(NULL)
{
}


void StoneRing::Event::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);

    std::string triggertype = get_string("triggerType",attributes);

    if(triggertype == "step")
        m_eTriggerType = STEP;
    else if (triggertype == "talk")
        m_eTriggerType = TALK;
    else if (triggertype == "act")
        m_eTriggerType = ACT;
    else throw CL_Exception(" Bad trigger type on event " + m_name );

    m_bRepeatable = get_implied_bool("repeatable",attributes,true);
    m_bRemember = get_implied_bool("remember",attributes,false);
}

bool StoneRing::Event::handle_element(Element::eElement element, Element *pElement)
{
    if(element == ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
    }
    else if(element == ECONDITIONSCRIPT)
    {
        m_pCondition = dynamic_cast<ScriptElement*>(pElement);
    }
    else
    {
        return false;
    }

    return true;
}


StoneRing::Event::~Event()
{
    delete m_pScript;
    delete m_pCondition;
}

std::string StoneRing::Event::GetName() const
{
    return m_name;
}

StoneRing::Event::eTriggerType StoneRing::Event::GetTriggerType()
{
    return m_eTriggerType;
}

bool StoneRing::Event::Repeatable()
{
    return m_bRepeatable;
}

bool StoneRing::Event::Remember()
{
    return m_bRemember;
}

bool StoneRing::Event::Invoke()
{

    if(m_pCondition && !m_pCondition->EvaluateCondition() )
        return false;

    StoneRing::IApplication::GetInstance()->GetParty()->DoEvent ( m_name, m_bRemember );

    if(m_pScript)
        m_pScript->ExecuteScript();

    return true;
}



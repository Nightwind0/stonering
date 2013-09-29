#include "Event.h"
#include "IApplication.h"
#include "Party.h"


namespace StoneRing { 

Event::Event():m_bRepeatable(true),m_bRemember(false),m_pCondition(NULL),m_pScript(NULL)
{
}


void Event::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);

    std::string triggertype = get_string("triggerType",attributes);

    if(triggertype == "step")
        m_eTriggerType = STEP;
    else if (triggertype == "talk")
        m_eTriggerType = TALK;
    else if (triggertype == "act")
        m_eTriggerType = ACT;
	else if (triggertype == "collide")
		m_eTriggerType = COLLIDE;
    else throw XMLException(" Bad trigger type on event " + m_name );

    m_bRepeatable = get_implied_bool("repeatable",attributes,true);
    m_bRemember = get_implied_bool("remember",attributes,false);
}

bool Event::handle_element(Element::eElement element, Element *pElement)
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


Event::~Event()
{
    delete m_pScript;
    delete m_pCondition;
}

std::string Event::GetName() const
{
    return m_name;
}

Event::eTriggerType Event::GetTriggerType()
{
    return m_eTriggerType;
}

bool Event::Repeatable()
{
    return m_bRepeatable;
}


bool Event::Invoke()
{

    if(m_pCondition && !m_pCondition->EvaluateCondition() )
        return false;

    IApplication::GetInstance()->GetParty()->DoEvent ( m_name, m_bRemember );

    if(m_pScript)
        m_pScript->ExecuteScript();

    return true;
}

bool Event::Invoke(const ParameterList& params)
{

    if(m_pCondition && !m_pCondition->EvaluateCondition() )
        return false;

    IApplication::GetInstance()->GetParty()->DoEvent ( m_name, m_bRemember );

    if(m_pScript)
        m_pScript->ExecuteScript(params);

    return true;
}

#if SR2_EDITOR
CL_DomElement Event::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement element(doc,"event");
    element.set_attribute("name", m_name );

    std::string triggertype;

    switch( m_eTriggerType )
    {
    case STEP:
        triggertype = "step";
        break;
    case TALK:
        triggertype = "talk";
        break;
    case ACT:
        triggertype = "act";
        break;
	case COLLIDE:
		triggertype = "collide";
		break;
    }

    element.set_attribute("triggerType", triggertype);

    if(!m_bRepeatable) element.set_attribute("repeatable","false");

    if(m_bRemember) element.set_attribute("remember","true");

    if(m_pCondition)
    {
        element.append_child(m_pCondition->CreateDomElement(doc));
    }

    if(m_pScript)
    {
        element.append_child(m_pScript->CreateDomElement(doc));
    }
    
    return element;
}

void Event::SetCondition(ScriptElement * pElement)
{
	if(m_pCondition) delete m_pCondition;
	m_pCondition = pElement;
}
void Event::SetScript(ScriptElement* pElement)
{
	if(m_pScript) delete m_pScript;
	m_pScript = pElement;
}
void Event::SetRepeatable(bool repeatable)
{
	m_bRepeatable = repeatable;
}
void Event::SetRemember(bool remember)
{
	m_bRemember = remember;
}
void Event::SetTrigger(eTriggerType trigger)
{
	m_eTriggerType = trigger;
}
void Event::SetName(const std::string& name)
{
	m_name = name;
}


#endif


}



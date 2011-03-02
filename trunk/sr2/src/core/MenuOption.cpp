#include "MenuOption.h"
#include "BattleMenu.h"
#include "GraphicsManager.h"

using StoneRing::MenuOption;

MenuOption::MenuOption(int level):
m_pConditionScript(NULL),m_pScript(NULL)
{

}

MenuOption::~MenuOption()
{
    delete m_pConditionScript;
    delete m_pScript;
}

std::string MenuOption::GetName() const
{
    return m_name;
}
bool MenuOption::Enabled(const ParameterList &params) const
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
}

void MenuOption::Select(const ParameterList& params){
    m_pScript->ExecuteScript(params);
}


bool MenuOption::handle_element(Element::eElement element, Element *pElement)
{
    switch(element)
    {
    case ECONDITIONSCRIPT:
        m_pConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;
    };

    return true;
}

void MenuOption::load_finished()
{
}

void MenuOption::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_icon = GraphicsManager::GetIcon( get_implied_string("icon",attributes,"no_icon") );
}


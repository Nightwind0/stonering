#ifndef SR_MENU_OPTION_H
#define SR_MENU_OPTION_H

#include "Element.h"
#include "ScriptElement.h"
#include "Skill.h"
#include <stack>

class SteelInterpreter;

namespace StoneRing
{


    class MenuOption : public Element
    {
    public:
        MenuOption(int level=0);
        virtual ~MenuOption();
        virtual eElement WhichElement() const { return EMENUOPTION; }
        bool HasChildren() const;
        MenuOption* GetParent() const { return m_parent; }
        std::vector<MenuOption*>::const_iterator GetChildrenBegin()const;
        std::vector<MenuOption*>::const_iterator GetChildrenEnd()const;
        std::string GetName() const;
        clan::Image GetIcon() const { return m_icon; }
        bool Enabled(const ParameterList &params) const;
        void Select(const ParameterList& params);
		virtual std::string GetDebugId() const { return m_name; }				
    private:
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(clan::DomNamedNodeMap );
        virtual void load_finished();
    protected:
        std::string m_name;
        std::vector<MenuOption*> m_children;
        ScriptElement *m_pConditionScript;
        ScriptElement *m_pScript;
        MenuOption* m_parent;
        clan::Image m_icon;
    };
}


#endif


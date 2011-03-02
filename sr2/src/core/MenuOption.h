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

        std::string GetName() const;
        CL_Image GetIcon() const { return m_icon; }
        bool Enabled(const ParameterList &params) const;
        void Select(const ParameterList& params);
    private:
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap );
        virtual void load_finished();
    protected:

        std::string m_name;
        ScriptElement *m_pConditionScript;
        ScriptElement *m_pScript;
        CL_Image m_icon;

    };
}


#endif


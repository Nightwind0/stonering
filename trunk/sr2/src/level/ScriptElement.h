#ifndef SR_SCRIPT_ELEMENT_H
#define SR_SCRIPT_ELEMENT_H

#include "sr_defines.h"
#include "Element.h"
#ifdef _WINDOWS_
#include "SteelType.h"
#include "SteelInterpreter.h"
#else
#include <steel/SteelType.h>
#include <steel/SteelInterpreter.h>
#endif

class AstScript;

namespace StoneRing{


    class ScriptElement : public Element
    {
    public:
        ScriptElement(bool isCondition);
        virtual ~ScriptElement();
        virtual eElement WhichElement() const { return m_bIsCondition?ECONDITIONSCRIPT:ESCRIPT; }

        bool EvaluateCondition() const;
        bool EvaluateCondition(const ParameterList &params)const;
        SteelType ExecuteScript() const ;
        SteelType ExecuteScript(const ParameterList &params);

        bool IsConditionScript() const { return m_bIsCondition; }
    protected:
        virtual void load_attributes(CL_DomNamedNodeMap *);
        virtual void handle_text(const std::string &);
        AstScript * mp_script;
        std::string m_id;
        bool m_bIsCondition;

    };
}

#endif




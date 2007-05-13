#ifndef SR_SCRIPT_ELEMENT_H
#define SR_SCRIPT_ELEMENT_H

#include "sr_defines.h"
#include "Element.h"
#include "SteelType.h"
#include "SteelInterpreter.h"

class AstScript;

namespace StoneRing{


    class ScriptElement : public Element
    {
    public:
        ScriptElement(bool isCondition);
        virtual ~ScriptElement();
        virtual eElement whichElement() const { return mbIsCondition?ECONDITIONSCRIPT:ESCRIPT; }

        bool evaluateCondition() const;
        SteelType executeScript() const ;
        SteelType executeScript(const ParameterList &params);

        bool isConditionScript() const { return mbIsCondition; }

    protected:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void handleText(const std::string &);
        AstScript * mpScript;
        std::string mId;
        bool mbIsCondition;

    };
}

#endif




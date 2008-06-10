#ifndef SR_EVENT_H
#define SR_EVENT_H

#include "Element.h"
#include "ScriptElement.h"

namespace StoneRing{

    class Event : public Element
    {
    public:
        Event();
        virtual ~Event();

        enum eTriggerType { STEP, TALK, ACT };
        virtual eElement WhichElement() const{ return EEVENT; } 
        std::string GetName() const;
        eTriggerType GetTriggerType();
        bool Repeatable();
        inline bool Remember();
        bool Invoke();
    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        std::string m_name;
        bool m_bRepeatable;
        bool m_bRemember;
        eTriggerType m_eTriggerType;
        ScriptElement *m_pCondition;
        ScriptElement *m_pScript;

    };


};


#endif




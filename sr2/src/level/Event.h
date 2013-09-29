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

        enum eTriggerType { STEP, TALK, ACT, COLLIDE };
        virtual eElement WhichElement() const{ return EEVENT; }
        std::string GetName() const;
        eTriggerType GetTriggerType();
        bool Repeatable();
        inline bool Remember() { return m_bRemember; }
        bool Invoke();
        bool Invoke(const ParameterList& params);
		virtual std::string GetDebugId() const { return m_name; }				
		
    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap  attributes);
        std::string m_name;
        bool m_bRepeatable;
        bool m_bRemember;
        eTriggerType m_eTriggerType;
        ScriptElement *m_pCondition;
        ScriptElement *m_pScript;
#if SR2_EDITOR
	public:
        CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
		void SetCondition(ScriptElement * pElement);
		void SetScript(ScriptElement* pElement);
		void SetRepeatable(bool repeatable);
		void SetRemember(bool remember);
		void SetTrigger(eTriggerType trigger);
		void SetName(const std::string& name);
		ScriptElement * GetCondition() { return m_pCondition; } 
		ScriptElement * GetScript() { return m_pScript; }
#endif		
    };


}


#endif




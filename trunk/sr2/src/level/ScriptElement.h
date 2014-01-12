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


using Steel::ParameterList;
using Steel::SteelType;


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
		virtual std::string GetDebugId() const { return m_id; }				
        
    protected:
        virtual void load_attributes(clan::DomNamedNodeMap);
        virtual void handle_text(const std::string &);
        Steel::AstScript * mp_script;
        std::string m_id;
        bool m_bIsCondition;
#if SR2_EDITOR
	public:
		void SetScript(const std::string& script) { m_script = script; }	
		void SetId(const std::string& id) { m_id = id; }
		void SetIsCondition(bool is_condition) { m_bIsCondition = is_condition; }		
        virtual clan::DomElement CreateDomElement(clan::DomDocument&)const;
		std::string GetScriptText() const { return m_script; }
		std::string GetScriptId() const { return m_id; }
		ScriptElement(const ScriptElement& copy);
    protected:
        std::string m_script;        
#endif		
    };
}

#endif




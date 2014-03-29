#ifndef SR_SCRIPT_ELEMENT_H
#define SR_SCRIPT_ELEMENT_H

#include "sr_defines.h"
#include "Element.h"
#if defined(_WINDOWS_) || defined(__APPLE__)
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
        ScriptElement(bool condition, bool animation);
        virtual ~ScriptElement();
        virtual eElement WhichElement() const { return m_element_type; }

        bool EvaluateCondition() const;
        bool EvaluateCondition(const ParameterList &params)const;
        SteelType ExecuteScript() const ;
        SteelType ExecuteScript(const ParameterList &params);
        
    protected:
        virtual void load_attributes(clan::DomNamedNodeMap);
        virtual void handle_text(const std::string &);
        Steel::AstScript * mp_script;
        std::string m_id;
	public:
      bool IsConditionScript() const { return m_element_type == ECONDITIONSCRIPT; }
	  virtual std::string GetDebugId() const { return m_id; }	
	  Element::eElement m_element_type;
	private:
#if SR2_EDITOR
	public:
		void SetScript(const std::string& script) { m_script = script; }	
		void SetId(const std::string& id) { m_id = id; }
		void SetIsCondition(bool is_condition) { m_element_type = Element::ECONDITIONSCRIPT; }		
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




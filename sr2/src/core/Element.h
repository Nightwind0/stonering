#ifndef SR_ELEMENT_H
#define SR_ELEMENT_H

#include "sr_defines.h"
#if !defined(WIN32) && !defined(__APPLE__)
    #include "steel/SteelType.h"
#else
    #include "SteelType.h"
#endif
#include <sstream>
#include <list>
#include <ClanLib/core.h>
//#include "IFactory.h"
//#include "IApplication.h"




namespace StoneRing
{

    class IApplication;

    class Element 
    {
    public:
        Element()
#ifndef NDEBUG
            :m_pParent(NULL)
#endif
        {}

        virtual ~Element(){}

// These MUST be kept alphabetized
        enum eElement
        {
			EANIMATIONSCRIPT,
            EARMORCLASS,
            EARMORCLASSREF,
            EARMORENHANCER,
            EARMORIMBUEMENTREF,
            EARMORREF,
            EARMORTYPE,
            EARMORTYPEEXCLUSIONLIST,
            EARMORTYPEREF,
            EATTRIBUTEMODIFIER,
            EBATTLEMENU,
            EBATTLEMENUOPTION,
            ECHARACTER,
            ECHARACTERCLASS,
            ECONDITIONSCRIPT,
            EDIRECTIONBLOCK,
            EDESCRIPTION,
            EEVENT,
            EICONREF,
            EITEMREF,
            ELEVEL,
            ELEVELHEADER,
            EMAGICRESISTANCE,
            EMAPPABLEOBJECT,
            EMAPPABLEOBJECTS,
            EMONSTER,
            EMONSTERGROUP,
            EMONSTERREF,
            EMONSTERREGION,
            EMONSTERREGIONS,
            EMOVEMENT,
			EMENUOPTION,
            ENAMEDITEMREF,
            EOMEGA,
            EONCOUNTDOWN,
            EONDESELECT,
            EONEQUIP,
            EONINVOKE,
            EONREMOVE,
            EONROUND,
            EONSELECT,
            EONSTEP,
            EONUNEQUIP,
            EPREREQSKILLREF,
            EREGULARITEM,
            ERUNE,
            ERUNETYPE,
            ESCRIPT,
            ESKILL,
            ESKILLREF,
            ESKILLTREENODE,
            ESOUNDPLAY,
            ESPECIALITEM,
            ESPRITEDEFINITION,
            ESPRITEREF,
            ESTAT,
            ESTATSCRIPT,
            ESTATUSEFFECT,
            ESTATUSEFFECTINFLICTION,
            ESTATUSEFFECTMODIFIER,
            ESYSTEMITEM,
            ETILE,
            ETILEMAP,
            ETILES,
            EUNIQUEARMOR,
            EUNIQUEWEAPON,
            EWEAPONCLASS,
            EWEAPONCLASSREF,
            EWEAPONENHANCER,
            EWEAPONIMBUEMENTREF,
            EWEAPONREF,
            EWEAPONTYPE,
            EWEAPONTYPEEXCLUSIONLIST,
            EWEAPONTYPEREF,
            __END_OF_ELEMENTS__
        };

        virtual eElement WhichElement() const=0;
        void Load(clan::DomElement  element);
#ifndef NDEBUG
        void SetElementName(const std::string &name) { m_element_name = name; }
        void SetParent(Element *pParent) { m_pParent = pParent; }
#endif

    protected:
        virtual bool handle_element(eElement, Element * ){ return false;}
        virtual void load_attributes(clan::DomNamedNodeMap ){}
        virtual void handle_text(const std::string &){}
        virtual void load_finished(){} // You can check shit . Make sure you got everything.


        uint get_required_uint(const std::string &attrname, clan::DomNamedNodeMap  attributes);
        int  get_required_int(const std::string &attrname, clan::DomNamedNodeMap attributes);
        float get_required_float(const std::string &attrname, clan::DomNamedNodeMap attributes );
        std::string get_required_string (const std::string &attrname, clan::DomNamedNodeMap attributes );
        bool get_required_bool (const std::string &attrname, clan::DomNamedNodeMap attributes );
        bool has_attribute( const std::string &attrname, clan::DomNamedNodeMap attributes );
        uint get_uint(const std::string &attrname, clan::DomNamedNodeMap  attributes);
        int  get_int(const std::string &attrname, clan::DomNamedNodeMap attributes);
        float get_float(const std::string &attrname, clan::DomNamedNodeMap attributes );
        bool get_bool (const std::string &attrname, clan::DomNamedNodeMap attributes );
        std::string get_string (const std::string &attrname, clan::DomNamedNodeMap attributes );
        bool get_implied_bool ( const std::string &attrname, clan::DomNamedNodeMap attributes, bool defaultValue);
        int get_implied_int( const std::string &attrname, clan::DomNamedNodeMap  attributes, int defaultValue);
        std::string get_implied_string( const std::string &attrname, clan::DomNamedNodeMap  attributes, const std::string &defaultValue);
        float get_implied_float(const std::string &attrname, clan::DomNamedNodeMap attributes, float defaultValue);
#ifndef NDEBUG
        std::string get_element_name() const;
		virtual std::string GetDebugId() const =0;
#else
        std::string get_element_name() const {
            std::ostringstream os;
            os << "Element #" << WhichElement() << "- Use debug mode to find out the name.";
            return os.str();
        }
#endif

    private:
#ifndef NDEBUG
        Element * m_pParent;
        std::string m_element_name;
#endif

/*
  struct ElementCreationEntry
  {
  const char * pszElementName;
  const IFactory * (IApplication::*pGetFactory)();
  const CreateElementMethod
  const Element     * (Factory::*pCreateElement)();
  };

  // You have to keep this bitch sorted so you can binary search it's ass
  const static ElementCreationEntry g_pElementCreationEntries[];
*/

    };





class XMLException {
public:
	XMLException(const std::string& str){
		push_error(str);
	}
	~XMLException(){}
	void push_error(const std::string& str){
		m_err_stack.push_back(str);
	}
	void dump(std::ostream& out){
		for(std::list<std::string>::const_iterator it = m_err_stack.begin(); it!= m_err_stack.end(); it++){
			out << *it << std::endl;
		}
	}
private:
	std::list<std::string> m_err_stack;
};




}

#endif





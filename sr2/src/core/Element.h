#ifndef SR_ELEMENT_H
#define SR_ELEMENT_H

#include "sr_defines.h"

#include <sstream>
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
            EALTERSPRITE, 
            EANIMATION,
            EARMORCLASS,
            EARMORCLASSREF,
            EARMORENHANCER,
            EARMORREF,
            EARMORTYPE,
            EARMORTYPEEXCLUSIONLIST,
            EARMORTYPEREF,
            EATTRIBUTEENHANCER,
            EBATTLEMENU,
            EBATTLEMENUOPTION,
            ECHARACTER,
            ECHARACTERCLASS,
            ECONDITIONSCRIPT,
            EDIRECTIONBLOCK,
            EEVENT,
            EICONREF,
            EITEMREF,
            ELEVEL,
            ELEVELHEADER,
            EMAGICDAMAGECATEGORY,
            EMAGICRESISTANCE,
            EMAPPABLEOBJECT,
            EMAPPABLEOBJECTS,
            EMONSTER,
            EMONSTERGROUP,
            EMONSTERREF,
            EMONSTERREGION,
            EMONSTERREGIONS,
            EMOVEMENT,
            ENAMEDITEMELEMENT,
            ENAMEDITEMREF,
            EONCOUNTDOWN,
            EONDESELECT,
            EONEQUIP,
            EONINVOKE,
            EONREMOVE,
            EONROUND,
            EONSELECT,
            EONSTEP,
            EONUNEQUIP,
            EPHASE,
            EPREREQSKILLREF,
            EREGULARITEM,
            ERUNE,
            ERUNETYPE,
            ESCRIPT,
            ESKILL,
            ESKILLREF,
            ESPECIALITEM,
            ESPELL,
            ESPELLREF,
            ESPRITEANIMATION,
            ESPRITEDEFINITION,
            ESPRITEMOVEMENT,
            ESPRITEREF,
            ESPRITESTUB,
            ESTAT,
            ESTATSCRIPT,
            ESTATUSEFFECT,
            ESTATUSEFFECTMODIFIER,
            ESYSTEMITEM,
            ETILE,
            ETILEMAP,
            ETILES,
            EUNIQUEARMOR,
            EUNIQUEWEAPON,
            EWEAPONCLASS,
            EWEAPONCLASSREF,
            EWEAPONDAMAGECATEGORY,
            EWEAPONENHANCER,
            EWEAPONREF,
            EWEAPONTYPE,
            EWEAPONTYPEEXCLUSIONLIST,
            EWEAPONTYPEREF,
            __END_OF_ELEMENTS__
        };

        virtual eElement WhichElement() const=0;
        void Load(CL_DomElement * pElement);
#ifndef NDEBUG
        void SetElementName(const std::string &name) { m_element_name = name; }
        void SetParent(Element *pParent) { m_pParent = pParent; }
#endif
          
    protected:
        virtual bool handle_element(eElement, Element * ){ return false;}
        virtual void load_attributes(CL_DomNamedNodeMap *){}
        virtual void handle_text(const std::string &){}
        virtual void load_finished(){} // You can check shit . Make sure you got everything.


        uint get_required_uint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        int  get_required_int(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        float get_required_float(const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        std::string get_required_string (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool get_required_bool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool has_attribute( const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        uint get_uint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        int  get_int(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        float get_float(const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool get_bool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        std::string get_string (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool get_implied_bool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, bool defaultValue);
        int get_implied_int( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, int defaultValue);
        std::string get_implied_string( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, const std::string &defaultValue);
        float get_implied_float(const std::string &attrname, CL_DomNamedNodeMap *pAttributes, float defaultValue);
#ifndef NDEBUG
        std::string get_element_name() const;
#else
        std::string getElementName() const { 
            std::ostringstream os;
            os << "Element #" << whichElement() << "- Use debug mode to find out the name.";
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










};

#endif





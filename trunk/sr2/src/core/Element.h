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
            :mpParent(NULL)
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

        virtual eElement whichElement() const=0;
        void load(CL_DomElement * pElement);
#ifndef NDEBUG
        void setElementName(const std::string &name) { mElementName = name; }
        void setParent(Element *pParent) { mpParent = pParent; }
#endif
          
    protected:
        virtual bool handleElement(eElement, Element * ){ return false;}
        virtual void loadAttributes(CL_DomNamedNodeMap *){}
        virtual void handleText(const std::string &){}
        virtual void loadFinished(){} // You can check shit . Make sure you got everything.


        uint getRequiredUint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        int  getRequiredInt(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        float getRequiredFloat(const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        std::string getRequiredString (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool getRequiredBool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool hasAttr( const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        uint getUint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        int  getInt(const std::string &attrname, CL_DomNamedNodeMap * pAttributes);
        float getFloat(const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool getBool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        std::string getString (const std::string &attrname, CL_DomNamedNodeMap * pAttributes );
        bool getImpliedBool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, bool defaultValue);
        int getImpliedInt( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, int defaultValue);
        std::string getImpliedString( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, const std::string &defaultValue);
        float getImpliedFloat(const std::string &attrname, CL_DomNamedNodeMap *pAttributes, float defaultValue);
#ifndef NDEBUG
        std::string getElementName() const;
#else
        std::string getElementName() const { 
            std::ostringstream os;
            os << "Element #" << whichElement() << "- Use debug mode to find out the name.";
            return os.str();
        }
#endif

    private:
#ifndef NDEBUG
        Element * mpParent;
        std::string mElementName;
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





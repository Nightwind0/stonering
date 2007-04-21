#ifndef SR_ELEMENT_H
#define SR_ELEMENT_H

#include "sr_defines.h"

#include <ClanLib/core.h>
//#include "IFactory.h"
//#include "IApplication.h"




namespace StoneRing
{

    class IApplication;


    class Element
    {
    public:
        Element(){}
        virtual ~Element(){}

// These MUST be kept alphabetized
        enum eElement
        {    
            EANIMATION,
            EANIMATIONDEFINITION,
            EANIMATIONSPRITEREF,
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
            EMOVEMENT,
            ENAMEDITEMELEMENT,
            ENAMEDITEMREF,
            EONCOUNTDOWN,
            EONEQUIP,
            EONINVOKE,
            EONREMOVE,
            EONROUND,
            EONSTEP,
            EONUNEQUIP,
            EPAR,
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
            ESPRITEREF,
            ESTATINCREASE,
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
            EWEAPONTYPESPRITE,
            __END_OF_ELEMENTS__
        };

        virtual eElement whichElement() const=0;
        void load(CL_DomElement * pElement);

          
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
        std::string getElementName() const;

    private:

    


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





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

			virtual CL_DomElement  createDomElement(CL_DomDocument&) const=0;

// These MUST be kept alphabetized
			enum eElement
				{	 


                    EAND,
                    EANIMATION,
                    EANIMATIONSPRITEREF,
                    EARMORCLASS,
                    EARMORCLASSREF,
                    EARMORENHANCER,
                    EARMORREF,
                    EARMORTYPE,
                    EARMORTYPEEXCLUSIONLIST,
                    EARMORTYPEREF,
                    EATTRIBUTEEFFECT,
                    EATTRIBUTEENHANCER,
                    EATTRIBUTEMODIFIER,
                    ECHARACTERCLASS,
                    ECHOICE,
                    ECONDITION,
                    EDIDEVENT,
                    EDIRECTIONBLOCK,
                    EDOATTACK,
                    EDOMAGICDAMAGE,
                    EDOSTATUSEFFECT,
                    EDOWEAPONDAMAGE,
                    EEVENT,
                    EGIVE,
                    EGIVEGOLD,
                    EHASGOLD,
                    EHASITEM,
                    EICONREF,
                    EINVOKESHOP,
                    EITEMREF,
                    ELEVEL,
                    ELOADLEVEL,
                    EMAGICDAMAGECATEGORY,
                    EMAGICRESISTANCE,
                    EMAPPABLEOBJECT,
                    EMOVEMENT,
                    ENAMEDITEMELEMENT,
                    ENAMEDITEMREF,
                    EONCOUNTDOWN,
                    EONINVOKE,
                    EONREMOVE,
                    EONROUND,
                    EOPERATOR,
                    EOPTION,
                    EOR,
                    EPAR,
                    EPAUSE,
                    EPLAYANIMATION,
                    EPLAYSOUND,
                    EPOP,
					EPREREQSKILLREF,
                    EREGULARITEM,
                    ERUNE,
                    ERUNETYPE,
                    ESAY,
                    ESKILL,
                    ESKILLREF,
                    ESPECIALITEM,
                    ESPELL,
                    ESPELLREF,
                    ESPRITEREF,
                    ESTARTBATTLE,
                    ESTARTINGSTAT,
                    ESTATINCREASE,
                    ESTATUSEFFECT,
                    ESTATUSEFFECTACTIONS,
                    ESTATUSEFFECTMODIFIER,
                    ESYSTEMITEM,
                    ETAKE,
                    ETILE,
                    ETILEMAP,
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


            static const char * pszElementNames[__END_OF_ELEMENTS__];
    
        protected:
            virtual void handleElement(eElement, Element * ){}
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

            bool isAction(eElement element)const;
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

#include <ClanLib/core.h>
#include "Element.h"

#include "IApplication.h"
#include "LevelFactory.h"
#include <algorithm>
#include "ItemFactory.h"


using StoneRing::IApplication;
using StoneRing::LevelFactory;
using StoneRing::Element;


/*const  Element::ElementCreationEntry Element::g_pElementCreationEntries[] = 
  { 
  {"itemRef", &IApplication::getLevelFactory, &LevelFactory::createItemRef},
  {"tile", &IApplication::getLevelFactory, &LevelFactory::createTile},
  {"condition", &IApplication::getLevelFactory, &LevelFactory::createCondition}
  }
*/

// MUST be alphabetized.
const char * StoneRing::Element::pszElementNames[Element::__END_OF_ELEMENTS__] = 
{

    "and",//        AND,
    "animation",//          ANIMATION,
    "animationSpriteRef",
    "armorClass",//         ARMORCLASS,
    "armorClassRef",//      ARMORCLASSREF,
    "armorEnhancer",//      ARMORENHANCER,
    "armorRef",//           ARMORREF,
    "armorType",//          ARMORTYPE,
    "armorTypeExclusionList",
    "armorTypeRef",//       ARMORTYPEREF,
    "attributeEffect",//            ATTRIBUTEEFFECT,
    "attributeEnhancer",//          ATTRIBUTEENHANCER,
    "attributeModifier",//          ATTRIBUTEMODIFIER,
    "choice",//             CHOICE,
    "condition",//          CONDITION,
    "didEvent",//           DIDEVENT,
    "directionBlock",//  DIRECTIONBLOCK,
    "doMagicDamage",//      DOMAGICDAMAGE,
    "doStatusEffect",//             DOSTATUSEFFECT,
    "doWeaponDamage",//             DOWEAPONDAMAGE,
    "event",//      EVENT,
    "give",//       GIVE,
    "giveGold",//           GIVEGOLD,
    "hasGold",//            HASGOLD,
    "hasItem",//            HASITEM,
    "iconRef",
    "invokeShop",//         INVOKESHOP,
    "itemRef",//            ITEMREF,
    "level",//      LEVEL,
    "loadLevel",//          LOADLEVEL,
    "magicDamageCategory",//        MAGICDAMAGECATEGORY,
    "magicResistance",//            MAGICRESISTANCE,
    "mappableObject",//             MAPPABLEOBJECT,
    "movement",//           MOVEMENT,
    "namedItemElement",//           NAMEDITEMELEMENT,
    "namedItemRef",//       NAMEDITEMREF,
    "onCountdown",         // EONCOUNTDOWN,
    "onInvoke",//EONINVOKE,
    "onRemove",//                   EONREMOVE,
    "onRound",//                    EONROUND,
    "operator",//           OPERATOR,
    "option",//             OPTION,
    "or",//         OR,
    "par",
    "pause",//      PAUSE,
    "playAnimation",//      PLAYANIMATION,
    "playSound",//          PLAYSOUND,
    "regularItem",//        REGULARITEM,
    "rune",//       RUNE,
    "runeType",//           RUNETYPE,
    "say",//        SAY,
    "skillRef",
    "specialItem",//        SPECIALITEM,
    "spell",//      SPELL,
    "spellRef",//           SPELLREF,
    "spriteRef",//          SPRITEREF,
    "startBattle",//        STARTBATTLE,
    "startingStat",//       STARTINGSTAT,
    "statIncrease",//       STATINCREASE,
    "statusEffect",//       STATUSEFFECT,
    "statusEffectActions",//        STATUSEFFECTACTIONS,
    "statusEffectModifier",//       STATUSEFFECTMODIFIER,
    "systemItem",//         SYSTEMITEM,
    "take",//       TAKE,
    "tile",//       TILE,
    "tilemap",//            TILEMAP,
    "uniqueArmor",//        UNIQUEARMOR,
    "uniqueWeapon",//       UNIQUEWEAPON,
    "weaponClass",//        WEAPONCLASS,
    "weaponClassRef",//             WEAPONCLASSREF,
    "weaponDamageCategory",//       WEAPONDAMAGECATEGORY,
    "weaponEnhancer",//             WEAPONENHANCER,
    "weaponRef",//          WEAPONREF,
    "weaponType",//         WEAPONTYPE,
    "weaponTypeExclusionList",
    "weaponTypeRef",//      WEAPONTYPEREF,

};

uint Element::getRequiredUint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    if( hasAttr ( attrname, pAttributes ) )
    {
        return getUint ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname );
    }
}

int Element:: getRequiredInt(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    if( hasAttr ( attrname, pAttributes ) )
    {
        return getInt ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname );
    }

}

float Element::getRequiredFloat(const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{

    if( hasAttr ( attrname, pAttributes ) )
    {
        return getFloat ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname );
    }

}

std::string Element::getRequiredString (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    if( hasAttr ( attrname, pAttributes ) )
    {
        return getString ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname );
    }

    return "";
}

bool Element::getRequiredBool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    if( hasAttr ( attrname, pAttributes ) )
    {
        return getBool ( attrname, pAttributes );
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname );
    }

    return false;
}

bool Element::getBool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    std::string str = getString ( attrname, pAttributes);

    if(str == "true") return true;
    else if (str == "false") return false;
    else throw CL_Error("Boolean value for " + attrname + " must be 'true' or 'false'.");

    return false;
}

bool Element::getImpliedBool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, bool defaultValue)
{
    if(hasAttr(attrname, pAttributes))
    {
        return getBool ( attrname, pAttributes);
    }
    else return defaultValue;
}

int Element::getImpliedInt( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, int defaultValue)
{
    if(hasAttr(attrname, pAttributes))
    {
        return getInt(attrname, pAttributes );
    }
    else return defaultValue;
}

std::string Element::getImpliedString( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, const std::string &defaultValue)
{
    if(hasAttr(attrname,pAttributes))
    {
        return getString(attrname,pAttributes);
    }
    else return defaultValue;
}


float Element::getImpliedFloat(const std::string &attrname, CL_DomNamedNodeMap *pAttributes, float defaultValue)
{
    if(hasAttr(attrname,pAttributes))
    {
        return getFloat(attrname,pAttributes);
    }
    else return defaultValue;
}
bool Element::hasAttr( const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return ! pAttributes->get_named_item(attrname).is_null();
}

uint Element::getUint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    return atoi(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

int Element:: getInt(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    return atoi(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

float Element::getFloat(const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return atof(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

std::string Element::getString (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return pAttributes->get_named_item(attrname).get_node_value();
}
    

void Element::load(CL_DomElement * pDomElement)
{
    const int n = sizeof(pszElementNames) / sizeof(const char *);

    std::vector<IFactory*> factories;

    IFactory * pItemFactory = IApplication::getInstance()->getItemFactory();

    factories.push_back(pItemFactory);
    factories.push_back(IApplication::getInstance()->getAbilityFactory());
    factories.push_back(IApplication::getInstance()->getLevelFactory());

                
    loadAttributes(&pDomElement->get_attributes());

    CL_DomElement child = pDomElement->get_first_child().to_element();  

    while(!child.is_null())
    {
        std::string element_name = child.get_node_name();
        const char ** found_string = std::lower_bound(pszElementNames, pszElementNames + n, element_name.c_str(), std::less<std::string>());
                
        if((*found_string) == (const char*)(pszElementNames + n))
        {
            // It wasn't found.
            throw CL_Error("Element " + element_name + " was not found in pszElementNames.");
        }
        else
        {
            int index = found_string - pszElementNames;
                
            cl_assert( pszElementNames[index] == *found_string );

            eElement element = static_cast<eElement>(index);

            Element * pElement = NULL;

            for(std::vector<IFactory*>::iterator iter = factories.begin();
                iter != factories.end();
                iter++)
            {
                IFactory * pFactory = *iter;
                                        
                if(pFactory->canCreate(element))
                {
                    pElement = pFactory->createElement(element);
                    break;
                }
                                        
            }
                        
            cl_assert ( pElement );

            pElement->load( &child );

            handleElement(element, pElement );

        }


        child = child.get_next_sibling().to_element();
    }

    handleText ( pDomElement->get_text() );

    loadFinished();
}

bool Element::isAction(Element::eElement element) const
{
    switch(element)
    {
    case EPLAYANIMATION:
    case EPLAYSOUND:
    case ELOADLEVEL:
    case ESTARTBATTLE:
    case EINVOKESHOP:
    case EPAUSE:
    case ESAY:
    case EGIVE:
    case ETAKE:
    case EGIVEGOLD:
    case ECHOICE:
        // case EUSE:
        return true;
    default:
        return false;
    }

}

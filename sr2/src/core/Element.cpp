#include <ClanLib/core.h>
#include "Element.h"

#include "IApplication.h"
#include "LevelFactory.h"
#include <algorithm>
#include "ItemFactory.h"
#include "CharacterFactory.h"


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



uint Element::getRequiredUint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    if( hasAttr ( attrname, pAttributes ) )
    {
        return getUint ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname + " on " + getElementName() );
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
        throw CL_Error("Missing attribute " + attrname + " on " + getElementName() );
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
        throw CL_Error("Missing attribute " + attrname + " on " + getElementName() );
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

        throw CL_Error("Missing attribute " + attrname + " on " + getElementName() );
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
        throw CL_Error("Missing attribute " + attrname + " on " + getElementName() );
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

std::string Element::getElementName() const
{
   // return pszElementNames[ whichElement() ];
    return "";
}
    

void Element::load(CL_DomElement * pDomElement)
{
    IFactory* pFactory = IApplication::getInstance()->getElementFactory();

    CL_DomNamedNodeMap attributes = pDomElement->get_attributes();

                
    loadAttributes(&attributes);

    CL_DomNode childNode = pDomElement->get_first_child(); //.to_element();  
    CL_DomElement child;


    if(childNode.is_text())
    {
        CL_DomText text = childNode.to_text();
        handleText(text.get_node_value());
    }

    child = childNode.to_element();
    

    while(!child.is_null())
    {
        std::string element_name = child.get_node_name();
        Element * pElement = NULL;
                                        
        pElement = pFactory->createElement(element_name);
                                 
        pElement->load( &child );

        if(!handleElement(pElement->whichElement(), pElement ))
        {
            // They didn't handle it. So lets get rid of it
            std::cout << "Unhandled element " << element_name << " found" << std::endl;
            delete pElement;
        }

        
        if(child.get_next_sibling().is_text())
            std::cout << "Found Text" << std::endl;

        child = child.get_next_sibling().to_element();
        
    }

    

#if 0
    if(pDomElement->is_text())
    {
        CL_DomCDATASection cdata = pDomElement->to_text();
#ifndef NDEBUG
        if(!cdata.is_null())
        {
            std::string theText = cdata.substring_data(0,text.get_length());
            std::cout << '\'' << theText  << '\'' << std::endl;
        }
#endif
        handleText (  cdata.substring_data(0,cdata.length()) );
    }
#endif

    loadFinished();
}



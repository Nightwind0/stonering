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



uint Element::get_required_uint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    if( has_attribute ( attrname, pAttributes ) )
    {
        return get_uint ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname + " on " + get_element_name() );
    }
}

int Element:: get_required_int(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    if( has_attribute ( attrname, pAttributes ) )
    {
        return get_int ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname + " on " + get_element_name() );
    }

}

float Element::get_required_float(const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{

    if( has_attribute ( attrname, pAttributes ) )
    {
        return get_float ( attrname, pAttributes);
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname + " on " + get_element_name() );
    }

}

std::string Element::get_required_string (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    if( has_attribute ( attrname, pAttributes ) )
    {
        return get_string ( attrname, pAttributes);
    }
    else
    {

        throw CL_Error("Missing attribute " + attrname + " on " + get_element_name() );
    }

    return "";
}

bool Element::get_required_bool (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    if( has_attribute ( attrname, pAttributes ) )
    {
        return get_bool ( attrname, pAttributes );
    }
    else
    {
        throw CL_Error("Missing attribute " + attrname + " on " + get_element_name() );
    }

    return false;
}

bool Element::get_bool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    std::string str = get_string ( attrname, pAttributes);

    if(str == "true") return true;
    else if (str == "false") return false;
    else throw CL_Error("Boolean value for " + attrname + " must be 'true' or 'false'.");

    return false;
}

bool Element::get_implied_bool ( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, bool defaultValue)
{
    if(has_attribute(attrname, pAttributes))
    {
        return get_bool ( attrname, pAttributes);
    }
    else return defaultValue;
}

int Element::get_implied_int( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, int defaultValue)
{
    if(has_attribute(attrname, pAttributes))
    {
        return get_int(attrname, pAttributes );
    }
    else return defaultValue;
}

std::string Element::get_implied_string( const std::string &attrname, CL_DomNamedNodeMap * pAttributes, const std::string &defaultValue)
{
    if(has_attribute(attrname,pAttributes))
    {
        return get_string(attrname,pAttributes);
    }
    else return defaultValue;
}


float Element::get_implied_float(const std::string &attrname, CL_DomNamedNodeMap *pAttributes, float defaultValue)
{
    if(has_attribute(attrname,pAttributes))
    {
        return get_float(attrname,pAttributes);
    }
    else return defaultValue;
}
bool Element::has_attribute( const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return ! pAttributes->get_named_item(attrname).is_null();
}

uint Element::get_uint(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    return atoi(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

int Element::get_int(const std::string &attrname, CL_DomNamedNodeMap * pAttributes)
{
    return atoi(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

float Element::get_float(const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return atof(pAttributes->get_named_item(attrname).get_node_value().c_str());
}

std::string Element::get_string (const std::string &attrname, CL_DomNamedNodeMap * pAttributes )
{
    return pAttributes->get_named_item(attrname).get_node_value();
}
#ifndef NDEBUG
std::string Element::get_element_name() const
{
    return m_element_name;
}
#endif

void Element::Load(CL_DomElement * pDomElement)
{
    IFactory* pFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomNamedNodeMap attributes = pDomElement->get_attributes();
                
    load_attributes(&attributes);

    CL_DomNode childNode = pDomElement->get_first_child(); //.to_element();  
    CL_DomElement child;


    if(childNode.is_text())
    {
        CL_DomText text = childNode.to_text();
        handle_text(text.get_node_value());
    }

    child = childNode.to_element();
    

    while(!child.is_null())
    {
        std::string element_name = child.get_node_name();
        Element * pElement = NULL;
                                        
        pElement = pFactory->createElement(element_name);
#ifndef NDEBUG
        pElement->SetElementName(element_name);
#endif
        pElement->Load( &child );

        if(!handle_element(pElement->WhichElement(), pElement ))
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

    load_finished();
}





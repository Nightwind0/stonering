#include <ClanLib/core.h>
#include "Element.h"



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

    return "";
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
    


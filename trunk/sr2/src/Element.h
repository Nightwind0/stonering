#ifndef ELEMENT_H
#define ELEMENT_H

#include "sr_defines.h"

#include <ClanLib/core.h>


#define GET_CHILD pElement->get_first_child().to_element();

class Element
{
public:
    Element(){}
    ~Element(){}
    virtual CL_DomElement  createDomElement(CL_DomDocument&) const=0;
    
protected:
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

private:
};




#endif

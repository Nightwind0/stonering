#ifndef ELEMENT_H
#define ELEMENT_H



#include <ClanLib/core.h>


class Element
{
public:
    Element(){}
    ~Element(){}
    virtual CL_DomElement  createDomElement(CL_DomDocument&) const=0;
    
private:
};




#endif

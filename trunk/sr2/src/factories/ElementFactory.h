#ifndef SR_ELEMENT_FACTORY_H
#define SR_ELEMENT_FACTORY_H

#include <ClanLib/core.h>
#include <map>
#include "IFactory.h"

namespace StoneRing
{

    class ElementFactory : public IFactory
    {
    public:
        ElementFactory();
        virtual ~ElementFactory(){}

        virtual Element * createElement( const std::string &element_name );
    protected:

        virtual Element * createConditionScript() const;
        virtual Element * createScriptElement() const;
		virtual Element * createAnimationScript() const;
     
	template <class T>Element* CreateElement()const;
	virtual void registerCreateMethods();
        typedef Element * (ElementFactory::* CreateMethod)() const;
        typedef std::map<std::string,CreateMethod> MethodMap;

        MethodMap mCreateMethods;

    };
}

#endif




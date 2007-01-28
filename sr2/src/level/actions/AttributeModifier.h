#ifndef SR_ATTRIBUTE_MODIFIER_H
#define SR_ATTRIBUTE_MODIFIER_H

#include "Element.h"

namespace StoneRing{

    class ScriptElement;

    class AttributeModifier : public Element
    {
    public:
        AttributeModifier();
        virtual ~AttributeModifier();
        virtual eElement whichElement() const{ return EATTRIBUTEMODIFIER; } 
        enum eTarget { CURRENT, ALL, CASTER, COMMON };
        enum eChangeTo { ADD, TO_MIN, TO_MAX, MULTIPLIER, MULTIPLY_ADD };

        virtual void invoke();

        bool applicable() const;

        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;


    protected:
        virtual bool handleElement(eElement element, Element * pElement );      
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        ScriptElement* mpCondition;
        int mAdd;
        float mfMultiplier;
        uint mnAttribute;
        eTarget  meTarget;
        eChangeTo meChangeTo;
    };

};
#endif



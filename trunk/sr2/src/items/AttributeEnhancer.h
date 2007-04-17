#ifndef SR_ATTRIBUTE_ENHANCER_H
#define SR_ATTRIBUTE_ENHANCER_H

#include "Element.h"

namespace StoneRing{
    class AttributeEnhancer : public Element
    {
    public:
        AttributeEnhancer();
        virtual ~AttributeEnhancer();
        virtual eElement whichElement() const{ return EATTRIBUTEENHANCER; } 

        uint getAttribute() const;
        int getAdd() const;
        float getMultiplier() const;

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when invoking. (By calling equip on the armor/weapon...)
        void invoke();

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when revoking. (By calling unequip on the armor/weapon...)
        void revoke();

        virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        // Used to make sure that when we multiply the value to get it
        // back to what it was, we end up with the right values.
        int mnDelta;
        int mnAdd;
        float mfMultiplier;
        uint mnAttribute;
        
    };
};

#endif




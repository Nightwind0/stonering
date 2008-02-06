#ifndef SR_ATTRIBUTE_ENHANCER_H
#define SR_ATTRIBUTE_ENHANCER_H

#include "Element.h"

namespace StoneRing{

    class ICharacter;

    class AttributeEnhancer : public Element
    {
    public:
        AttributeEnhancer();
        virtual ~AttributeEnhancer();
        virtual eElement whichElement() const{ return EATTRIBUTEENHANCER; } 

        enum eType
        {
            EADD=1,
            EMULTIPLY=2,
            ETOGGLE=4,
            EADD_MULTIPLY = (EADD | EMULTIPLY),
            EAUTO = 0
        }; 

        eType getType() const;
        uint getAttribute() const;
        int getAdd() const;
        float getMultiplier() const;
        bool getToggle() const;

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when invoking. (By calling equip on the armor/weapon...)
        void invoke();

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when revoking. (By calling unequip on the armor/weapon...)
        void revoke();
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        // Used to make sure that when we multiply the value to get it
        // back to what it was, we end up with the right values.
        eType meType;
        int mnDelta;
        int mnAdd;
        float mfMultiplier;
        uint mnAttribute;
        bool mbToggle;
        
    };
};

#endif




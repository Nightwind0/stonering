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
        virtual eElement WhichElement() const{ return EATTRIBUTEENHANCER; } 

        enum eType
        {
            EADD=1,
            EMULTIPLY=2,
            ETOGGLE=4,
            EADD_MULTIPLY = (EADD | EMULTIPLY),
            EAUTO = 0
        }; 

        eType GetType() const;
        uint GetAttribute() const;
        int GetAdd() const;
        float GetMultiplier() const;
        bool GetToggle() const;

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when invoking. (By calling equip on the armor/weapon...)
        void Invoke();

        // Uses IParty::modifyAttribute to modify the CURRENT player,
        // Meaning that the system must select the proper current player
        // when revoking. (By calling unequip on the armor/weapon...)
        void Revoke();
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        // Used to make sure that when we multiply the value to get it
        // back to what it was, we end up with the right values.
        eType m_eType;
        int m_nDelta;
        int m_nAdd;
        float m_fMultiplier;
        uint m_nAttribute;
        bool m_bToggle;
        
    };
};

#endif




#ifndef SR_ATTRIBUTE_MODIFIER_H
#define SR_ATTRIBUTE_MODIFIER_H

#include "Element.h"
#include "Character.h"

class AstScript;

namespace StoneRing{

    class ScriptElement;

    class AttributeModifier : public Element
    {
    public:
        AttributeModifier();
        virtual ~AttributeModifier();
        virtual eElement WhichElement() const{ return EATTRIBUTEMODIFIER; }

        enum eType
        {
            EADD,
            EMULTIPLY,
            ETOGGLE
        };

        eType GetType() const;
        uint GetAttribute() const;
        double GetAdd() const;
        double GetMultiplier() const;
        bool GetToggle() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        virtual bool handle_element(eElement, Element * );
        virtual void load_finished();
        // Used to make sure that when we multiply the value to get it
        // back to what it was, we end up with the right values.
        eType m_eType;
        uint m_nAttribute;
        union ValueType{
            double m_float;
            int m_int;
            bool m_toggle;
        };
        ValueType m_value;
        bool m_has_value;
        ScriptElement* m_pScript;
    };
};

#endif

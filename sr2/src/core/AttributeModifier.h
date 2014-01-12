#ifndef SR_ATTRIBUTE_MODIFIER_H
#define SR_ATTRIBUTE_MODIFIER_H

#include "Element.h"
#include "Character.h"

class AstScript;

namespace StoneRing{

    class ScriptElement;

    class AttributeModifier 
    {
    public:
        AttributeModifier();
        virtual ~AttributeModifier();

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
        void SetAttribute(uint attribute);
        void SetMultiplier(double value);
        void SetAdd(double value);
        void SetToggle(bool toggle);
    protected:

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

    };
    
    class AttributeModifierElement : public Element, public AttributeModifier
    {
    public:
        AttributeModifierElement();
        virtual ~AttributeModifierElement();
        virtual eElement WhichElement() const{ return EATTRIBUTEMODIFIER; }
        double GetAdd() const;
        double GetMultiplier() const;
        bool GetToggle() const;
		virtual std::string GetDebugId() const { return ""; }						
    private:
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        virtual bool handle_element(eElement, Element * );
        virtual void load_finished();
        bool m_has_value;
        ScriptElement* m_pScript;
    };
};

#endif

#ifndef SR_ATTRIBUTE_EFFECT_H
#define SR_ATTRIBUTE_EFFECT_H

#include "Element.h"
#include "ScriptElement.h"
#include "NamedScript.h"

namespace StoneRing{

    class StatusEffect : public Element
    {
    public:
        StatusEffect();
        virtual ~StatusEffect();
        virtual eElement WhichElement() const{ return ESTATUSEFFECT; }  

        OnInvoke * GetOnInvoke() const;
        OnRound * GetOnRound() const;
        OnCountdown * GetOnCountdown() const;
        OnRemove * GetOnRemove() const;

        std::string GetName() const;
        enum eLast { ROUND_COUNT, BATTLE, PERMANENT };
        eLast GetLast() const;
        uint GetRoundCount() const; 

        // Multiply the magic power of the user by this using an algorithm to get length..
        float GetLengthMultiplier() const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        std::string m_name;
        OnInvoke * m_pOnInvoke;
        OnRound * m_pOnRound;
        OnCountdown * m_pOnCountdown;
        OnRemove * m_pOnRemove;
        eLast m_eLast;
        uint m_nRoundCount;
        float m_fLengthMultiplier;
    };
}

#endif





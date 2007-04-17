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
        virtual eElement whichElement() const{ return ESTATUSEFFECT; }  
        CL_DomElement createDomElement(CL_DomDocument &doc) const;

        OnInvoke * getOnInvoke() const;
        OnRound * getOnRound() const;
        OnCountdown * getOnCountdown() const;
        OnRemove * getOnRemove() const;

        std::string getName() const;

        enum eLast { ROUND_COUNT, BATTLE, PERMANENT };

        eLast getLast() const;

        uint getRoundCount() const; 

        // Multiply the magic power of the user by this using an algorithm to get length..
        float getLengthMultiplier() const;


    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        std::string mName;
        OnInvoke * mpOnInvoke;
        OnRound * mpOnRound;
        OnCountdown * mpOnCountdown;
        OnRemove * mpOnRemove;
        eLast meLast;
        uint mnRoundCount;
        float mfLengthMultiplier;
    };
}

#endif





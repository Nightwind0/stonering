#ifndef SR_STAT_H
#define SR_STAT_H

#include "Element.h"
#include "Character.h"

namespace StoneRing
{
    class Stat : public Element
    {
    public:
        Stat();
        virtual ~Stat();

        double GetStat() const;
        bool GetToggle() const;
        ICharacter::eCharacterAttribute GetAttribute() const;

        eElement WhichElement()const { return ESTAT; }
    private:
        virtual void load_attributes(CL_DomNamedNodeMap);
        ICharacter::eCharacterAttribute m_eAttr;
        double m_fValue;
        bool m_bToggle;
    };
}\


#endif

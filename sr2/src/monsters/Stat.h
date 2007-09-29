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

        double getStat() const;
        bool getToggle() const;
        eCharacterAttribute getAttribute() const;

        eElement whichElement()const { return ESTAT; }

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        eCharacterAttribute meAttr;
        double mfValue;
        bool mbToggle;
    };
};


#endif
#ifndef SR_MONSTER_REF_H
#define SR_MONSTER_REF_H

#include "Element.h"

namespace StoneRing
{
    class MonsterRef : public Element
    {
    public:
        MonsterRef();
        virtual ~MonsterRef();

        std::string getName() const;
        int getCount() const;

        virtual eElement whichElement() const { return EMONSTERREF; }
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void loadFinished();
        
        std::string mName;
        int mnCount;
    };
}

#endif
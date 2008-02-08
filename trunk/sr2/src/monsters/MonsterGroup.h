#ifndef SR_MONSTER_GROUP_H
#define SR_MONSTER_GROUP_H   

#include "Element.h"


namespace StoneRing{

    class MonsterRef;

    class MonsterGroup : public Element
    {
    public:
        MonsterGroup();
        virtual ~MonsterGroup();
        
        virtual eElement whichElement() const { return EMONSTERGROUP; }
        int getEncounterWeight() const { return mnWeight; }
        const std::vector<MonsterRef*> & getMonsters() const { return mMonsters; }
    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void loadFinished();

        int mnWeight;
        std::vector<MonsterRef*> mMonsters;
    };
}
#endif
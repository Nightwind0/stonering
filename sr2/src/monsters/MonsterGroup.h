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
        int getCellColumns() const { return mnCellCols; }
        int getCellRows() const { return mnCellRows; }
        const std::vector<MonsterRef*> & getMonsters() const { return mMonsters; }
    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void loadFinished();

        int mnWeight;
        int mnCellCols;
        int mnCellRows;
        std::vector<MonsterRef*> mMonsters;
    };
}
#endif
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
        int getCellX() const { return mnCellX; }
        int getCellY() const { return mnCellY; }
        int getColumns() const { return mnColumns; }
        int getRows() const { return mnRows; }
        virtual eElement whichElement() const { return EMONSTERREF; }
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void loadFinished();
        
        std::string mName;
        int mnCount;
        int mnCellX;
        int mnCellY;
        int mnColumns;
        int mnRows;
    };
}

#endif
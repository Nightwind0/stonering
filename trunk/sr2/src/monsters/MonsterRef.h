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

        std::string GetName() const;
        int GetCount() const;
        int GetCellX() const { return m_nCellX; }
        int GetCellY() const { return m_nCellY; }
        int GetColumns() const { return m_nColumns; }
        int GetRows() const { return m_nRows; }
        virtual eElement WhichElement() const { return EMONSTERREF; }
    private:
        virtual void load_attributes(CL_DomNamedNodeMap);
        virtual void load_finished();

        std::string m_name;
        int m_nCount;
        int m_nCellX;
        int m_nCellY;
        int m_nColumns;
        int m_nRows;
    };
}

#endif


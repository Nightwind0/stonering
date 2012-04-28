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

        virtual eElement WhichElement() const { return EMONSTERGROUP; }
        int GetEncounterWeight() const { return m_nWeight; }
        int GetCellColumns() const { return m_nCellCols; }
        int GetCellRows() const { return m_nCellRows; }
        const std::vector<MonsterRef*> & GetMonsters() const { return m_monsters; }
#if SR2_EDITOR
        CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
#endif
    private:
        virtual bool handle_element(eElement, Element * );
        virtual void load_attributes(CL_DomNamedNodeMap);
        virtual void load_finished();

        int m_nWeight;
        int m_nCellCols;
        int m_nCellRows;
        std::vector<MonsterRef*> m_monsters;
    };
}
#endif

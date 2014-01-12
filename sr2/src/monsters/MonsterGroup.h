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
		virtual std::string GetDebugId() const { return ""; }				

    private:
        virtual bool handle_element(eElement, Element * );
        virtual void load_attributes(clan::DomNamedNodeMap);
        virtual void load_finished();

        int m_nWeight;
        int m_nCellCols;
        int m_nCellRows;
        std::vector<MonsterRef*> m_monsters;
#if SR2_EDITOR
	public:
        clan::DomElement CreateDomElement(clan::DomDocument& doc)const;
		void SetCellColumns(int cols){m_nCellCols = cols;}
		void SetCellRows(int rows){m_nCellRows = rows;}
		void SetWeight(int weight){m_nWeight = weight;}
		void AddMonster(MonsterRef* pRef){m_monsters.push_back(pRef);}
		void SetEncounterWeight(int weight){ m_nWeight = weight; } 
		void RemoveMonsters(int index){m_monsters.erase(m_monsters.begin()+index);}
#endif		
    };
}
#endif

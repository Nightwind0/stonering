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
		virtual std::string GetDebugId() const { return m_name; }				
        
#if SR2_EDITOR
        CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
		void SetCellX(int x){m_nCellX = x;}
		void SetCellY(int y){m_nCellY = y;}
#endif
    private:
        virtual void load_attributes(CL_DomNamedNodeMap);
        virtual void load_finished();
    protected:
        std::string m_name;
        int m_nCount;
        int m_nCellX;
        int m_nCellY;
        int m_nColumns;
        int m_nRows;
    };
    
    class DynamicMonsterRef : public MonsterRef 
    {
    public:
	DynamicMonsterRef();
	virtual ~DynamicMonsterRef();
	
	void SetName(const std::string &name);
	void SetCount(int count);
	void SetCellX(int cell_x);
	void SetCellY(int cell_y);
	void SetColumns(int cols);
	void SetRows(int rows);
    };
}

#endif


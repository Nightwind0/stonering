#ifndef MONSTER_REGION_H
#define MONSTER_REGION_H

#include "sr_defines.h"
#include "Element.h"
#include <list>

namespace StoneRing
{

    class MonsterGroup;

    class MonsterRegion : public Element
    {
        public:
            MonsterRegion();
            virtual ~MonsterRegion();

            virtual eElement WhichElement() const { return EMONSTERREGION; }

            MonsterGroup * GetMonsterGroup() const;

            int GetLevelX() const {return m_LevelX;}
            int GetLevelY() const {return m_LevelY;}
            int GetWidth() const {return m_Width;}
            int GetHeight() const {return m_Height;}
            float GetEncounterRate() const {return m_encounter_rate;}
            std::string GetBackdrop() const { return m_backdrop; }
        private:
            virtual bool handle_element(eElement, Element * );
            virtual void load_attributes(CL_DomNamedNodeMap);
            virtual void load_finished();

            int m_LevelX;
            int m_LevelY;
            int m_Width;
            int m_Height;
            float m_encounter_rate;
            int m_nTotalWeight;
			uchar m_id;
            std::string m_backdrop;
            std::list<MonsterGroup*> m_monster_groups;
#if SR2_EDITOR
	public:
			std::list<MonsterGroup*>::const_iterator GetMonsterGroupsBegin() const{
				return m_monster_groups.begin();
			}
			std::list<MonsterGroup*>::const_iterator GetMonsterGroupsEnd() const{
				return m_monster_groups.end();
			}
            CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
			void SetEncounterRate(float rate){ m_encounter_rate = rate; }
			void SetBackdrop(const std::string& backdrop){ m_backdrop = backdrop; }
			void AddMonsterGroup(MonsterGroup* group){ m_monster_groups.push_back(group); }
			void SetId(uchar id){ m_id = id; }
			uchar GetId() const { return m_id; }
#endif
    };

    class MonsterRegions : public Element
    {
    public:
        MonsterRegions();
        virtual ~MonsterRegions();

        MonsterRegion * GetApplicableRegion(uint levelx, uint levely) const ;
		MonsterRegion * GetMonsterRegion(uchar id){
			return m_monster_regions[id];
		}

        virtual eElement WhichElement() const { return EMONSTERREGIONS; }
#if SR2_EDITOR
        CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
		std::map<uchar,MonsterRegion*>::const_iterator GetRegionsBegin() const{
			return m_monster_regions.begin();
		}
		std::map<uchar,MonsterRegion*>::const_iterator GetRegionsEnd() const{
			return m_monster_regions.end();
		}
		void AddMonsterRegion(MonsterRegion* region){
			region->SetId(get_next_id());
			m_monster_regions[region->GetId()] = region;
		}
		void RemoveMonsterRegion(MonsterRegion* region){
			m_monster_regions.erase(region->GetId());
		}
#endif
    private:
        virtual bool handle_element(eElement, Element * );
        virtual void load_finished();
		uchar get_next_id() const;

        std::map<uchar,MonsterRegion *> m_monster_regions;
    };



};

#endif


#ifndef MONSTER_REGION_H
#define MONSTER_REGION_H

#include "Element.h"

namespace StoneRing
{

    class MonsterGroup;

    class MonsterRegion : public Element
    {
        public:
            MonsterRegion();
            virtual ~MonsterRegion();
        
            virtual eElement whichElement() const { return EMONSTERREGION; }
 
            MonsterGroup * getMonsterGroup() const;
        
            int getLevelX() const {return mLevelX;}
            int getLevelY() const {return mLevelY;}
            int getWidth() const {return mWidth;}
            int getHeight() const {return mHeight;}
            float getEncounterRate() const {return mEncounterRate;}
            std::string getBackdrop() const { return mBackdrop; }
        private:
            virtual bool handleElement(eElement, Element * );
            virtual void loadAttributes(CL_DomNamedNodeMap *);
            virtual void loadFinished();
        
            int mLevelX;
            int mLevelY;
            int mWidth;
            int mHeight;
            float mEncounterRate;
            int mnTotalWeight;
            std::string mBackdrop;
            std::list<MonsterGroup*> mMonsterGroups;
    
    };

    class MonsterRegions : public Element
    {
    public:
        MonsterRegions();
        virtual ~MonsterRegions();

        MonsterRegion * getApplicableRegion(uint levelx, uint levely) const ;

        virtual eElement whichElement() const { return EMONSTERREGIONS; }
    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadFinished();

        std::list<MonsterRegion *> mMonsterRegions;
        int mnTotalWeight;
    };
    


};

#endif


#ifndef MONSTER_REGION_H
#define MONSTER_REGION_H

namespace stonering
{
    class MonsterRegion : public Element
    {
        public:
            MonsterRegion();
            virtual ~MonsterRegion();
        
            virtual eElement whichElement() const { return EMONSTERREGION; }
            std::string getName() const { return mName; }
            MonsterGroup getMonsterGroup();
        
            int getLevelX() const {return mLevelX;}
            int getLevelY() const {return mLevelY;}
            int getWidth() const {return mWidth;}
            int getHeight() const {return mHeight;}
            float getEncounterRate() const {return mEncounterRate;}
        
        private:
            virtual bool handleElement(eElement, Element * );
            virtual void loadAttributes(CL_DomNamedNodeMap *);
            virtual void loadFinished();
        
            std::string mName;
            int mLevelX;
            int mLevelY;
            int mWidth;
            int mHeight;
            float mEncounterRate;
            std::list<MonsterGroup*> mMonsterGroups;
            
        
    };
    
    class MonsterGroup : public Element
    {
        public:
            MonsterGroup();
            virtual ~MonsterGroup();
        
            virtual eElement whichElement() const { return EMONSTERGROUP; }
            std::string getName() const { return mName; }
        
        private:
            std::string mName;
    };

};

#endif


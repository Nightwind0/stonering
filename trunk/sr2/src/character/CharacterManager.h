#ifndef SR_CHARACTER_MANAGER_H
#define SR_CHARACTER_MANAGER_H

#include <string>
#include <map>
#include <ClanLib/core.h>

namespace StoneRing{

    class CharacterClass;
    class Character;
    class Monster;

    class CharacterManager
    {
    public:
        CharacterManager(){}
        ~CharacterManager();
        
        CharacterClass * getClass(const std::string &name)const;
        Monster * getMonster(const std::string &name)const;

        void loadCharacterClassFile (CL_DomDocument  &doc);
        void loadCharacters(CL_DomDocument &doc);
        void loadMonsterFile(CL_DomDocument &doc);
    private:
        typedef std::map<std::string,CharacterClass*> ClassMap;
        typedef std::map<std::string,Character*> CharacterMap;
        typedef std::map<std::string,Monster*> MonsterMap; // Do the  map. Do the monster map
        
        ClassMap mCharacterClasses;
        CharacterMap mCharacters;
        MonsterMap mMonsters;
        
    };
    
}
#endif

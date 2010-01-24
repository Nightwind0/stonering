#ifndef SR_CHARACTER_MANAGER_H
#define SR_CHARACTER_MANAGER_H

#include <string>
#include <map>
#include <ClanLib/core.h>

namespace StoneRing{

    class CharacterClass;
    class Character;
    class MonsterElement;
    class Monster;

    class CharacterManager
    {
    public:
        CharacterManager(){}
        ~CharacterManager();
        
        CharacterClass * GetClass(const std::string &name)const;
        MonsterElement * GetMonsterElement(const std::string &name)const;
        Character * GetCharacter(const std::string &name)const;
        Monster * CreateMonster(const std::string &name)const;

        void LoadCharacterClassFile (CL_DomDocument  &doc);
        void LoadCharacters(CL_DomDocument &doc);
        void LoadMonsterFile(CL_DomDocument &doc);
    private:
        typedef std::map<std::string,CharacterClass*> ClassMap;
        typedef std::map<std::string,Character*> CharacterMap;
        typedef std::map<std::string,MonsterElement*> MonsterMap; // Do the  map. Do the monster map
        
        ClassMap m_character_classes;
        CharacterMap m_characters;
        MonsterMap m_monsters;
        
    };
    
}
#endif

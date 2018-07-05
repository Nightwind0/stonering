#ifndef SR_CHARACTER_MANAGER_H
#define SR_CHARACTER_MANAGER_H

#include <string>
#include <map>
#include <list>
#include <ClanLib/core.h>

namespace StoneRing{

    class CharacterClass;
    class Character;
    class MonsterElement;
    class Monster;

    class CharacterManager
    {
    public:
        static void initialize();
        
        static CharacterClass * GetClass(const std::string &name);
        static MonsterElement * GetMonsterElement(const std::string &name);
        static Character * GetCharacter(const std::string &name);
        static Monster * CreateMonster(const std::string &name);
		
		static void GetMonsterList(std::list<MonsterElement*>& o_list);
        static void LoadCharacterClassFile (clan::DomDocument  &doc);
        static void LoadCharacters(clan::DomDocument &doc);
        static void LoadMonsterFile(clan::DomDocument &doc);
    private:
        typedef std::map<std::string,CharacterClass*> ClassMap;
        typedef std::map<std::string,Character*> CharacterMap;
        typedef std::map<std::string,MonsterElement*> MonsterMap; // Do the  map. Do the monster map
        
        ClassMap m_character_classes;
        CharacterMap m_characters;
        MonsterMap m_monsters;
        static CharacterManager* m_pInstance;
        CharacterManager(){}
        ~CharacterManager();        
    };
    
}
#endif

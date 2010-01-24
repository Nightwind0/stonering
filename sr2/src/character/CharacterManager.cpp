#include "CharacterManager.h"
#include "CharacterClass.h"
#include "Character.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "MonsterElement.h"
#include "Monster.h"
#include <algorithm>

using StoneRing::CharacterManager;

CharacterManager::~CharacterManager()
{
    std::for_each(m_character_classes.begin(),m_character_classes.end(),
                  compose_f_gx(del_fun<CharacterClass>(),
                               get_second<ClassMap::value_type>())
        );
}

StoneRing::MonsterElement * CharacterManager::GetMonsterElement(const std::string &name)const
{
    MonsterMap::const_iterator it = m_monsters.find(name);
    if(it == m_monsters.end()) throw CL_Exception("Monster " + name + " not found in manager");
    return it->second;
}

StoneRing::Monster * CharacterManager::CreateMonster(const std::string &name)const
{
    MonsterElement * pElement = GetMonsterElement(name);
    if(pElement == NULL) return NULL;
	return new StoneRing::Monster(pElement);
}

void CharacterManager::LoadCharacterClassFile ( CL_DomDocument &doc )
{
    IFactory * pFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement classesNode = doc.named_item("characterClasses").to_element();
    CL_DomElement classNode = classesNode.get_first_child().to_element();

    while(!classNode.is_null())
    {
        CharacterClass * pCharacterClass = dynamic_cast<CharacterClass*>
            (pFactory->createElement("characterClass"));

        pCharacterClass->Load(classNode);
        m_character_classes [ pCharacterClass->GetName() ] = pCharacterClass;
        classNode = classNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Class: " << pCharacterClass->GetName() << std::endl;
#endif
    }


}
void CharacterManager::LoadMonsterFile ( CL_DomDocument &doc )
{
    IFactory * pFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement monstersNode = doc.named_item("monsters").to_element();
    assert(monstersNode.is_element());
    assert(!monstersNode.is_null());
    CL_DomElement monsterNode = monstersNode.get_first_child().to_element();
    assert(monsterNode.is_element());

    while(!monsterNode.is_null())
    {
        MonsterElement * pMonster = dynamic_cast<MonsterElement*>
            (pFactory->createElement("monster"));

        pMonster->Load(monsterNode);
        m_monsters [ pMonster->GetName() ] = pMonster;
        monsterNode = monsterNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Monster: " << '\'' << pMonster->GetName() << '\'' << std::endl;
#endif
    }


}

void CharacterManager::LoadCharacters(CL_DomDocument &doc)
{
    IFactory * pFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement charactersNode = doc.named_item("characters").to_element();
    assert(charactersNode.is_element());
    assert(!charactersNode.is_null());
    CL_DomElement characterNode = charactersNode.get_first_child().to_element();
    assert(characterNode.is_element());

    while(!characterNode.is_null())
    {
        Character * pCharacter = dynamic_cast<Character*>
            (pFactory->createElement(characterNode.get_node_name()));

        assert(pCharacter);
        assert(pCharacter->WhichElement() == Element::ECHARACTER);
        pCharacter->Load(characterNode);
        m_characters [ pCharacter->GetName() ] = pCharacter;
        characterNode = characterNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Character: " << '\'' << pCharacter->GetName() << '\'' << std::endl;
#endif
    }

}

StoneRing::Character * CharacterManager::GetCharacter(const std::string &name)const
{
    return m_characters.find(name)->second;
}

StoneRing::CharacterClass * CharacterManager::GetClass ( const std::string &cls ) const
{
    return m_character_classes.find(cls)->second;
}




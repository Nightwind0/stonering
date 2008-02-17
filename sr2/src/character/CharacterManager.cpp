#include "CharacterManager.h"
#include "CharacterClass.h"
#include "Character.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Monster.h"

using StoneRing::CharacterManager;

CharacterManager::~CharacterManager()
{
    std::for_each(mCharacterClasses.begin(),mCharacterClasses.end(),
                  compose_f_gx(del_fun<CharacterClass>(),
                               get_second<ClassMap::value_type>())
        );
}

StoneRing::Monster * CharacterManager::getMonster(const std::string &name)const
{
    MonsterMap::const_iterator it = mMonsters.find(name);
    if(it == mMonsters.end()) throw CL_Error("Monster " + name + " not found in manager");
    return it->second;
}

void CharacterManager::loadCharacterClassFile ( CL_DomDocument &doc )
{
    IFactory * pFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement classesNode = doc.named_item("characterClasses").to_element();
    CL_DomElement classNode = classesNode.get_first_child().to_element();

    while(!classNode.is_null())
    {
        CharacterClass * pCharacterClass = dynamic_cast<CharacterClass*>
            (pFactory->createElement("characterClass"));

        pCharacterClass->load(&classNode);
        mCharacterClasses [ pCharacterClass->getName() ] = pCharacterClass;
        classNode = classNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Class: " << pCharacterClass->getName() << std::endl;
#endif
    }


}
void CharacterManager::loadMonsterFile ( CL_DomDocument &doc )
{
    IFactory * pFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement monstersNode = doc.named_item("monsters").to_element();
    assert(monstersNode.is_element());
    assert(!monstersNode.is_null());
    CL_DomElement monsterNode = monstersNode.get_first_child().to_element();
    assert(monsterNode.is_element());

    while(!monsterNode.is_null())
    {
        Monster * pMonster = dynamic_cast<Monster*>
            (pFactory->createElement("monster"));

        pMonster->load(&monsterNode);
        mMonsters [ pMonster->getName() ] = pMonster;
        monsterNode = monsterNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Monster: " << '\'' << pMonster->getName() << '\'' << std::endl;
#endif
    }


}

void CharacterManager::loadCharacters(CL_DomDocument &doc)
{
    IFactory * pFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement charactersNode = doc.named_item("characters").to_element();
    assert(charactersNode.is_element());
    assert(!charactersNode.is_null());
    CL_DomElement characterNode = charactersNode.get_first_child().to_element();
    assert(characterNode.is_element());
    std::cout << "Node name: " << characterNode.get_node_name();

    while(!characterNode.is_null())
    {
        Character * pCharacter = dynamic_cast<Character*>
            (pFactory->createElement(characterNode.get_node_name()));

        assert(pCharacter);
        assert(pCharacter->whichElement() == Element::ECHARACTER);
        pCharacter->load(&characterNode);
        mCharacters [ pCharacter->getName() ] = pCharacter;
        characterNode = characterNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Character: " << '\'' << pCharacter->getName() << '\'' << std::endl;
#endif
    }

}

StoneRing::Character * CharacterManager::getCharacter(const std::string &name)const
{
    return mCharacters.find(name)->second;
}

StoneRing::CharacterClass * CharacterManager::getClass ( const std::string &cls ) const
{
    return mCharacterClasses.find(cls)->second;
}




#include "CharacterManager.h"
#include "CharacterClass.h"
#include "Character.h"
#include "AbilityManager.h"
#include "IApplication.h"

using StoneRing::CharacterManager;

CharacterManager::~CharacterManager()
{
    std::for_each(mCharacterClasses.begin(),mCharacterClasses.end(),
                  compose_f_gx(del_fun<CharacterClass>(),
                               get_second<ClassMap::value_type>())
        );
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

StoneRing::CharacterClass * CharacterManager::getClass ( const std::string &cls ) const
{
    return mCharacterClasses.find(cls)->second;
}




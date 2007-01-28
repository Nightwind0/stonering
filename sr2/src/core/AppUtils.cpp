#include "AppUtils.h"
#include "ItemManager.h"
#include "AbilityManager.h"
#include <cassert>

using StoneRing::AppUtils;

AppUtils::AppUtils()
{
}

AppUtils::~AppUtils()
{
}

void AppUtils::loadGameItemsAndSkills(const std::string &path, CL_ResourceManager *pResources)
{
    std::string startinglevel = CL_String::load("Game/StartLevel",pResources);
    std::string itemdefinition = CL_String::load("Game/ItemDefinitions", pResources );
    std::string statusEffectDefinition = CL_String::load("Game/StatusEffectDefinitions",pResources);
    std::string spelldefinition = CL_String::load("Game/SpellDefinitions", pResources);
    std::string skilldefinition = CL_String::load("Game/SkillDefinitions",pResources);
    std::string classdefinition = CL_String::load("Game/CharacterClassDefinitions",pResources);

    loadStatusEffects(path + statusEffectDefinition);
    loadSpells(path + spelldefinition);
    loadItems(path + itemdefinition);
    loadSkills(path + skilldefinition);
   // loadCharacterClasses(path + classdefinition);
}


void AppUtils::loadSpells(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading spells..." << std::endl;
#endif    

    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    getAbilityManager()->loadSpellFile ( document );

}

void AppUtils::loadSkills(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading skills..." << std::endl;
#endif    

    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    getAbilityManager()->loadSkillFile ( document );

}

void AppUtils::loadStatusEffects(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading status effects...." << std::endl;
#endif

    CL_InputSource_File file(filename);

    CL_DomDocument document;

    document.load(&file);

    getAbilityManager()->loadStatusEffectFile( document );
}

void AppUtils::loadCharacterClasses(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading character classes..." << std::endl;
#endif

    CL_InputSource_File file(filename);

    CL_DomDocument document;

    document.load(&file);

    getAbilityManager()->loadCharacterClassFile( document );
}

void AppUtils::loadItems(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading items..." << std::endl;
#endif    
  
    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    getItemManager()->loadItemFile ( document );

}

StoneRing::AbilityManager * AppUtils::getAbilityManager()
{
    IApplication *pApp = IApplication::getInstance();
    assert ( NULL != pApp );
    AbilityManager * pMgr = pApp->getAbilityManager();
    assert ( NULL != pMgr );

    return pMgr;
}

StoneRing::ItemManager * AppUtils::getItemManager()
{
    IApplication *pApp = IApplication::getInstance();
    assert ( NULL != pApp );

    return pApp->getItemManager();
}
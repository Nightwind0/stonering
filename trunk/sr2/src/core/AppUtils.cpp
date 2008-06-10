#include "AppUtils.h"
#include "ItemManager.h"
#include "AbilityManager.h"
#include "CharacterManager.h"
#include <cassert>

using StoneRing::AppUtils;

AppUtils::AppUtils()
{
}

AppUtils::~AppUtils()
{
}

void AppUtils::LoadGameplayAssets(const std::string &path, CL_ResourceManager *pResources)
{
    std::string startinglevel = CL_String::load("Game/StartLevel",pResources);
    std::string itemdefinition = CL_String::load("Game/ItemDefinitions", pResources );
    std::string statusEffectDefinition = CL_String::load("Game/StatusEffectDefinitions",pResources);
    std::string spelldefinition = CL_String::load("Game/SpellDefinitions", pResources);
    std::string skilldefinition = CL_String::load("Game/SkillDefinitions",pResources);
    std::string classdefinition = CL_String::load("Game/CharacterClassDefinitions",pResources);
    std::string monsterdefinition = CL_String::load("Game/MonsterDefinitions",pResources);
    std::string characterdefinition = CL_String::load("Game/CharacterDefinitions",pResources);

    LoadStatusEffects(path + statusEffectDefinition);
    LoadSpells(path + spelldefinition);
    LoadItems(path + itemdefinition);
    LoadSkills(path + skilldefinition);
    LoadCharacterClasses(path + classdefinition);
    LoadMonsters(path + monsterdefinition);
    LoadCharacters(path + characterdefinition);
}


void AppUtils::LoadSpells(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading spells..." << std::endl;
#endif    

    CL_InputSource_File file(filename);
    CL_DomDocument document;
        
    document.load(&file);

    GetAbilityManager()->LoadSpellFile ( document );
}

void AppUtils::LoadSkills(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading skills..." << std::endl;
#endif    

    CL_InputSource_File file(filename);
    CL_DomDocument document;
    document.load(&file);

    GetAbilityManager()->LoadSkillFile ( document );
}

void AppUtils::LoadStatusEffects(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading status effects...." << std::endl;
#endif

    CL_InputSource_File file(filename);
    CL_DomDocument document;

    document.load(&file);

    GetAbilityManager()->LoadStatusEffectFile( document );
}

void AppUtils::LoadCharacterClasses(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading character classes..." << std::endl;
#endif

    CL_InputSource_File file(filename);
    CL_DomDocument document;
    document.load(&file);

    GetCharacterManager()->LoadCharacterClassFile( document );
}

void AppUtils::LoadCharacters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading characters from " << filename << std::endl;
#endif

    CL_InputSource_File file(filename);
    CL_DomDocument document;
    document.load(&file);

    GetCharacterManager()->LoadCharacters(document);
}

void AppUtils::LoadMonsters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading monsters..." << std::endl;
#endif

    CL_InputSource_File file(filename);
    CL_DomDocument document;
    document.load(&file);

    GetCharacterManager()->LoadMonsterFile(document);
}

void AppUtils::LoadItems(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading items..." << std::endl;
#endif    
  
    CL_InputSource_File file(filename);
    CL_DomDocument document;
   
    document.load(&file);
    GetItemManager()->LoadItemFile ( document );
}

StoneRing::AbilityManager * AppUtils::GetAbilityManager()
{
    IApplication *pApp = IApplication::GetInstance();
    assert ( NULL != pApp );
    AbilityManager * pMgr = pApp->GetAbilityManager();
    assert ( NULL != pMgr );

    return pMgr;
}

StoneRing::ItemManager * AppUtils::GetItemManager()
{
    IApplication *pApp = IApplication::GetInstance();
    assert ( NULL != pApp );

    return pApp->GetItemManager();
}

StoneRing::CharacterManager * AppUtils::GetCharacterManager()
{
    IApplication *pApp = IApplication::GetInstance();
    assert ( NULL != pApp );

    return pApp->GetCharacterManager();
}

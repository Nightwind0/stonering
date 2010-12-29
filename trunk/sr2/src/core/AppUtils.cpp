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

void AppUtils::LoadGameplayAssets(const std::string &path, CL_ResourceManager& resources)
{
    std::string startinglevel = CL_String_load("Game/StartLevel",resources);
    std::string itemdefinition = CL_String_load("Game/ItemDefinitions", resources );
    std::string statusEffectDefinition = CL_String_load("Game/StatusEffectDefinitions",resources);
    std::string spelldefinition = CL_String_load("Game/SpellDefinitions", resources);
    std::string skilldefinition = CL_String_load("Game/SkillDefinitions",resources);
    std::string classdefinition = CL_String_load("Game/CharacterClassDefinitions",resources);
    std::string monsterdefinition = CL_String_load("Game/MonsterDefinitions",resources);
    std::string characterdefinition = CL_String_load("Game/CharacterDefinitions",resources);
    std::string animationdefintion = CL_String_load("Game/AnimationDefinitions",resources);

    LoadStatusEffects(path + statusEffectDefinition);
    LoadSpells(path + spelldefinition);
    LoadItems(path + itemdefinition);
    LoadSkills(path + skilldefinition);
    LoadCharacterClasses(path + classdefinition);
    LoadMonsters(path + monsterdefinition);
    LoadCharacters(path + characterdefinition);
    LoadAnimations(path + animationdefintion);
}


void AppUtils::LoadSpells(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading spells..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;

    document.load(file);

    AbilityManager::LoadSpellFile ( document );
}

void AppUtils::LoadSkills(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading skills..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);

    AbilityManager::LoadSkillFile ( document );
}

void AppUtils::LoadStatusEffects(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading status effects...." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;

    document.load(file);

    AbilityManager::LoadStatusEffectFile( document );
}

void AppUtils::LoadCharacterClasses(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading character classes..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);

    GetCharacterManager()->LoadCharacterClassFile( document );
}

void AppUtils::LoadAnimations(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading animationss..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);

    AbilityManager::LoadAnimationFile( document );
}

void AppUtils::LoadCharacters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading characters from " << filename << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);

    GetCharacterManager()->LoadCharacters(document);
}

void AppUtils::LoadMonsters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading monsters..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);

    GetCharacterManager()->LoadMonsterFile(document);
}

void AppUtils::LoadItems(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading items..." << std::endl;
#endif

    CL_File file(filename);
    CL_DomDocument document;

    document.load(file);
    GetItemManager()->LoadItemFile ( document );
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
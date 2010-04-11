#ifndef SR_APPUTILS_H
#define SR_APPUTILS_H
#include <ClanLib/core.h>
#include <string>

namespace StoneRing{

class ItemManager;
class AbilityManager;
class CharacterManager;

class AppUtils
{
public:
    AppUtils();
    ~AppUtils();

    void LoadGameplayAssets(const std::string &path,CL_ResourceManager& resources);
private:

    void LoadItems(const std::string &filename);
    void LoadSpells(const std::string &filename);
    void LoadSkills(const std::string &filename);
    void LoadStatusEffects(const std::string &filename);
    void LoadCharacterClasses(const std::string &filename);
    void LoadCharacters(const std::string &filename);
    void LoadMonsters(const std::string &filename);
    void LoadAnimations(const std::string &filename);

    ItemManager * GetItemManager();
    CharacterManager * GetCharacterManager();
};
}

#endif


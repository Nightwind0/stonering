#ifndef SR_APPUTILS_H
#define SR_APPUTILS_H
#include <ClanLib/core.h>
#include <string>

namespace StoneRing{
class ItemManager;
class AbilityManager;

class AppUtils
{
public:
    AppUtils();
    ~AppUtils();

    void loadGameItemsAndSkills(const std::string &path,CL_ResourceManager *pResources);
private:

    void loadItems(const std::string &filename);
    void loadSpells(const std::string &filename);
    void loadSkills(const std::string &filename);
    void loadStatusEffects(const std::string &filename);
    void loadCharacterClasses(const std::string &filename);
    AbilityManager * getAbilityManager();
    ItemManager * getItemManager();
};
}

#endif
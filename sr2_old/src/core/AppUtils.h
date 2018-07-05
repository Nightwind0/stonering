#ifndef SR_APPUTILS_H
#define SR_APPUTILS_H
#include <ClanLib/core.h>
#include <string>
#include <list>

namespace StoneRing{

class ItemManager;
class AbilityManager;
class CharacterManager;

class AppUtils
{
public:
	struct SaveSummary {
        struct CharInfo{
            std::string m_name;
            uint m_level;
        };
        uint                    m_minutes;
        uint                    m_gold;        
        std::list<CharInfo>     m_characters;
        clan::DateTime          m_datetime;
		std::string             m_colors;
		bool 				    m_isValid;
    }; 

	static uint32_t 		SaveExists(uint32_t index);
    static void   			SaveGame(uint slot);
    static bool   			LoadGame(uint slot);	
	static SaveSummary 		LoadSaveSummary(uint32_t index);
    static void 			LoadGameplayAssets(const std::string &path,clan::ResourceManager& resources);	
private:
	static const uint32_t kSaveFileVersion;
	
	static SaveSummary load_file_header ( std::istream& in );
	static std::string         filename_for_slot(uint slot);
    static bool                verify_file(std::istream& in);
 	
    static void LoadItems(const std::string &filename);
    static void LoadSkills(const std::string &filename);
    static void LoadStatusEffects(const std::string &filename);
    static void LoadCharacterClasses(const std::string &filename);
    static void LoadCharacters(const std::string &filename);
    static void LoadMonsters(const std::string &filename);
    static void LoadAnimations(const std::string &filename);
    static void LoadMainMenu(const std::string &filename);
    
    static ItemManager * GetItemManager();
};
}

#endif


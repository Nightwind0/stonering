#include "AppUtils.h"
#include "ItemManager.h"
#include "AbilityManager.h"
#include "CharacterManager.h"
#include "IApplication.h"
#include "Party.h"
#include <cassert>

using StoneRing::AppUtils;

const uint32_t AppUtils::kSaveFileVersion = 5;



bool AppUtils::verify_file ( std::istream& in )
{
    char magic[5];
    magic[4] = '\0';
    in.read(magic,4);
    std::string magic_str(magic);
    if(magic_str != "SR2S"){
        return false;
    }
    
    int version;
    in.read((char*)&version,sizeof(version));
    if(version != kSaveFileVersion)
        return false;
    return true;
}


AppUtils::SaveSummary AppUtils::load_file_header ( std::istream& in )
{
    SaveSummary preview;
    uint gold;
    uint minutes;
    long long ticks;
    in.read((char*)&gold,sizeof(preview.m_gold));
    in.read((char*)&minutes,sizeof(preview.m_minutes));
    in.read((char*)&ticks,sizeof(ticks));
    preview.m_gold = gold;
    preview.m_minutes = minutes;
    preview.m_datetime = clan::DateTime::get_utc_time_from_ticks(ticks).to_local();
    uint party_size;
    in.read((char*)&party_size,sizeof(uint));
    for(uint i=0;i<party_size;i++){
        SaveSummary::CharInfo info;
        info.m_name = ReadString(in);
        in.read((char*)&info.m_level,sizeof(info.m_level));
        preview.m_characters.push_back(info);
    }
    preview.m_isValid = true;
    
    return preview;
}

std::string AppUtils::filename_for_slot ( uint slot )
{
    std::ostringstream os;
    os << "SaveSlot" << slot << ".sr2s";
    return os.str();
}



uint32_t 		AppUtils::SaveExists(uint32_t index){
	std::string filename = filename_for_slot(index);
	std::ifstream in_file;
	in_file.open(filename.c_str(), std::ios::in | std::ios::binary);
	bool exists = in_file.is_open();
	if(exists){
		exists = verify_file(in_file);
	}
	in_file.close();
	return exists;
}
void   			AppUtils::SaveGame(uint slot){
    char sig[] = "SR2S";
	std::string filename = filename_for_slot(slot);
    std::ofstream out_file(filename.c_str(),std::ios::out|std::ios::binary);
    out_file.write(sig,4);
    out_file.write((char*)&kSaveFileVersion,sizeof(kSaveFileVersion));
    Party * party = IApplication::GetInstance()->GetParty();
    int gold = party->GetGold();
    uint minutes = party->GetMinutesPlayed();
    long long ticks = clan::DateTime::get_current_utc_time().to_ticks();
    uint num_chars = party->GetCharacterCount();
    out_file.write((char*)&gold,sizeof(gold));
    out_file.write((char*)&minutes,sizeof(minutes));
    out_file.write((char*)&ticks,sizeof(ticks));
    out_file.write((char*)&num_chars,sizeof(num_chars));
    for(uint c = 0; c < num_chars; c++){
        uint level = party->GetCharacter(c)->GetLevel();
        WriteString(out_file,party->GetCharacter(c)->GetName());
        out_file.write((char*)&level,sizeof(uint));
    }
    IApplication::GetInstance()->Serialize(out_file);
    out_file.close();	
}
bool   			AppUtils::LoadGame(uint slot){
	std::string filename = filename_for_slot(slot);
    std::ifstream in_file(filename.c_str(),std::ios::in|std::ios::binary);
    if(!verify_file(in_file))
        return false;
    load_file_header(in_file); // load and drop on the floor. don't care now
    bool success = IApplication::GetInstance()->Deserialize(in_file);
    in_file.close();
    return success;	
}



AppUtils::SaveSummary 		AppUtils::LoadSaveSummary(uint32_t index){
	std::string filename = filename_for_slot(index);
	std::ifstream in_file(filename.c_str(),std::ios::in|std::ios::binary);
	bool verified = verify_file(in_file);
	if(!verified){
		SaveSummary summary;
		summary.m_isValid = false;
		return summary;
	}
	SaveSummary summary = load_file_header(in_file);
	in_file.close();
	return summary;
}



void AppUtils::LoadGameplayAssets(const std::string &path, clan::ResourceManager& resources)
{
    std::string itemdefinition = String_load("Game/ItemDefinitions", resources );
    std::string statusEffectDefinition = String_load("Game/StatusEffectDefinitions",resources);
    std::string skilldefinition = String_load("Game/SkillDefinitions",resources);
    std::string classdefinition = String_load("Game/CharacterClassDefinitions",resources);
    std::string monsterdefinition = String_load("Game/MonsterDefinitions",resources);
    std::string characterdefinition = String_load("Game/CharacterDefinitions",resources);
    std::string mainmenudefinition = String_load("Game/MainMenuDefinitions",resources);


    LoadStatusEffects(path + statusEffectDefinition);
    LoadItems(path + itemdefinition);
    LoadSkills(path + skilldefinition);
    LoadCharacterClasses(path + classdefinition);
    LoadMonsters(path + monsterdefinition);
    LoadCharacters(path + characterdefinition);

    LoadMainMenu(path + mainmenudefinition);
}


void AppUtils::LoadSkills(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading skills..." << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;
    document.load(file);

    AbilityManager::LoadSkillFile ( document );
}

void AppUtils::LoadStatusEffects(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading status effects from " << filename << "...." << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;

    document.load(file);

    AbilityManager::LoadStatusEffectFile( document );
}

void AppUtils::LoadCharacterClasses(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading character classes..." << std::endl;
#endif
    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);	
    clan::DomDocument document;
    document.load(file);

    CharacterManager::LoadCharacterClassFile( document );
}


void AppUtils::LoadCharacters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading characters from " << filename << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;
    document.load(file);

    CharacterManager::LoadCharacters(document);
}

void AppUtils::LoadMonsters(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading monsters..." << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;
    document.load(file);

    CharacterManager::LoadMonsterFile(document);
}

void AppUtils::LoadItems(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading items..." << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;

    document.load(file);
    ItemManager::LoadItemFile ( document );
}

void AppUtils::LoadMainMenu(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading main menu..." << std::endl;
#endif

    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;

    document.load(file);
    IApplication::GetInstance()->LoadMainMenu(document);
}



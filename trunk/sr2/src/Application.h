#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include "ItemManager.h"
#include "CharacterManager.h"
#include "AppUtils.h"
#include <queue>
#include "MapState.h"
#include "SayState.h"
#include "ExperienceState.h"
#include "BattleState.h"
#include "MainMenuState.h"
#include "ChoiceState.h"
#include "State.h"
#ifdef _WINDOWS_
#include <SteelInterpreter.h>
#else
#include <steel/SteelInterpreter.h>
#endif
#include "ElementFactory.h"
#include "BattleConfig.h"
#include "UtilityScripts.h"
#include "ItemSelectState.h"
#include "DynamicMenuState.h"
#include "SkillTreeState.h"

namespace StoneRing
{
    class Level;
    class Choice;


    class Application : public IApplication
    {
    public:
        Application() ;
        virtual ~Application();

        virtual int main(const std::vector<CL_String> &args);

        virtual CL_ResourceManager& GetResources();
        virtual CL_DisplayWindow& GetApplicationWindow();
        virtual void PopLevelStack(bool);
        virtual IParty * GetParty() const;
        virtual AbilityManager * GetAbilityManager();
        virtual ItemManager * GetItemManager();
        virtual IFactory * GetElementFactory() { return &mElementFactory; }
        virtual CharacterManager * GetCharacterManager() { return &mCharacterManager; }
        virtual CL_Rect GetDisplayRect() const;
        virtual void StartBattle(const MonsterGroup &group,const std::string &backdrop);
        virtual void RequestRedraw(const State *pState);
	virtual void RunState(State *pState);
	virtual void LoadMainMenu(CL_DomDocument& doc);
        virtual AstScript * LoadScript(const std::string &name, const std::string &script);
        virtual SteelType RunScript(AstScript * pScript);
        virtual SteelType RunScript(AstScript *pScript, const ParameterList &params);
	virtual AstScript* GetUtility(Utility util)const;
	
	virtual void MainMenu();
    int frameRate();
    protected:
        virtual int GetScreenWidth()const;
        virtual int GetScreenHeight()const;
    private:
	
	void queryJoystick();
	double get_value_for_axis_direction(IApplication::AxisDirection dir) const;
	AxisDirection get_direction_for_value(IApplication::Axis axis, double value) const;
	
        // Steel functions.
        SteelType gaussian(double mean, double sigma);
	SteelType log(const std::string &string);

        SteelType playScene(const std::string &animation);
        SteelType playSound(const std::string &sound);
        SteelType loadLevel(const std::string &level, uint startX, uint startY);
	SteelType mainMenu();
        SteelType startBattle(const std::string &monster, uint count, bool isBoss, const std::string &backdrop);
        SteelType say(const std::string &speaker, const std::string &text);
        SteelType message( const std::string &text );
        SteelType pause(uint time);
        SteelType invokeShop(const std::string &shoptype);
        SteelType choice(const std::string &choiceText, const SteelType::Container &choices);
        SteelType pop_(bool bAll);
        SteelType giveNamedItem(const std::string &item, uint count);
        SteelType giveItem(SteelType::Handle hItem, int count, bool silent);
        SteelType getGold();
        SteelType hasItem(const std::string &name, uint count);
        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
        SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);
        SteelType didEvent(const std::string &event);
        SteelType doEvent(const std::string &event, bool bRemember);
        SteelType takeNamedItem(const std::string &item, uint count);
        SteelType selectItem(bool battle, bool dispose);
        SteelType addCharacter(const std::string &character, int level, bool announce);
	SteelType inBattle();

	SteelType getPartyArray(void);
        SteelType getItemName(const SteelType::Handle hItem);
        SteelType getWeaponAttribute(const SteelType::Handle hWeapon, uint attr);
        SteelType getArmorAttribute(const SteelType::Handle hArmor, uint attr);
        // You can also take
        SteelType giveGold(int amount);
        // SteelType giveGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
        // SteelType giveGeneratedArmor(const std::string &armclass, const std::string &armtype);
        SteelType getCharacterName(const SteelType::Handle hICharacter);
        SteelType getCharacterLevel(const SteelType::Handle hICharacter);
        SteelType getCharacterSP(const SteelType::Handle hCharacter);
        SteelType setCharacterSP(const SteelType::Handle hCharacter, int sp);
        SteelType getMonsterSPReward(const SteelType::Handle hMonster);
	SteelType addExperience(const SteelType::Handle hICharacter, int xp);
	SteelType getExperience(const SteelType::Handle hICharacter);
        //
        SteelType getCharacterAttribute(const SteelType::Handle hICharacter, uint attr);
        SteelType augmentCharacterAttribute(const SteelType::Handle hICharacter, uint attr, double augment);

        SteelType getCharacterToggle(const SteelType::Handle hICharacter, uint attr);
        SteelType setCharacterToggle(const SteelType::Handle hICharacter, uint attr, bool toggle);
        SteelType getEquippedWeaponAttribute(const SteelType::Handle hICharacter, uint attr);
        SteelType getEquippedArmorAttribute(const SteelType::Handle hICharacter, uint attr);
        SteelType getStatusEffect(const std::string &effect);
        SteelType addStatusEffect(SteelType::Handle hCharacter, SteelType::Handle hStatusEffect);
        SteelType removeStatusEffect(SteelType::Handle hCharacter, SteelType::Handle hStatusEffect);
        SteelType statusEffectChance(SteelType::Handle HCharacter, SteelType::Handle hStatusEffect);
        // This method directly affects a character's HP. No factors are taken into account
        // The handle is to an ICharacter
        SteelType doDamage(SteelType::Handle hICharacter, int damage);
	SteelType doMPDamage(const SteelType::Handle hICharacter, int amount);
        SteelType kill(SteelType::Handle hICharacter);
        SteelType raise(SteelType::Handle hICharacter);

        SteelType hasEquipment(SteelType::Handle hICharacter, int slot);
        SteelType getEquipment(SteelType::Handle hCharacter, int slot);
        SteelType equip(SteelType::Handle hCharacter, int slot, const std::string &);

        SteelType forgoAttack(SteelType::Handle hWeapon);
        SteelType getWeaponType(SteelType::Handle hWeapon);
        SteelType getArmorType(SteelType::Handle hArmor);
        SteelType getWeaponTypeDamageCategory(SteelType::Handle hWeaponType);
	SteelType getWeaponTypeAnimation(SteelType::Handle hWeaponType);
	SteelType weaponTypeHasAnimation(SteelType::Handle hWeaponType);
        SteelType getDamageCategoryResistance(SteelType::Handle hICharacter, int damage_category);
        SteelType isArmor(SteelType::Handle hEquipment);

        SteelType invokeArmor(SteelType::Handle hCharacter, SteelType::Handle hArmor);
        SteelType invokeWeapon(SteelType::Handle hCharacter, SteelType::Handle hWeapon, uint invokeTime);
        SteelType attackCharacter(SteelType::Handle hICharacter, SteelType::Handle hIAttacker, int category, int amount);
	
	// Item Steel API
	SteelType getItemType(SteelType::Handle hItem);
	SteelType getItemValue(SteelType::Handle hItem);
	SteelType getItemSellValue(SteelType::Handle hItem);
	SteelType isBattleItem(SteelType::Handle hItem);
	SteelType isWorldItem(SteelType::Handle hItem);
	SteelType getItemTargetable(SteelType::Handle hItem);
	SteelType getItemDefaultTarget(SteelType::Handle hItem);
	SteelType isReusableItem(SteelType::Handle hItem);
        SteelType disposeItem(SteelType::Handle hItem,  uint count); // Call multiple times to take multiple of this item
	SteelType useItem(SteelType::Handle hItem, const SteelType& targets);

        SteelType getHitSound(SteelType::Handle hWeaponType);
        SteelType getMissSound(SteelType::Handle hWeaponType);
        SteelType getUnarmedHitSound(SteelType::Handle hICharacter);
        SteelType getUnarmedMissSound(SteelType::Handle hICharacter);
	
	SteelType getAnimation(const std::string& name);
	SteelType showExperience(const SteelArray&  characters, const SteelArray& xp_gained,
				 const SteelArray& oldLevels, const SteelArray& sp_gained);
        SteelType menu(const SteelArray& options);
        SteelType skilltree(SteelType::Handle hCharacter, bool buy);
        SteelType getSkill(const std::string& name);
        SteelType learnSkill(SteelType::Handle hCharacter, SteelType::Handle skill);
        SteelType hasSkill(SteelType::Handle hCharacter, const std::string&);
        SteelType doSkill(SteelType::Handle hSkill, SteelType::Handle hCharacter);
        
        SteelType generateRandomWeapon(double min_value, double max_value);
        SteelType tryEquipmentStatusEffectInflictions(SteelType::Handle hEquipment, SteelType::Handle hTarget);

        void steelConst(const std::string &name, int value);
        void steelConst(const std::string &name, double value);

        void setupClanLib();
        void teardownClanLib();
        void showRechargeableOnionSplash();
        void showIntro();
        void showError(int line, const std::string &script, const std::string &message);
        void registerSteelFunctions();

        void draw();
        void run();
        void loadscript(std::string &o_str, const std::string & filename);

        void startKeyUpQueue();
        void stopKeyUpQueue();

        /* SIGNALS */
        void onSignalQuit();
        void onSignalKeyDown(const CL_InputEvent &key,const CL_InputState& state);
        void onSignalKeyUp(const CL_InputEvent &key, const CL_InputState& state);
	void onSignalJoystickButtonDown(const CL_InputEvent &event, const CL_InputState& state);
	void onSignalJoystickButtonUp(const CL_InputEvent &event, const CL_InputState& state);
	void onSignalJoystickAxisMove(const CL_InputEvent &event, const CL_InputState& state);

        int calc_fps(int);

        Party *mpParty;
        ItemManager mItemManager;
        AbilityManager mAbilityManager;
        CharacterManager mCharacterManager;
        SteelInterpreter mInterpreter;
        ElementFactory mElementFactory;
        bool mbDone;
        CL_ResourceManager m_resources;
        CL_DisplayWindow m_window;

        std::string mGold;

        /* STATES */
        MapState mMapState;
        SayState mSayState;
        BattleState mBattleState;
	ExperienceState mExperienceState;
	MainMenuState  mMainMenuState;
	ItemSelectState mItemSelectState;
        SkillTreeState mSkillTreeState;
        std::vector<State*> mStates;
        std::vector<IFactory*> mFactories;
	BattleConfig mBattleConfig;
        AppUtils mAppUtils;
	UtilityScripts mUtilityScripts;
    };

inline void Application::steelConst(const std::string &name, int value)
{
    SteelType val;
    val.set(value);
    mInterpreter.declare_const(name,val);
}

inline void Application::steelConst(const std::string &name, double value)
{
    SteelType val;
    val.set(value);
    mInterpreter.declare_const(name,val);
}


}


#endif







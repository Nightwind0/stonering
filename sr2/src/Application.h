#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <functional>
#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "sr_defines.h"
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
#ifdef SR2_EDITOR
#include "MapEditorState.h"
#endif
#include "State.h"
#include "ElementFactory.h"
#include "BattleConfig.h"
#include "UtilityScripts.h"
#include "ItemSelectState.h"
#include "DynamicMenuState.h"
#include "SkillTreeState.h"
#include "EquipState.h"
#include "ShopState.h"
#include "CutSceneState.h"
#include "StartupState.h"
#include "ZipFileProvider.h"
#include "ItemGetState.h"
#include "JoystickConfig.h"
#include "BannerState.h"
#include "ReserveParty.h"
#include "SkillGetState.h"

using Steel::SteelType;
class DrawThread;

#define SHOW_FPS 1

namespace StoneRing
{
class Level;
class Choice;

// camelCase

class Application : public IApplication
{
public:
    Application() ;
    virtual ~Application();

    virtual int main(const std::vector<std::string> args);

    virtual clan::ResourceManager&  GetResources();
    virtual clan::DisplayWindow&       GetApplicationWindow();
    virtual void                    PopLevelStack(bool);
    virtual Party*                  GetParty() const;
    virtual AbilityManager *        GetAbilityManager();
    virtual IFactory *              GetElementFactory() {
        return &mElementFactory;
    }
    virtual clan::Rect                 GetDisplayRect() const;
    virtual void                    StartBattle(const MonsterGroup &group,const std::string &backdrop);
    virtual void                    RunState(State *pState, bool threaded=false);
    virtual void                    LoadMainMenu(clan::DomDocument& doc);
    virtual Level *                 GetCurrentLevel() const;
	virtual clan::Point  				 GetCurrentLevelCenter() const;
    virtual AstScript *             LoadScript(const std::string &name, const std::string &script);
    virtual SteelType               RunScript(AstScript * pScript);
    virtual SteelType               RunScript(AstScript *pScript, const ParameterList &params);
    virtual AstScript*              GetUtility(Utility util)const;
    virtual uint                    GetMinutesPlayed()const;
    virtual std::string             GetCurrencyName() const;
    virtual void                    StartGame(bool load);
    virtual bool                    Serialize(std::ostream& stream);
    virtual bool                    Deserialize(std::istream& stream);
    virtual void                    MainMenu();
    virtual void                    RunOnMainThread(std::function<void()>& function);
    virtual void                    Banner(const std::string&, int time);
	virtual clan::IODevice     		 OpenResource(const std::string& str);	
	virtual bool 					 IsCutsceneRunning() const; // Too much of a hack?
	virtual int 			 		 DynamicMenu(const std::vector<std::string>& options);	
	virtual std::string     		GetResourcePath()const;	
    int                             frameRate();
#ifdef SR2_EDITOR
	virtual void  			 EditMaps();
#endif	
protected:
    virtual int                     GetScreenWidth()const;
    virtual int                     GetScreenHeight()const;
private:

    struct ThreadFunctor {
        ThreadFunctor(const clan::Event& event,std::function<void()> &func):m_event(event),m_functor(func) {
        }
        std::function<void()> m_functor;
        clan::Event m_event;
    };


    void                            queryJoystick();
    double                          get_value_for_axis_direction(IApplication::AxisDirection dir) const;
    AxisDirection                   get_direction_for_value(IApplication::Axis axis, double value) const;


    // Steel functions.
    SteelType                       gaussian(double mean, double sigma);
    SteelType                       log(const std::string &string);

    SteelType                       playScene(const SteelType& functor);
    SteelType                       playSound(const std::string &sound, bool wait, double echo);
    SteelType                       loadLevel(const std::string &level, uint startX, uint startY);
	SteelType  						 pushLevel(const std::string &level, uint startX, uint startY);
    SteelType                       mainMenu();
    SteelType                       startBattle(const std::string &monster, uint count, bool isBoss, const std::string &backdrop);
    SteelType                       say(const std::string &speaker, const std::string &text);
    SteelType                       message( const std::string &text );
    SteelType                       pause(uint time);
    SteelType                       invokeShop(const std::string &shoptype);
    SteelType                       choice(const std::string &choiceText, const SteelType::Container &choices);
    SteelType                       popLevel(bool bAll);
    SteelType                       getNamedItem(const std::string &item);
	SteelType 						 giveItem(const SteelType::Handle hItem, int count, bool silent);
    SteelType                       giveItems(const Steel::SteelArray& items, bool silent);
    SteelType                       getGold();
    SteelType                       hasItem(const std::string &name, uint count);
    SteelType                       hasGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
    SteelType                       hasGeneratedArmor(const std::string &armclass, const std::string &armtype);
    SteelType                       didEvent(const std::string &event);
    SteelType                       doEvent(const std::string &event, bool bRemember);
    SteelType                       takeItem(SteelType::Handle hItem, uint count, bool silent);
    SteelType                       selectItem(bool battle, bool dispose);
    SteelType                       selectItemAdv(uint filter);
    SteelType                       addCharacter(const std::string &character, int level, bool announce);
    SteelType                       inBattle();
    SteelType                       shop(const Steel::SteelArray& items); // only for buying
    SteelType                       sell();
    SteelType                       getPartyArray(void);
	SteelType 						 getReservePartyArray(void);
	SteelType 						 reserveCharacter(const std::string& name);
	SteelType 						 deployCharacter(const std::string& name);
	//SteelType 						 getParty(void);
	//SteelType 						 getReserveParty(void);
	
	
    SteelType                       getItemName(const SteelType::Handle hItem);
	SteelType 						 getWeaponClass(const SteelType::Handle hWeapon);
	SteelType 						 getWeaponImbuement(const SteelType::Handle hWeapon);
    SteelType                       getWeaponAttribute(const SteelType::Handle hWeapon, uint attr);
    SteelType                       getArmorAttribute(const SteelType::Handle hArmor, uint attr);
    // You can also take
    SteelType                       giveGold(int amount);
    // SteelType giveGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
    // SteelType giveGeneratedArmor(const std::string &armclass, const std::string &armtype);
    SteelType                       getCharacterName(const SteelType::Handle hICharacter);
    SteelType                       getCharacterLevel(const SteelType::Handle hICharacter);
    SteelType                       getCharacterSP(const SteelType::Handle hCharacter);
    SteelType                       setCharacterSP(const SteelType::Handle hCharacter, int sp);
    SteelType                       addExperience(const SteelType::Handle hICharacter, int xp);
    SteelType                       getExperience(const SteelType::Handle hICharacter);
    SteelType                       changeCharacterClass(SteelType::Handle hCharacter, const std::string& chr_class);
    //
    SteelType                       getCharacterAttribute(const SteelType::Handle hICharacter, uint attr);
    SteelType                       augmentCharacterAttribute(const SteelType::Handle hICharacter, uint attr, double augment);

    SteelType                       getMonsterDrops(const SteelType::Handle hMonster);

    SteelType                       getCharacterToggle(const SteelType::Handle hICharacter, uint attr);
    SteelType                       setCharacterToggle(const SteelType::Handle hICharacter, uint attr, bool toggle);
    SteelType                       getEquippedWeaponAttribute(const SteelType::Handle hICharacter, uint attr, int slot);
    SteelType                       getEquippedArmorAttribute(const SteelType::Handle hICharacter, uint attr);
    SteelType                       getStatusEffect(const std::string &effect);
    SteelType                       addStatusEffect(SteelType::Handle hCharacter, SteelType::Handle hStatusEffect);
    SteelType                       removeStatusEffect(SteelType::Handle hCharacter, SteelType::Handle hStatusEffect);
    SteelType                       statusEffectChance(SteelType::Handle HCharacter, SteelType::Handle hStatusEffect);
    // This method directly affects a character's HP. No factors are taken into account
    // The handle is to an ICharacter
    SteelType                       doDamage(SteelType::Handle hICharacter, int damage);
    SteelType                       doMPDamage(const SteelType::Handle hICharacter, int amount);
    SteelType                       kill(SteelType::Handle hICharacter);
    SteelType                       raise(SteelType::Handle hICharacter);

    SteelType                       hasEquipment(SteelType::Handle hICharacter, int slot);
    SteelType                       getEquipment(SteelType::Handle hCharacter, int slot);
    SteelType                       equip(SteelType::Handle hCharacter, int slot, const std::string &);

    SteelType                       weaponTypeIsRanged(SteelType::Handle hWeaponType);
    SteelType                       forgoAttack(SteelType::Handle hWeapon);
    SteelType                       getWeaponType(SteelType::Handle hWeapon);
    SteelType                       getArmorType(SteelType::Handle hArmor);
    SteelType                       getWeaponTypeDamageCategory(SteelType::Handle hWeaponType);
	SteelType 						getWeaponAttribute(SteelType::Handle, int attr);
	SteelType 						getArmorAttribute(SteelType::Handle, int attr);
	SteelType 						weaponHasAnimationScript(SteelType::Handle hWeapon);
	SteelType 						runWeaponAnimationScript(SteelType::Handle hWeapon, SteelType::Handle hCharacter, SteelType::Handle hTarget);
    SteelType                       getDamageCategoryResistance(SteelType::Handle hICharacter, int damage_category);
    SteelType                       isArmor(SteelType::Handle hEquipment);
    SteelType                       changeArmorClass(SteelType::Handle hArmor, const std::string& arm_class);
    SteelType                       changeArmorImbuement(SteelType::Handle hArmor, const std::string& imbuement);
    SteelType                       changeWeaponClass(SteelType::Handle hArmor, const std::string& weapon_class);
    SteelType                       changeWeaponImbuement(SteelType::Handle hWeapon, const std::string& imbuement);


    SteelType                       invokeArmor(SteelType::Handle hCharacter, SteelType::Handle hArmor);
    SteelType                       invokeWeapon(SteelType::Handle hCharacter, SteelType::Handle hTarget, SteelType::Handle hWeapon, uint hand, uint invokeTime);
    SteelType                       attackCharacter(SteelType::Handle hICharacter, SteelType::Handle hIAttacker, uint category, bool melee, int amount);

    // Item Steel API
    SteelType                       getItemType(SteelType::Handle hItem);
    SteelType                       getItemValue(SteelType::Handle hItem);
    SteelType                       getItemSellValue(SteelType::Handle hItem);
    SteelType                       isBattleItem(SteelType::Handle hItem);
    SteelType                       isWorldItem(SteelType::Handle hItem);
    SteelType                       getItemTargetable(SteelType::Handle hItem);
    SteelType                       getItemDefaultTarget(SteelType::Handle hItem);
    SteelType                       isReusableItem(SteelType::Handle hItem);
    SteelType                       disposeItem(SteelType::Handle hItem,  uint count); // Call multiple times to take multiple of this item
    SteelType                       useItem(SteelType::Handle hItem, const SteelType& targets);

    SteelType                       getUnarmedHitSound(SteelType::Handle hICharacter);
    SteelType                       getUnarmedMissSound(SteelType::Handle hICharacter);

    SteelType                       showExperience(const Steel::SteelArray&  characters, const Steel::SteelArray& xp_gained,
            const Steel::SteelArray& oldLevels, const Steel::SteelArray& sp_gained);
    SteelType                       menu(const Steel::SteelArray& options);
    SteelType                       skilltree(SteelType::Handle hCharacter, bool buy);
    SteelType                       getSkill(const std::string& name);
    SteelType                       learnSkill(SteelType::Handle hCharacter, SteelType::Handle skill, bool silent=true);
    SteelType                       hasSkill(SteelType::Handle hCharacter, const std::string&);
    SteelType                       doSkill(SteelType::Handle hSkill, SteelType::Handle hCharacter);

    SteelType                       equipOmega(uint slot, const SteelType::Handle &hOmega);
    SteelType                       unequipOmega(uint slot);
    SteelType                       omegaSlotCount();
    SteelType                       getOmega(uint slot);
    SteelType                       omegaSlotIsEmpty(uint slot);

    SteelType                       generateRandomWeapon(uint rarity, int min_value, int max_value);
    SteelType                       generateRandomArmor(uint rarity, int min_value, int max_value);
    SteelType                       randomItem(uint rarity, int min_value, int max_value);
    SteelType                       doEquipmentStatusEffectInflictions(SteelType::Handle hEquipment, SteelType::Handle hTarget);
    SteelType                       equipScreen(SteelType::Handle hCharacter);
    SteelType                       save();
    SteelType                       load();
    SteelType                       statusScreen(bool party);
    SteelType                       getThemes();
    SteelType                       setTheme(const std::string&);
    SteelType                       banner(const std::string&, double time);
	SteelType                       closeMap();
	SteelType                       gameoverScreen();
	
	SteelType 						 pushMusic(const std::string&);
	SteelType 						 popMusic();
	SteelType 						 stopMusic();
	SteelType 						 startMusic();
	SteelType  						 configJoystick();
	
#ifndef NDEBUG
	SteelType 						enableInfiniteSP(bool);
	SteelType 						enableInfiniteBP(bool);
	SteelType  						enableInfiniteGold(bool);
#endif
	SteelType 						enableAllSkills(bool);
			 

    SteelType                       editing(); // In editor build or not
	SteelType						debug(); // In debug build or not
    SteelType                       editMap();


    void steelConst(const std::string &name, int value);
    void steelConst(const std::string &name, double value);

    void setupClanLib();
    void teardownClanLib();
    void showRechargeableOnionSplash();
    void showIntro();
    void showError(int line, const std::string &script, const std::string &message);
    void registerSteelFunctions();
	
	State* getInputState() const;

    void draw();
    void run(bool process_functors=true);
    void loadscript(std::string &o_str, const std::string & filename, clan::FileSystem fs);

    void startKeyUpQueue();
    void stopKeyUpQueue();

    /* SIGNALS */
	void onSignalLostFocus();
    void onSignalQuit();
    void onSignalKeyDown(const clan::InputEvent &key);
    void onSignalKeyUp(const clan::InputEvent &key);
    void onSignalMouseDown(const clan::InputEvent &event);
    void onSignalMouseUp(const clan::InputEvent &event);
    void onSignalDoubleClick(const clan::InputEvent& event);
    void onSignalMouseMove(const clan::InputEvent& event);
    void onSignalJoystickButtonDown(const clan::InputEvent &event);
    void onSignalJoystickButtonUp(const clan::InputEvent &event);
    void onSignalJoystickAxisMove(const clan::InputEvent &event);

    int calc_fps(int);

    Party *mpParty;
    AbilityManager mAbilityManager;
    ElementFactory mElementFactory;
    bool mbDone;
    clan::ResourceManager m_resources;
	
    clan::DisplayWindow m_window;
    clan::Canvas m_canvas;

    std::queue<ThreadFunctor> m_mainthread_functors;

    std::string mGold;
    uint64_t m_startTime;
	
	/* Joystick config */
	enum JoystickTrainState {
		JS_TRAIN_IDLE,
		JS_TRAIN_AXIS,
		JS_TRAIN_BUTTON
	}m_joystick_train_state;
	
	union JoystickTrainComponent {
		IApplication::AxisDirection m_dir;
		IApplication::Button m_button;
	}m_joystick_train_component;

	JoystickConfig m_joystick_config;

    /* STATES */
    clan::Mutex mFunctorMutex;
    StartupState mStartupState;
    MapState mMapState;
    SayState mSayState;
    BattleState mBattleState;
    ShopState mShopState;
    ExperienceState mExperienceState;
    MainMenuState  mMainMenuState;
    ItemSelectState mItemSelectState;
    SkillTreeState mSkillTreeState;
	SkillGetState mSkillGetState;
    EquipState mEquipState;
	BannerState mBannerState;
	SteelInterpreter* mInterpreter;
#if SR2_EDITOR
    MapEditorState mMapEditorState;
#endif
	bool m_threaded_mode;
	uint m_max_party_size;
	std::string m_resource_path;
	ReserveParty m_reserve_party;
    std::deque<State*> mStates;
    std::vector<IFactory*> mFactories;
    BattleConfig mBattleConfig;
    UtilityScripts mUtilityScripts;
	clan::FileSystem m_resource_dir;
	ZipFileProvider m_zip_provider;
	std::map<IApplication::Button,bool> m_button_down;
	
#if SHOW_FPS
	int m_draws_total;
	long m_draw_start_time;
#endif

};

inline void Application::steelConst(const std::string &name, int value)
{
    SteelType val;
    val.set(value);
    mInterpreter->declare_const(name,val);
}

inline void Application::steelConst(const std::string &name, double value)
{
    SteelType val;
    val.set(value);
    mInterpreter->declare_const(name,val);
}


}


#endif







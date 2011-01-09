#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "Application.h"
#include "Level.h"
#include "Party.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "CharacterFactory.h"
#include "ChoiceState.h"
#include "GraphicsManager.h"
#include "MonsterRef.h"
#include "Monster.h"
#include "Weapon.h"
#include "WeaponType.h"
#include "Armor.h"
#ifndef _WINDOWS_
#include <steel/SteelType.h>
#else
#include <SteelType.h>
#endif

#include "BattleConfig.h"
#include "Animation.h"

//
//
// 





using StoneRing::Application;
using namespace StoneRing;


// Global instance
Application * pApp = NULL;

#ifndef _MSC_VER

using std::min;
using std::max;

#endif

const unsigned int WINDOW_WIDTH = 800 ;
const unsigned int WINDOW_HEIGHT = 600 ;
const unsigned int MS_BETWEEN_MOVES = 20;


bool gbDebugStop;



CL_DisplayWindow& Application::GetApplicationWindow()
{
    return m_window;
}


int Application::GetScreenWidth()const
{
    return WINDOW_WIDTH;
}

int Application::GetScreenHeight()const
{
    return WINDOW_HEIGHT;
}


SteelType Application::playScene(const std::string &animation)
{
#ifndef NDEBUG
    std::cout << "Playing scene " << animation << std::endl;
#endif

    return SteelType();
}

SteelType Application::playSound(const std::string &sound)
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif

    return SteelType();
}
SteelType Application::loadLevel(const std::string &level, uint startX, uint startY)
{
    Level * pLevel = new Level();
    pLevel->Load(level, m_resources);
    pLevel->Invoke();
    mMapState.PushLevel( pLevel, static_cast<uint>(startX), static_cast<uint>(startY) );

    return SteelType();
}

void Application::Pop(bool bAll)
{
    mMapState.Pop(bAll);
}

SteelType Application::pop_(bool bAll)
{
    Pop(bAll);

    return SteelType();
}

void Application::StartBattle(const MonsterGroup &group, const std::string &backdrop)
{
#ifndef NDEBUG
    std::cout << "Encounter! Backdrop = " << backdrop << std::endl;

    const std::vector<MonsterRef*> &monsters = group.GetMonsters();

    for (std::vector<MonsterRef*>::const_iterator it =  monsters.begin();
            it != monsters.end(); it++)
    {
        MonsterRef * pRef = *it;
        std::cout << '\t' << pRef->GetName() << " x" << pRef->GetCount() << std::endl;
    }
#endif
    mBattleState.init(group,backdrop);

    mStates.push_back(&mBattleState);

    run();
}

SteelType Application::startBattle(const std::string &monster, uint count, bool isBoss, const std::string& backdrop)
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif
    
    DynamicMonsterRef* monsterRef = new DynamicMonsterRef();
    
    float square_root = sqrt((float)count);
    int square_size = ceil(square_root);
    monsterRef->SetName(monster);
    monsterRef->SetCellX(0);
    monsterRef->SetCellY(0);
    monsterRef->SetCount(count);
    monsterRef->SetRows( square_size );
    monsterRef->SetColumns( square_size );
    
    std::vector<MonsterRef*> monsters;
    monsters.push_back(monsterRef);
    
    mBattleState.init(monsters,
		      monsterRef->GetRows(),
		      monsterRef->GetColumns(),
		      isBoss,
		      backdrop);

    mStates.push_back(&mBattleState);

    run();
    

    // TODO: Return false if you lose, true if you win.
    return SteelType();
}

SteelType Application::choice(const std::string &choiceText,
                              const std::vector<SteelType> &choices_)
{
    static ChoiceState choiceState;
    std::vector<std::string> choices;
    choices.reserve ( choices_.size() );

    for (unsigned int i=0;i<choices_.size();i++)
        choices.push_back ( choices_[i] );

    choiceState.Init(choiceText,choices);
    mStates.push_back ( &choiceState );
    run(); // Run pops for us.

    SteelType selection;
    selection.set( choiceState.GetSelection() );

    return selection;
}


SteelType Application::say(const std::string &speaker, const std::string &text)
{
    mSayState.Init(speaker,text);

    mStates.push_back(&mSayState);

    run();

    return SteelType();
}

void Application::RunState(State * pState)
{
    mStates.push_back(pState);

    run();
}

void Application::showError(int line, const std::string &script, const std::string &message)
{
    std::ostringstream os;
    os << "Script error in " << script << " on line " << line << " (" << message << ')';

    say("Error", os.str());
}


SteelType Application::gaussian(double mean, double sigma)
{
    SteelType result;
    result.set ( normal_random(mean,sigma) );

    return result;
}


SteelType Application::pause(uint time)
{
    CL_System::sleep(time);

    return SteelType();
}

SteelType Application::invokeShop(const std::string &shoptype)
{
    return SteelType();
}


SteelType Application::getGold()
{
    SteelType val;
    val.set ( mpParty->GetGold() );
    return val;
}

SteelType Application::hasItem(const std::string &item, uint count)
{
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    assert ( pMgr );
    SteelType var;
    var.set ( mpParty->HasItem(pMgr->GetNamedItem(item),count) );

    return var;
}

SteelType Application::didEvent(const std::string &event)
{
    SteelType var;
    var.set ( mpParty->DidEvent(event ) );

    return var;
}

SteelType Application::doEvent(const std::string &event, bool bRemember)
{
    mpParty->DoEvent ( event, bRemember );
    return SteelType();
}

SteelType Application::giveNamedItem(const std::string &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    assert ( pMgr );
    mpParty->GiveItem ( pMgr->GetNamedItem(item), count );

    os << "You received " << item;

    if (count > 1)
        os << " x" << count;

    say("Item Received",os.str());

    return SteelType();
}

SteelType Application::takeNamedItem(const std::string  &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    SteelType val;
    val.set ( mpParty->GiveItem ( pMgr->GetNamedItem(item), count ) );

    os << "Gave up " << item;

    if (count > 1)
        os << " x" << count;

    say("Item Lost",os.str());

    return val;
}

SteelType Application::giveGold(int amount)
{
    std::ostringstream os;
    mpParty->GiveGold(amount);

    if (amount > 0)
    {
        os << "You received " << amount << ' ' << mGold << '.';
        say(mGold, os.str());
    }
    else if ( amount > 0)
    {
        os << "Lost " << amount << ' ' << mGold << '.';
        say(mGold,os.str());
    }

    return SteelType();
}

SteelType Application::addCharacter(const std::string &character, int level, bool announce)
{
    Character * pCharacter = mCharacterManager.GetCharacter(character);
    pCharacter->SetLevel(level);
    // TODO: This is kind of a hack... probably ought to do this via steel API
    pCharacter->PermanentAugment(ICharacter::CA_HP, pCharacter->GetAttribute(ICharacter::CA_MAXHP));
    pCharacter->PermanentAugment(ICharacter::CA_MP, pCharacter->GetAttribute(ICharacter::CA_MAXMP));
    mpParty->AddCharacter(pCharacter);

    if (announce)
    {
        std::ostringstream os;
        os << character << " joined the party!";

        say("",os.str());
    }
    SteelType returnPointer;
    returnPointer.set(pCharacter);

    return returnPointer;
}

SteelType Application::doDamage(SteelType::Handle hICharacter, int damage)
{
    ICharacter * pCharacter = dynamic_cast<ICharacter*>(hICharacter);

    if (!pCharacter->GetToggle(Character::CA_ALIVE)) return SteelType();

    int hp = pCharacter->GetAttribute(Character::CA_HP);
    const int maxhp = pCharacter->GetAttribute(Character::CA_MAXHP);

    if (hp - damage<=0)
    {
        damage = hp;
        // Kill him/her/it
        pCharacter->Kill();
        //TODO: Check if all characters are dead and game over
    }

    if (hp - damage >maxhp)
    {
        damage = -(maxhp - hp);
    }

    pCharacter->PermanentAugment(Character::CA_HP,-damage);

    SteelType newhp;
    newhp.set(damage);

    return newhp;
}


SteelType Application::useItem()
{
    return SteelType();
}

SteelType Application::getCharacterName(const SteelType::Handle handle)
{
    ICharacter * pCharacter = dynamic_cast<ICharacter*>(handle);
    if(!pCharacter) throw TypeMismatch();
    SteelType name;
    name.set(pCharacter->GetName());

    return name;
}


SteelType Application::getPartyArray()
{
    SteelType array;
    std::vector<SteelType> vector;
    for(int i=0;i<mpParty->GetCharacterCount();i++)
    {
	SteelType ptr;
	ptr.set(mpParty->GetCharacter(i));
	vector.push_back(ptr);
    }
    
    array.set(vector);
  
    return array;
}

SteelType Application::getCharacterLevel(const SteelType::Handle hCharacter)
{
    ICharacter *pCharacter = dynamic_cast<ICharacter*>(hCharacter);
    if(pCharacter == NULL) throw TypeMismatch();
    SteelType level;
    level.set((int)pCharacter->GetLevel());

    return level;
}

SteelType Application::equip(SteelType::Handle hCharacter, int slot, const std::string &equipment)
{
    Character * pCharacter = dynamic_cast<Character*>(hCharacter);
    if(!pCharacter) throw TypeMismatch();
    Equipment * pEquipment = dynamic_cast<Equipment*>(mItemManager.GetNamedItem(equipment));
    SteelType result;
    if (pEquipment != NULL)
    {
        pCharacter->Equip(static_cast<Equipment::eSlot>(slot),pEquipment);
        result.set(true);
    }
    else
    {
        result.set(false);
    }

    return result;
}

SteelType Application::addStatusEffect(SteelType::Handle hCharacter, const std::string &effect)
{
    ICharacter *pCharacter = dynamic_cast<ICharacter*>(hCharacter);
    if(!pCharacter) throw TypeMismatch();
    pCharacter->AddStatusEffect( mAbilityManager.GetStatusEffect(effect) );

    return SteelType();
}

SteelType Application::removeStatusEffects(SteelType::Handle hCharacter, const std::string &effect)
{
    ICharacter *pCharacter = dynamic_cast<ICharacter*>(hCharacter);
    if(!pCharacter) throw TypeMismatch();
    pCharacter->RemoveEffects(effect);

    return SteelType();
}

SteelType Application::hasEquipment(SteelType::Handle hICharacter, int slot)
{
    SteelType result;

    ICharacter *iCharacter = GrabHandle<ICharacter*>(hICharacter);
    Monster * pMonster = dynamic_cast<Monster*>(iCharacter);

    if (pMonster) // Monsters currently can't have equipment
    {
        result.set(false);
        return result;
    }
    Character *pCharacter = dynamic_cast<Character*>(iCharacter);
    result.set(pCharacter->HasEquipment(static_cast<Equipment::eSlot>(slot)));
    return result;
}

SteelType Application::getEquipment(SteelType::Handle hCharacter, int slot)
{
    SteelType result;
    Character *pCharacter = GrabHandle<Character*>(hCharacter);
    result.set(pCharacter->GetEquipment(static_cast<Equipment::eSlot>(slot)));
    return result;
}

SteelType Application::getEquippedWeaponAttribute(const SteelType::Handle hICharacter, uint attr)
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    result.set(pCharacter->GetEquippedWeaponAttribute(static_cast<Weapon::eAttribute>(attr)));

    return result;
}

SteelType Application::getEquippedArmorAttribute(const SteelType::Handle hICharacter, uint attr)
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    result.set(pCharacter->GetEquippedArmorAttribute(static_cast<Armor::eAttribute>(attr)));

    return result;
}

SteelType Application::getItemName(const SteelType::Handle hItem)
{
    SteelType result;
    Item * pItem = GrabHandle<Item*>(hItem);
    result.set(pItem->GetName());
    return result;
}

SteelType Application::getWeaponAttribute(const SteelType::Handle hWeapon, uint attr)
{
    SteelType result;
    Weapon* pWeapon = GrabHandle<Weapon*>(hWeapon);
    float value = pWeapon->GetWeaponAttribute(static_cast<Weapon::eAttribute>(attr));
    result.set(value);

    return result;
}

SteelType Application::getWeaponScriptMode(SteelType::Handle hWeapon)
{
    SteelType result;
    Weapon* pWeapon = GrabHandle<Weapon*>(hWeapon);

    result.set( pWeapon->GetScriptMode() );

    return result;
}

SteelType Application::getArmorAttribute(const SteelType::Handle hArmor, uint attr)
{
    SteelType result;
    Armor * pArmor = GrabHandle<Armor*>(hArmor);
    float value = pArmor->GetArmorAttribute(static_cast<Armor::eAttribute>(attr));
    result.set(value);

    return result;
}

SteelType Application::getCharacterAttribute(const SteelType::Handle hICharacter, uint attr)
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute>(attr);

    result.set(pCharacter->GetAttribute(theAttr));

    return result;
}

SteelType Application::getCharacterToggle(const SteelType::Handle hICharacter, uint attr)
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute>(attr);

    result.set(pCharacter->GetToggle(theAttr));

    return result;
}

SteelType Application::setCharacterToggle(const SteelType::Handle hICharacter, uint attr, bool toggle)
{
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute>(attr);

    pCharacter->SetToggle(theAttr,toggle);

    return SteelType();
}


SteelType Application::getWeaponType(SteelType::Handle hWeapon)
{
    SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*>(hWeapon);
    val.set ( pWeapon->GetWeaponType() );

    return val;
}

SteelType Application::getArmorType(SteelType::Handle hArmor)
{
    SteelType val;
    Armor* pArmor = GrabHandle<Armor*>(hArmor);
    val.set ( pArmor->GetArmorType() );

    return val;
}

SteelType Application::getWeaponTypeDamageCategory(SteelType::Handle hWeaponType)
{
    SteelType val;
    WeaponType* pWeaponType = GrabHandle<WeaponType*>(hWeaponType);
    val.set ( pWeaponType->GetDamageCategory() );

    return val;
}

SteelType Application::getWeaponTypeAnimation(SteelType::Handle hWeaponType)
{
    SteelType val;
    WeaponType* pWeaponType = GrabHandle<WeaponType*>(hWeaponType);
    val.set ( pWeaponType->GetAnimation() );

    return val;
}

SteelType Application::weaponTypeHasAnimation(SteelType::Handle hWeaponType)
{
    SteelType val;
    WeaponType* pWeaponType = GrabHandle<WeaponType*>(hWeaponType);
    val.set ( pWeaponType->GetAnimation() != NULL );

    return val;
}


SteelType Application::getDamageCategoryResistance(SteelType::Handle hICharacter, int damage_category)
{
    SteelType val;

    eDamageCategory type = static_cast<eDamageCategory>(damage_category);
    ICharacter* iChar = GrabHandle<ICharacter*>(hICharacter);

    val.set ( iChar->GetDamageCategoryResistance(type) );

    return val;
}

SteelType Application::getHitSound(SteelType::Handle hWeaponType)
{

    return SteelType();
}

SteelType Application::getMissSound(SteelType::Handle hWeaponType)
{
    return SteelType();
}

SteelType Application::getUnarmedHitSound(SteelType::Handle hICharacter)
{
    return SteelType();
}

SteelType Application::getUnarmedMissSound(SteelType::Handle hICharacter)
{
    return SteelType();
}

SteelType Application::getAnimation(const std::string& name)
{
    SteelType val;
    
    Animation * pAnim = GetAbilityManager()->GetAnimation(name);
    
    if(pAnim == NULL) throw CL_Exception("Animation: " + name + " was missing.");
    
    val.set (pAnim);
    
    return val;
}


SteelType Application::invokeEquipment(SteelType::Handle hEquipment)
{
    Equipment* equipment = GrabHandle<Equipment*>(hEquipment);
    equipment->Invoke();

    return SteelType();
}

SteelType Application::attackCharacter(SteelType::Handle hICharacter)
{
    ICharacter* iCharacter = GrabHandle<ICharacter*>(hICharacter);
    iCharacter->Attacked();

    return SteelType();
}

SteelType Application::log(const std::string& str)
{
	CL_Console::write_line(str);
	return SteelType();
}

SteelType Application::showExperience(const SteelArray&  characters, const SteelArray& xp_gained, const SteelArray& oldLevels)
{
    mExperienceState.Init();
    for(int i=0;i<characters.size();i++)
    {
	Character* c = GrabHandle<Character*>(characters[i]);
	mExperienceState.AddCharacter(c,xp_gained[i],oldLevels[i]);
    }
    
    mStates.push_back(&mExperienceState);
    run();
    return SteelType();
}

IApplication * IApplication::GetInstance()
{
    return pApp;
}


IParty * Application::GetParty() const
{
    return mpParty;
}


ItemManager * Application::GetItemManager()
{
    return &mItemManager;
}

AbilityManager * Application::GetAbilityManager()
{
    return &mAbilityManager;
}


CL_ResourceManager& Application::GetResources()
{
    return m_resources;
}


Application::Application():mpParty(0),
        mbDone(false)

{
    mpParty = new Party();
}

Application::~Application()
{

}



CL_Rect Application::GetDisplayRect() const
{
    return CL_Rect(0,0,GetScreenWidth(), GetScreenHeight());

}

void Application::setupClanLib()
{



}

void Application::teardownClanLib()
{
}


double Application::get_value_for_axis_direction(IApplication::AxisDirection dir) const
{
    // TODO: Make these dynamic, some joysticks are different
    switch(dir)
    {
	case AXIS_LEFT:
	    return -1.0;
	case AXIS_RIGHT:
	    return 1.0;
	case AXIS_UP:
	    return -1.0;
	case AXIS_DOWN:
	    return 1.0;
    }
}

IApplication::AxisDirection Application::get_direction_for_value(IApplication::Axis axis, double value) const
{
    if(value == 0.0) return AXIS_NEUTRAL;
    if(axis == IApplication::AXIS_HORIZONTAL)
    {
	if(get_value_for_axis_direction(AXIS_LEFT) >0)
	{
	    if(value >0) return AXIS_LEFT;
	    else return AXIS_RIGHT;
	}
	else
	{
	    if(value >0) return AXIS_RIGHT;
	    else return AXIS_LEFT;
	}
    }
    else if(axis == IApplication::AXIS_VERTICAL)
    {
	if(get_value_for_axis_direction(AXIS_UP) >0)
	{
	    if(value >0) return AXIS_UP;
	    else return AXIS_DOWN;
	}
	else
	{
	    if(value >0) return AXIS_DOWN;
	    else return AXIS_UP;
	}
    }
}


void Application::onSignalKeyDown(const CL_InputEvent &key, const CL_InputState&)
{

    // Handle raw key press
    mStates.back()->HandleKeyDown(key);
    
    // Do mappings now
    switch(key.id)
    {
	case CL_KEY_DOWN:
	    mStates.back()->HandleAxisMove(IApplication::AXIS_VERTICAL,AXIS_DOWN,get_value_for_axis_direction(AXIS_DOWN));
	    break;
	case CL_KEY_UP:
	    mStates.back()->HandleAxisMove(IApplication::AXIS_VERTICAL,AXIS_UP,get_value_for_axis_direction(AXIS_UP));
	    break;
	case CL_KEY_LEFT:
	    mStates.back()->HandleAxisMove(IApplication::AXIS_HORIZONTAL,AXIS_LEFT,get_value_for_axis_direction(AXIS_LEFT));
	    break;
	case CL_KEY_RIGHT:
	    mStates.back()->HandleAxisMove(IApplication::AXIS_HORIZONTAL,AXIS_RIGHT,get_value_for_axis_direction(AXIS_RIGHT));
	    break;
	case CL_KEY_SPACE:
	    mStates.back()->HandleButtonDown(BUTTON_CONFIRM);
	    break;
	case CL_KEY_TAB:
	    mStates.back()->HandleButtonDown(BUTTON_ALT);
	    break;
	case CL_KEY_ESCAPE:
	    mStates.back()->HandleButtonDown(BUTTON_CANCEL);
	    break;
	case CL_KEY_ENTER:
	    mStates.back()->HandleButtonDown(BUTTON_START);
	    break;
	case CL_KEY_HOME:
	    mStates.back()->HandleButtonDown(BUTTON_MENU);
	    break;
	case CL_KEY_ADD:
	    mStates.back()->HandleButtonDown(BUTTON_R);
	    break;
	case CL_KEY_SUBTRACT:
	    mStates.back()->HandleButtonDown(BUTTON_L);
	    break;
    }
    
    
}

void Application::onSignalKeyUp(const CL_InputEvent &key, const CL_InputState&)
{
    mStates.back()->HandleKeyUp(key);

        // Do mappings now
    switch(key.id)
    {
	case CL_KEY_DOWN:
	case CL_KEY_UP: 
	   mStates.back()->HandleAxisMove(IApplication::AXIS_VERTICAL,AXIS_NEUTRAL,0.0);
	    break;
	case CL_KEY_LEFT:
	case CL_KEY_RIGHT:
	   mStates.back()->HandleAxisMove(IApplication::AXIS_HORIZONTAL,AXIS_NEUTRAL,0.0);
	   break;
	case CL_KEY_SPACE:
	    mStates.back()->HandleButtonUp(BUTTON_CONFIRM);
	    break;
	case CL_KEY_TAB:
	    mStates.back()->HandleButtonUp(BUTTON_ALT);
	    break;
	case CL_KEY_ESCAPE:
	    mStates.back()->HandleButtonUp(BUTTON_CANCEL);
	    break;
	case CL_KEY_ENTER:
	    mStates.back()->HandleButtonUp(BUTTON_START);
	    break;
	case CL_KEY_HOME:
	    mStates.back()->HandleButtonUp(BUTTON_MENU);
	    break;
	case CL_KEY_ADD:
	    mStates.back()->HandleButtonUp(BUTTON_R);
	    break;
	case CL_KEY_SUBTRACT:
	    mStates.back()->HandleButtonUp(BUTTON_L);
	    break;
    }
    
}

void Application::onSignalJoystickButtonDown(const CL_InputEvent &event, const CL_InputState& state)
{
    
    if(!mStates.size()) return;
    
    switch(event.id)
    {
        // Do mappings now
  
	case 5:
	    mStates.back()->HandleButtonDown(BUTTON_CONFIRM);
	    break;
	case 0:
	    mStates.back()->HandleButtonDown(BUTTON_ALT);
	    break;
	case 1:
	    mStates.back()->HandleButtonDown(BUTTON_CANCEL);
	    break;
	case 2:
	    mStates.back()->HandleButtonDown(BUTTON_START);
	    break;
	case 4:
	    mStates.back()->HandleButtonDown(BUTTON_MENU);
	    break;
	case 6:
	    mStates.back()->HandleButtonDown(BUTTON_R);
	    break;
	case 7:
	    mStates.back()->HandleButtonDown(BUTTON_L);
	    break;
    }
    

}

void Application::onSignalJoystickButtonUp(const CL_InputEvent &event, const CL_InputState& state)
{
        
    if(!mStates.size()) return;
    
        switch(event.id)
    {
        // Do mappings now
  
	case 5:
	    mStates.back()->HandleButtonUp(BUTTON_CONFIRM);
	    break;
	case 0:
	    mStates.back()->HandleButtonUp(BUTTON_ALT);
	    break;
	case 1:
	    mStates.back()->HandleButtonUp(BUTTON_CANCEL);
	    break;
	case 2:
	    mStates.back()->HandleButtonUp(BUTTON_START);
	    break;
	case 4:
	    mStates.back()->HandleButtonUp(BUTTON_MENU);
	    break;
	case 6:
	    mStates.back()->HandleButtonUp(BUTTON_R);
	    break;
	case 7:
	    mStates.back()->HandleButtonUp(BUTTON_L);
	    break;
    }
    
    
}

void Application::onSignalJoystickAxisMove(const CL_InputEvent &event, const CL_InputState& state)
{
    
    if(event.id == 0)
    {
	mStates.back()->HandleAxisMove(AXIS_HORIZONTAL,get_direction_for_value(AXIS_HORIZONTAL,event.axis_pos), event.axis_pos);
    }
    else
    {
	mStates.back()->HandleAxisMove(AXIS_VERTICAL, get_direction_for_value(AXIS_VERTICAL,event.axis_pos),event.axis_pos);
    }
}


void Application::onSignalQuit()
{

}

void Application::RequestRedraw(const State * /*pState*/)
{
    draw();
}



AstScript * Application::LoadScript(const std::string &name, const std::string &script)
{
    return mInterpreter.prebuildAst(name,script);
}

SteelType Application::RunScript(AstScript * pScript)
{
    // Intentionally letting steel exceptions
    // Get caught by a higher layer
    return mInterpreter.runAst ( pScript );

}
SteelType Application::RunScript(AstScript *pScript, const ParameterList &params)
{
    return mInterpreter.runAst ( pScript, params );
}


void Application::registerSteelFunctions()
{
    static SteelFunctor2Arg<Application,const std::string&,const std::string&> fn_say(this,&Application::say);
    static SteelFunctor1Arg<Application,const std::string&> fn_playScene(this,&Application::playScene);
    static SteelFunctor1Arg<Application,const std::string&> fn_playSound(this,&Application::playSound);
    static SteelFunctor3Arg<Application,const std::string&,uint,uint> fn_loadLevel(this,&Application::loadLevel);
    static SteelFunctor4Arg<Application,const std::string &,uint,bool,const std::string&> fn_startBattle(this,&Application::startBattle);
    static SteelFunctor1Arg<Application,uint> fn_pause(this,&Application::pause);
    static SteelFunctor2Arg<Application,const std::string&, const std::vector<SteelType> &> fn_choice(this,&Application::choice);
    static SteelFunctor1Arg<Application,bool> fn_pop(this,&Application::pop_);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_giveNamedItem(this,&Application::giveNamedItem);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_takeNamedItem(this,&Application::takeNamedItem);
    static SteelFunctorNoArgs<Application> fn_getGold(this,&Application::getGold);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_hasItem (this,&Application::hasItem);
    static SteelFunctor1Arg<Application,const std::string &> fn_didEvent (this,&Application::didEvent);
    static SteelFunctor2Arg<Application,const std::string &, bool> fn_doEvent (this,&Application::doEvent);
    static SteelFunctor1Arg<Application,int> fn_giveGold(this,&Application::giveGold);
    static SteelFunctor3Arg<Application,const std::string &, int, bool> fn_addCharacter(this,&Application::addCharacter);

    static SteelFunctorNoArgs<Application> fn_getPartyArray(this,&Application::getPartyArray);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getItemName(this,&Application::getItemName);
    static SteelFunctor2Arg<Application,const SteelType::Handle, uint> fn_getWeaponAttribute(this,&Application::getWeaponAttribute);
    static SteelFunctor2Arg<Application,const SteelType::Handle, uint> fn_getArmorAttribute(this,&Application::getArmorAttribute);
    static SteelFunctor2Arg<Application, double, double> fn_gaussian(this,&Application::gaussian);

    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getCharacterName(this,&Application::getCharacterName);
    static SteelFunctor2Arg<Application,const SteelType::Handle,uint> fn_getCharacterAttribute(this, &Application::getCharacterAttribute);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getCharacterLevel(this, &Application::getCharacterLevel);
    static SteelFunctor2Arg<Application,const SteelType::Handle,uint> fn_getCharacterToggle(this, &Application::getCharacterToggle);
    static SteelFunctor3Arg<Application,const SteelType::Handle,uint,bool> fn_setCharacterToggle(this, &Application::setCharacterToggle);
    static SteelFunctor2Arg<Application,const SteelType::Handle, uint> fn_getEquippedWeaponAttribute(this,&Application::getEquippedWeaponAttribute);
    static SteelFunctor2Arg<Application,const SteelType::Handle, uint> fn_getEquippedArmorAttribute(this,&Application::getEquippedArmorAttribute);
    static SteelFunctor2Arg<Application,const SteelType::Handle,const std::string&> fn_addStatusEffect(this,&Application::addStatusEffect);
    static SteelFunctor2Arg<Application,const SteelType::Handle,const std::string&> fn_removeStatusEffects(this,&Application::removeStatusEffects);
    static SteelFunctor2Arg<Application,const SteelType::Handle,int> fn_doDamage(this,&Application::doDamage);

    static SteelFunctor2Arg<Application,const SteelType::Handle,int> fn_hasEquipment(this,&Application::hasEquipment);
    static SteelFunctor2Arg<Application,const SteelType::Handle,int> fn_getEquipment(this,&Application::getEquipment);
    static SteelFunctor3Arg<Application,const SteelType::Handle,int,const std::string&> fn_equip(this,&Application::equip);


    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getWeaponType(this,&Application::getWeaponType);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getArmorType(this,&Application::getArmorType);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getWeaponTypeDamageCategory(this,&Application::getWeaponTypeDamageCategory);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getWeaponTypeAnimation(this,&Application::getWeaponTypeAnimation);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_weaponTypeHasAnimation(this,&Application::weaponTypeHasAnimation);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getWeaponScriptMode(this,&Application::getWeaponScriptMode);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_invokeEquipment(this,&Application::invokeEquipment);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_attackCharacter(this,&Application::attackCharacter);

    static SteelFunctor2Arg<Application,const SteelType::Handle,int> fn_getDamageCategoryResistance(this,&Application::getDamageCategoryResistance);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getHitSound(this,&Application::getHitSound);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getMissSound(this,&Application::getMissSound);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getUnarmedHitSound(this,&Application::getUnarmedHitSound);
    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getUnarmedMissSound(this,&Application::getUnarmedMissSound);
    
    static SteelFunctor1Arg<Application,const std::string&> fn_getAnimation(this,&Application::getAnimation);
    static SteelFunctor1Arg<Application,const std::string&> fn_log(this,&Application::log);
    static SteelFunctor3Arg<Application,const SteelArray&,const SteelArray&, const SteelArray&> fn_showExperience(this,&Application::showExperience);



    mInterpreter.pushScope();

    steelConst("$_HAND",Equipment::EHAND);
    steelConst("$_OFFHAND",Equipment::EOFFHAND);
    steelConst("$_HEAD",Equipment::EHEAD);
    steelConst("$_HANDS",Equipment::EHANDS);
    steelConst("$_BODY",Equipment::EBODY);
    steelConst("$_FINGER1",Equipment::EFINGER1);
    steelConst("$_FINGER2",Equipment::EFINGER2);
    steelConst("$_FEET",Equipment::EFEET);

    steelConst("$_SM_ATTACK_BEFORE",Weapon::ATTACK_BEFORE);
    steelConst("$_SM_ATTACK_AFTER",Weapon::ATTACK_AFTER);
    steelConst("$_SM_FORGO_ATTACK",Weapon::FORGO_ATTACK);
    steelConst("$_SM_WORLD_ONLY",Weapon::WORLD_ONLY);


    steelConst("$_HP",Character::CA_HP);
    steelConst("$_MP",Character::CA_MP);
    steelConst("$_STR",Character::CA_STR);
    steelConst("$_DEF",Character::CA_DEF);
    steelConst("$_DEX",Character::CA_DEX);
    steelConst("$_EVD",Character::CA_EVD);
    steelConst("$_MAG",Character::CA_MAG);
    steelConst("$_RST",Character::CA_RST);
    steelConst("$_LCK",Character::CA_LCK);
    steelConst("$_JOY",Character::CA_JOY);

    steelConst("$_HIT",Weapon::HIT);
    steelConst("$_ATTACK", Weapon::ATTACK);
    steelConst("$_CRITICAL", Weapon::CRITICAL);

    steelConst("$_AC", Armor::AC);
    steelConst("$_RESIST", Armor::RESIST);

    steelConst("$_DRAW_ILL",Character::CA_DRAW_ILL);
    steelConst("$_DRAW_STONE",Character::CA_DRAW_STONE);
    steelConst("$_DRAW_BERSERK",Character::CA_DRAW_BERSERK);
    steelConst("$_DRAW_WEAK",Character::CA_DRAW_WEAK);
    steelConst("$_DRAW_PARALYZED",Character::CA_DRAW_PARALYZED);
    steelConst("$_DRAW_TRANSLUCENT",Character::CA_DRAW_TRANSLUCENT);
    steelConst("$_DRAW_MINI",Character::CA_DRAW_MINI);
    steelConst("$_CAN_ACT",Character::CA_CAN_ACT);
    steelConst("$_CAN_FIGHT",Character::CA_CAN_FIGHT);
    steelConst("$_CAN_CAST",Character::CA_CAN_CAST);
    steelConst("$_CAN_SKILL",Character::CA_CAN_SKILL);
    steelConst("$_CAN_ITEM",Character::CA_CAN_ITEM);
    steelConst("$_CAN_RUN",Character::CA_CAN_RUN);
    steelConst("$_ALIVE",Character::CA_ALIVE);

    steelConst("$_MAXHP",Character::CA_MAXHP);
    steelConst("$_MAXMP",Character::CA_MAXMP);

    steelConst("$_BASH",BASH);
    steelConst("$_JAB",JAB);
    steelConst("$_SLASH",SLASH);
    steelConst("$_HOLY",HOLY);
    steelConst("$_DARK",DARK);
    steelConst("$_FIRE",FIRE);
    steelConst("$_WATER",WATER);
    steelConst("$_WIND",WIND);
    steelConst("$_EARTH",EARTH);
    steelConst("$_LIGHTNING",LIGHTNING);
    steelConst("$_ICE",ICE);
    steelConst("$_POISON",POISON);

    mInterpreter.addFunction("normal_random", &fn_gaussian);
    mInterpreter.addFunction("log",&fn_log);

    mInterpreter.addFunction("say",&fn_say);
    mInterpreter.addFunction("playScene", &fn_playScene);
    mInterpreter.addFunction("playSound", &fn_playSound);
    mInterpreter.addFunction("loadLevel", &fn_loadLevel);
    mInterpreter.addFunction("startBattle", &fn_startBattle);
    mInterpreter.addFunction("pause",&fn_pause);
    mInterpreter.addFunction("choice", &fn_choice);
    mInterpreter.addFunction("pop", &fn_pop);
    mInterpreter.addFunction("giveNamedItem", &fn_giveNamedItem );
    mInterpreter.addFunction("takeNamedItem", &fn_takeNamedItem );
    mInterpreter.addFunction("getGold", &fn_getGold );
    mInterpreter.addFunction("hasItem", &fn_hasItem );
    mInterpreter.addFunction("getItemName",&fn_getItemName);
    mInterpreter.addFunction("didEvent", &fn_didEvent );
    mInterpreter.addFunction("doEvent", &fn_doEvent );
    mInterpreter.addFunction("giveGold", &fn_giveGold );
    mInterpreter.addFunction("addCharacter", &fn_addCharacter );

    mInterpreter.addFunction("getPartyArray", &fn_getPartyArray);
    mInterpreter.addFunction("attackCharacter", &fn_attackCharacter);
    mInterpreter.addFunction("getWeaponAttribute", &fn_getWeaponAttribute);
    mInterpreter.addFunction("getWeaponScriptMode",&fn_getWeaponScriptMode);
    mInterpreter.addFunction("getArmorAttribute", &fn_getArmorAttribute);

    mInterpreter.addFunction("getCharacterAttribute", &fn_getCharacterAttribute);
    mInterpreter.addFunction("getCharacterLevel", &fn_getCharacterLevel);
    mInterpreter.addFunction("getCharacterToggle", &fn_getCharacterToggle);
    mInterpreter.addFunction("setCharacterToggle", &fn_setCharacterToggle);
    mInterpreter.addFunction("getCharacterName", &fn_getCharacterName);
    mInterpreter.addFunction("getEquippedWeaponAttribute",&fn_getEquippedWeaponAttribute);
    mInterpreter.addFunction("getEquippedArmorAttribute",&fn_getEquippedArmorAttribute);
    mInterpreter.addFunction("addStatusEffect", &fn_addStatusEffect);
    mInterpreter.addFunction("removeStatusEffects", &fn_removeStatusEffects);
    mInterpreter.addFunction("doDamage",&fn_doDamage);
    mInterpreter.addFunction("hasEquipment",&fn_hasEquipment);
    mInterpreter.addFunction("getEquipment",&fn_getEquipment);
    mInterpreter.addFunction("equip",&fn_equip);

    mInterpreter.addFunction("getWeaponType",&fn_getWeaponType);
    mInterpreter.addFunction("getArmorType",&fn_getArmorType);
    mInterpreter.addFunction("getWeaponTypeDamageCategory",&fn_getWeaponTypeDamageCategory);
    mInterpreter.addFunction("getWeaponTypeAnimation",&fn_getWeaponTypeAnimation);
    mInterpreter.addFunction("weaponTypeHasAnimation",&fn_weaponTypeHasAnimation);
    mInterpreter.addFunction("getDamageCategoryResistance",&fn_getDamageCategoryResistance);
    mInterpreter.addFunction("invokeEquipment",&fn_invokeEquipment);
    mInterpreter.addFunction("getHitSound",&fn_getHitSound);
    mInterpreter.addFunction("getMissSound",&fn_getMissSound);
    mInterpreter.addFunction("getUnarmedHitSound",&fn_getUnarmedHitSound);
    mInterpreter.addFunction("getUnarmedMissSound",&fn_getUnarmedMissSound);
    
    mInterpreter.addFunction("getAnimation",&fn_getAnimation);
    mInterpreter.addFunction("showExperience", &fn_showExperience);

//        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &webtype);
//       SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);

}

void Application::queryJoystick()
{
#if 0
    if(m_window.get_ic().get_joystick_count())
    {
	CL_InputDevice& joystick = m_window.get_ic().get_joystick(0);
	mStates.back()->HandleAxisMove(AXIS_HORIZONTAL, joystick.get_axis(0));
	mStates.back()->HandleAxisMove(AXIS_VERTICAL, joystick.get_axis(1));
    }
#endif
}

void Application::draw()
{
    CL_Rect dst = GetDisplayRect();

    m_window.get_gc().push_cliprect( dst);

    std::vector<State*>::iterator end = mStates.end();

    for (std::vector<State*>::iterator iState = mStates.begin();
            iState != end; iState++)
    {
        State * pState = *iState;
        pState->Draw(dst, m_window.get_gc());

        if (pState->LastToDraw()) break; // Don't draw any further.

    }

    m_window.get_gc().pop_cliprect();
}

void Application::run()
{
    State * backState = mStates.back();

    backState->SteelInit(&mInterpreter);
    backState->Start();
    unsigned int then = CL_System::get_time();
    while (!backState->IsDone())
    {
	queryJoystick();
        draw();
        m_window.flip();

        CL_KeepAlive::process();
#if 0
        if (count++ % 50 == 0)
            std::cout << "FPS " <<  frameRate.get_fps() << std::endl;
#endif
        unsigned int now = CL_System::get_time();

        if (now - then > MS_BETWEEN_MOVES)
        {
            if ( !backState->DisableMappableObjects())
            {
                mMapState.MoveMappableObjects();
                mStates.back()->MappableObjectMoveHook();
            }
            then = now;
        }


    }


    mStates.back()->Finish();
    mStates.back()->SteelCleanup(&mInterpreter);

    mStates.pop_back();


}

void Application::loadscript(std::string &o_str, const std::string & filename)
{
    std::ifstream in;
    std::string line;
    in.open(filename.c_str());

    while (in)
    {
        getline(in,line);
        o_str += line;
    }
    in.close();
}


int Application::main(const std::vector<CL_String> &args)
{
    GraphicsManager::initialize();

#ifndef NDEBUG

    CL_ConsoleWindow console("Stone Ring Debug",80,100);
#endif
    int njoystick = -1;
    setupClanLib();

    for(int i=0;i<args.size();i++)
    {
	std::string string = args[i];
	if(string.substr(0,5) == "--js=")
	{
		njoystick = atoi(string.substr(5).c_str());
	}
    }


    //CL_Display::get_buffer()
    try
    {
        registerSteelFunctions();

        m_resources = CL_ResourceManager ( "Media/resources.xml" );

#ifdef NDEBUG
        std::string name = CL_String_load("Configuration/name", m_resources);
#else
        std::string name = CL_String_load("Configuration/name", m_resources) + " (DEBUG)";
#endif
        mGold = CL_String_load("Game/Currency",m_resources);


        CL_DisplayWindowDescription desc;
        desc.set_title(name);
        desc.set_size(CL_Size(WINDOW_WIDTH,WINDOW_HEIGHT), true);


        m_window = CL_DisplayWindow(desc);
	

	std::string battleConfig = CL_String_load("Configuration/BattleConfig",m_resources);
	mBattleConfig.Load(battleConfig);
	mBattleState.SetConfig(&mBattleConfig);

        //for(int i =0; i < m_window.get_buffer_count(); i++)
        //  m_window.get_buffer(i).to_format(CL_PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));

        m_window.get_gc().clear(CL_Colorf(0.0f, 0.0f, 0.0f));


        mAppUtils.LoadGameplayAssets("",m_resources);
        std::string startinglevel = CL_String_load("Game/StartLevel",m_resources);
        std::string initscript;
        loadscript(initscript,CL_String_load("Game/StartupScript",m_resources));
        mInterpreter.run("Init",initscript);

        showRechargeableOnionSplash();
        showIntro();

        Level * pLevel = new Level();
        pLevel->Load(startinglevel, m_resources);
        pLevel->Invoke();

        mMapState.SetDimensions(GetDisplayRect());
        mMapState.PushLevel ( pLevel, 1,1 );

        mStates.push_back( &mMapState );
    }
    catch (CL_Exception error)
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;

#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }
    catch (SteelException ex)
    {
        std::cerr << "Steel Exception on line " << ex.getLine()
        << " of " << ex.getScript() << ':' << ex.getMessage() << std::endl;
#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }


    CL_InputDevice keyboard = m_window.get_ic().get_keyboard();

    CL_Slot slot_quit = m_window.sig_window_close().connect(this, &Application::onSignalQuit);
    CL_Slot slot_key_down = keyboard.sig_key_down().connect(this, &Application::onSignalKeyDown);
    CL_Slot slot_key_up  = keyboard.sig_key_up().connect(this, &Application::onSignalKeyUp);
    
    CL_Slot joystickDown;
    CL_Slot joystickUp;
    CL_Slot joystickAxis;
    
    if(njoystick < m_window.get_ic().get_joystick_count()){
	std::cout << "Joystick count = " << m_window.get_ic().get_joystick_count();
#if 1 
	CL_InputDevice& joystick = m_window.get_ic().get_joystick(njoystick);
	joystickDown = joystick.sig_key_down().connect(this,&Application::onSignalJoystickButtonDown);
	joystickUp = joystick.sig_key_up().connect(this,&Application::onSignalJoystickButtonUp);
	joystickAxis = joystick.sig_axis_move().connect(this,&Application::onSignalJoystickAxisMove);
#endif
    }



    try
    {

        m_window.get_gc().clear(CL_Colorf(0.0f, 0.0f, 0.0f));


        while (mStates.size())
            run();

#ifndef NDEBUG
        console.wait_for_key();
#endif


        teardownClanLib();
    }
    catch (SteelException ex)
    {
        while (mStates.size())
            mStates.pop_back();

        showError ( ex.getLine(), ex.getScript(), ex.getMessage() );


    }
    catch (CL_Exception error)
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;
    }

    mInterpreter.popScope();

    return 0;

}

void Application::showRechargeableOnionSplash()
{
}

void Application::showIntro()
{

    CL_InputDevice keyboard = m_window.get_ic().get_keyboard();
    CL_Image splash(m_window.get_gc(),"Configuration/splash", &m_resources);
    CL_Image background(m_window.get_gc(),"Configuration/splashbg", &m_resources);

    // CL_GraphicContext *gc = m_window.get_gc();

    int displayX = (WINDOW_WIDTH - splash.get_width()) / 2;
    int displayY = (WINDOW_HEIGHT - splash.get_height()) / 2;



    while (!keyboard.get_keycode(CL_KEY_ENTER))
    {
	if(m_window.get_ic().get_joystick_count())
	{
	    CL_InputDevice& joystick = m_window.get_ic().get_joystick(0);
	    if(joystick.get_keycode(5)) break;
	}
	

        background.draw(m_window.get_gc(),0,0);
        splash.draw(m_window.get_gc(),static_cast<float>(displayX),
                    static_cast<float>(displayY));

        m_window.flip();
        CL_KeepAlive::process();
    }

    // Wait for them to release the key before moving on.
    while (keyboard.get_keycode(CL_KEY_ENTER)) CL_KeepAlive::process();



}

int Application::calc_fps(int frame_time)
{
    static int fps_result = 0;
    static int fps_counter = 0;
    static int total_time = 0;

    total_time += frame_time;
    if (total_time >= 1000)     // One second has passed
    {
        fps_result = fps_counter + 1;
        fps_counter = total_time = 0;
    }
    fps_counter++;      // Increase fps

    return fps_result;
}



class Program
{
public:
    static int main(const std::vector<CL_String> &args)
    {
        CL_SetupCore setup_core;
        CL_SetupDisplay setup_display;
        CL_SetupGL setup_gl;

        Application app;
        pApp = &app;
        return app.main(args);
    }
};

CL_ClanApplication app(&Program::main);









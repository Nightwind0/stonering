	#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/sound.h>
//#include <ClanLib/vorbis.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <malloc.h>
#include "Application.h"
#include "Level.h"
#include "Party.h"
#include "ChoiceState.h"
#include "GraphicsManager.h"
#include "MonsterRef.h"
#include "Monster.h"
#include "Weapon.h"
#include "WeaponType.h"
#include "WeaponClass.h"
#include "GeneratedWeapon.h"
#include "Armor.h"
#include "StatusEffectModifier.h"
#ifndef _WINDOWS_
#include <steel/SteelType.h>
#else
#include <SteelType.h>
#endif
#include "SoundManager.h"
#include "BattleConfig.h"
#include "Animation.h"
#include "RegularItem.h"
#include "Direction.h"
#include "SaveLoadState.h"
#include "StartupState.h"
#include "StatusState.h"
#include "Omega.h"
#include "BannerState.h"
#include "GameoverState.h"
#include "GoldGetState.h"
#include "ItemGetSingleState.h"
#include "DebugControl.h"
#include "LoadingState.h"
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
const unsigned int MS_BETWEEN_MOVES = 30;


using namespace Steel;

bool gbDebugStop;



std::thread::id main_thread_id;
#if 0 
class DrawThread: public clan::Runnable {
public:
    DrawThread(Application& app):m_app(app){
    }
    virtual void run(){
        m_app.draw();
    }
private:
    Application m_app;
};
#endif

clan::DisplayWindow& Application::GetApplicationWindow()
{
    return m_window;
}


int Application::GetScreenWidth() const
{
    return WINDOW_WIDTH;
}

int Application::GetScreenHeight() const
{
    return WINDOW_HEIGHT;
}

string Application::GetCurrencyName() const
{
    return mGold;
}



SteelType Application::playScene ( const SteelType &functor )
{
#ifndef NDEBUG
    //std::cout << "Playing scene " << animation << std::endl;
#endif
    if(!functor.isFunctor()) throw clan::Exception("playScene argument wasn't a functor");
    CutSceneState cutSceneState;
    cutSceneState.Init(functor.getFunctor());
    RunState(&cutSceneState);
    return SteelType();
}

SteelType Application::playSound ( const std::string &sound )
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif
    SoundManager::PlaySound(sound);

    return SteelType();
}

SteelType Application::pushLevel ( const std::string &level, uint startX, uint startY )
{
    Level * pLevel = new Level();

    bool mapstaterunning = false;
    for(std::deque<State*>::const_iterator it = mStates.begin();
        it != mStates.end(); it++){
        if(*it == &mMapState){
            mapstaterunning = true;
            break;
        }
    }
    
    if(!mapstaterunning){
        mMapState.SetDimensions(GetDisplayRect());
        mMapState.Start();
        mStates.push_back(&mMapState);
    }
    
    std::function<void()> load_f = std::bind ( &Level::Load, pLevel, level, m_resources );
	LoadingState loader;
	loader.init(load_f);
	RunState(&loader);	
    
    mMapState.PushLevel ( pLevel, static_cast<uint> ( startX ), static_cast<uint> ( startY ) );
	pLevel->Invoke();

    return SteelType();
}

SteelType Application::loadLevel ( const std::string &level, uint startX, uint startY )
{
    Level * pLevel = new Level();
	std::function<void()> load_f = std::bind ( &Level::Load, pLevel, level, m_resources );
	LoadingState loader;
	loader.init(load_f);
	RunState(&loader);	
    //pLevel->Load ( level, m_resources );
    pLevel->Invoke();
    
#if 0 
    bool mapstaterunning = false;
    for(std::vector<State*>::const_iterator it = mStates.begin();
        it != mStates.end(); it++){
        if(*it == &mMapState){
            mapstaterunning = true;
            break;
        }
    }
    
    if(!mapstaterunning){
        mMapState.SetDimensions(GetDisplayRect());
        mMapState.Start();
        mStates.push_back(&mMapState);
    }
#endif
    
    mMapState.LoadLevel ( pLevel, static_cast<uint> ( startX ), static_cast<uint> ( startY ) );

    return SteelType();
}


void Application::PopLevelStack ( bool bAll )
{
    mMapState.Pop ( bAll );
}

SteelType Application::popLevel ( bool bAll )
{
    PopLevelStack ( bAll );

    return SteelType();
}

SteelType Application::mainMenu()
{
    MainMenu();

    return SteelType();
}

void Application::MainMenu()
{
    mMainMenuState.Init();
    RunState ( &mMainMenuState );
}

void Application::StartBattle ( const MonsterGroup &group, const std::string &backdrop )
{
#ifndef NDEBUG
    std::cout << "Encounter! Backdrop = " << backdrop << std::endl;

    const std::vector<MonsterRef*> &monsters = group.GetMonsters();

    for ( std::vector<MonsterRef*>::const_iterator it =  monsters.begin();
            it != monsters.end(); it++ )
    {
        MonsterRef * pRef = *it;
        std::cout << '\t' << pRef->GetName() << " x" << pRef->GetCount() << std::endl;
    }

#endif
    mBattleState.init ( group, backdrop );
    
    RunState(&mBattleState);
}

SteelType Application::startBattle ( const std::string &monster, uint count, bool isBoss, const std::string& backdrop )
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif

    DynamicMonsterRef* monsterRef = new DynamicMonsterRef();

    float square_root = sqrt ( ( float ) count );
    int square_size = ceil ( square_root );
    monsterRef->SetName ( monster );
    monsterRef->SetCellX ( 0 );
    monsterRef->SetCellY ( 0 );
    monsterRef->SetCount ( count );
    monsterRef->SetRows ( square_size );
    monsterRef->SetColumns ( square_size );

    std::vector<MonsterRef*> monsters;
    monsters.push_back ( monsterRef );

    mBattleState.init ( monsters,
                        monsterRef->GetRows(),
                        monsterRef->GetColumns(),
                        isBoss,
                        backdrop,
						true
  					);

    RunState(&mBattleState);


	SteelType ret;
	ret.set(mBattleState.playerWon());
    return ret;
}


SteelType Application::choice ( const std::string &choiceText,
                                const SteelType::Container &choices_ )
{
    static ChoiceState choiceState;
    std::vector<std::string> choices;
    choices.reserve ( choices_.size() );

    for ( unsigned int i = 0;i < choices_.size();i++ )
        choices.push_back ( choices_[i] );

    choiceState.Init ( choiceText, choices );


    RunState(&choiceState);
    
    SteelType selection;

    selection.set ( choiceState.GetSelection() );
	
	//std::cout << "Selected " << choiceState.GetSelection() << std::endl;

    return selection;
}

SteelType Application::message ( const std::string& text )
{
    Steel::SteelArray array;
    Steel::SteelType type;
    type.set ( text );
    array.push_back ( type );
    return menu ( array );
}

SteelType Application::say ( const std::string &speaker, const std::string &text )
{
    mSayState.Init ( speaker, text, -1, true );

    RunState(&mSayState);

    return SteelType();
}


void Application::StartGame(bool load)
{

#ifndef NDEBUG
	if(/*dump_equipment*/0){
		ItemManager::DumpItemCSV();
		return;
	}
#endif	
	mMapState.SetDimensions ( GetDisplayRect() );	
	mpParty->Clear();
    if(load){
        if(!this->load())
			return;
    }else{   
        std::string startscript;
		clan::XMLResourceDocument doc = clan::XMLResourceManager::get_doc(m_resources);
		clan::XMLResourceNode node = doc.get_resource("Game/StartupScript");
        loadscript ( startscript, String_load ( "Game/StartupScript", m_resources ), node.get_file_system() );
        mInterpreter->run ( "Startup", startscript );
    }
    RunState(&mMapState);    
}


void Application::RunOnMainThread ( std::function<void()>& functor )
{
	if(std::this_thread::get_id() == main_thread_id){
		functor();
	}else{
		clan::Event event;		
		ThreadFunctor thread_functor(event,functor);
		mFunctorMutex.lock();
		m_mainthread_functors.push(thread_functor);
		mFunctorMutex.unlock();
		event.wait();
	}
}


void Application::RunState ( State * pState, bool threaded )
{
	// Bug is this... basically finding the calling state by
	// looking at the back state is wrong. 
	// if you call this on the main thread and it thinks
	// its threaded, you get a deadlock and you're boned
	
	if(!mStates.empty()){
		mStates.back()->Covered();
	}
	
	threaded = threaded || m_threaded_mode;
	mStates.push_back ( pState );
    if(threaded){
        class RunFunctor : public Functor {
        public:
            RunFunctor(Application& app):m_app(app){
            }
            void operator()(){
                m_app.run(false);
            }
        private:
            Application& m_app;
        };
        RunFunctor functor(*this);
		std::function<void()> func = functor;
        RunOnMainThread(func);
    }else{
        run();
    }
}

void Application::showError ( int line, const std::string &script, const std::string &message )
{
    std::ostringstream os;
    os << "Script error in " << script << " on line " << line << " (" << message << ')';

    say ( "Error", os.str() );
}


SteelType Application::gaussian ( double mean, double sigma )
{
    SteelType result;
    result.set ( normal_random ( mean, sigma ) );

    return result;
}


SteelType Application::pause ( uint time )
{
    clan::System::sleep ( time );

    return SteelType();
}

SteelType Application::invokeShop ( const std::string &shoptype )
{
    return SteelType();
}


SteelType Application::getGold()
{
    SteelType val;
    val.set ( (int)mpParty->GetGold() );
    return val;
}

SteelType Application::hasItem ( const std::string &item, uint count )
{
    SteelType var;
    var.set ( mpParty->HasItem ( ItemManager::GetNamedItem ( item ), count ) );

    return var;
}

SteelType Application::didEvent ( const std::string &event )
{
    SteelType var;
    var.set ( mpParty->DidEvent ( event ) );

    return var;
}

SteelType Application::doEvent ( const std::string &event, bool bRemember )
{
    mpParty->DoEvent ( event, bRemember );
    return SteelType();
}

SteelType Application::getNamedItem ( const std::string &item ) 
{
    Item * pItem = ItemManager::GetNamedItem(item);
    
    SteelType var;
    var.set(pItem);
    
    return var;
}



SteelType Application::disposeItem ( SteelType::Handle iItem, uint count )
{
    Item* pItem = GrabHandle<Item*> ( iItem );
    mpParty->TakeItem ( pItem, count );

    return SteelType();
}

SteelType Application::takeItem ( SteelType::Handle hItem , uint count, bool silent )
{
    std::ostringstream os;
    Item* pItem = GrabHandle<Item*>(hItem);
    SteelType val;
    val.set ( mpParty->TakeItem ( pItem, count ) );

    if(!silent){
		ItemGetSingleState state;
		state.SetInverse(true);
		state.SetItem(pItem,count);
		RunState(&state);
    }
    return val;
}

SteelType Application::giveGold ( int amount )
{
    mpParty->GiveGold ( amount );
	GoldGetState state;
	if(amount <0)
		state.SetInverse(true);
	state.SetGold(amount);
	RunState(&state);
    return SteelType();
}

SteelType Application::stopMusic()
{
	SoundManager::StopMusic();
	return SteelType();
}

SteelType Application::startMusic()
{
	SoundManager::StartMusic();
	return SteelType();
}

SteelType Application::pushMusic(const std::string& music)
{
	SoundManager::PushMusic();
	SoundManager::SetMusic(music);
	return SteelType();
}

SteelType Application::popMusic()
{
	SoundManager::PopMusic();
	return SteelType();
}


SteelType Application::addCharacter ( const std::string &character, int level, bool announce )
{
    Character * pCharacter = CharacterManager::GetCharacter ( character );
    Steel::AstScript * pTNL = GetUtility ( XP_FOR_LEVEL );
    ParameterList params;
    params.push_back ( ParameterListItem ( "$_LEVEL", level ) );
    pCharacter->SetXP ( RunScript ( pTNL, params ) );
    pCharacter->SetLevel ( level );
    // TODO: This is kind of a hack... probably ought to do this via steel API
    pCharacter->PermanentAugment ( ICharacter::CA_HP, pCharacter->GetAttribute ( ICharacter::CA_MAXHP ) );
    pCharacter->PermanentAugment ( ICharacter::CA_MP, pCharacter->GetAttribute ( ICharacter::CA_MAXMP ) );
	
	bool reserves = false;
	
	if(mpParty->GetCharacterCount() >= m_max_party_size){
		reserves = true;
		m_reserve_party.AddCharacter(pCharacter);
	}else{
	    mpParty->AddCharacter ( pCharacter );
	}

    if ( announce )
    {
        std::ostringstream os;
        os << character;
		if( reserves )
			os << " joined the reserve party!";
		else os << " joined the party!";

        say ( "", os.str() );
    }

    SteelType returnPointer;

    returnPointer.set ( pCharacter );

    return returnPointer;
}

SteelType Application::changeCharacterClass ( SteelType::Handle hCharacter, const std::string& chr_class )
{
    Character * pCharacter = GrabHandle<Character*>(hCharacter);
    CharacterClass * pNewClass = CharacterManager::GetClass(chr_class);
    if(!pNewClass){
        throw clan::Exception("changeCharacterClass with bad class name");
    }
    pCharacter->ChangeClass(pNewClass);
    
    return SteelType();
}



SteelType Application::kill(SteelType::Handle hICharacter)
{
    ICharacter * iCharacter = GrabHandle<ICharacter*>(hICharacter);
    
    iCharacter->Kill();
    
    return SteelType();
}

SteelType Application::raise(SteelType::Handle hICharacter)
{
    ICharacter * iCharacter = GrabHandle<ICharacter*>(hICharacter);
    
    iCharacter->Raise();
    
    return SteelType();
}

SteelType Application::doDamage ( SteelType::Handle hICharacter, int damage )
{
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );

    if ( !pCharacter->GetToggle ( Character::CA_ALIVE ) ) return SteelType();

    int hp = pCharacter->GetAttribute ( Character::CA_HP );

    const int maxhp = pCharacter->GetAttribute ( Character::CA_MAXHP );

    if ( hp - damage <= 0 )
    {
        damage = hp;
        // Kill him/her/it
        pCharacter->Kill();
        //TODO: Check if all characters are dead and game over
    }

    if ( hp - damage > maxhp )
    {
        damage = - ( maxhp - hp );
    }

    pCharacter->PermanentAugment ( Character::CA_HP, -damage );

    SteelType newhp;
    newhp.set ( damage );

    return newhp;
}

SteelType Application::doMPDamage ( SteelType::Handle hICharacter, int damage )
{
    ICharacter * pCharacter = dynamic_cast<ICharacter*> ( hICharacter );

    if ( !pCharacter->GetToggle ( Character::CA_ALIVE ) ) return SteelType();

    int mp = pCharacter->GetAttribute ( Character::CA_MP );

    const int maxmp = pCharacter->GetAttribute ( Character::CA_MAXMP );

    if ( mp - damage <= 0 )
    {
        damage = mp;
    }

    if ( mp - damage > maxmp )
    {
        damage = - ( maxmp - mp );
    }

    pCharacter->PermanentAugment ( Character::CA_MP, -damage );

    SteelType newmp;
    newmp.set ( damage );

    return newmp;
}

SteelType Application::selectItemAdv( uint filter ) 
{
    mItemSelectState.Init ( false, filter );
    RunState ( &mItemSelectState );
    SteelType val;
    val.set ( mItemSelectState.GetSelectedItem() );
    return val;    
}

SteelType Application::selectItem ( bool battle, bool dispose )
{
    if ( !dispose )
    {
        mItemSelectState.Init ( battle, Item::REGULAR_ITEM );
    }
    else
    {
        mItemSelectState.Init ( battle, Item::REGULAR_ITEM | Item::WEAPON | Item::ARMOR );
    }

    RunState ( &mItemSelectState );

    SteelType val;
    val.set ( mItemSelectState.GetSelectedItem() );
    return val;
}

SteelType Application::getCharacterName ( const SteelType::Handle handle )
{
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( handle );
    SteelType name;
    name.set ( pCharacter->GetName() );

    return name;
}

SteelType Application::addExperience ( const SteelType::Handle hCharacter, int xp )
{
    Character * pCharacter = GrabHandle<Character*> ( hCharacter );
    pCharacter->SetXP ( pCharacter->GetXP() + xp );
    SteelType val;
    val.set ( static_cast<int> ( pCharacter->GetXP() ) );

    return val;
}

SteelType Application::getExperience ( const SteelType::Handle hCharacter )
{
    Character * pCharacter = GrabHandle<Character*> ( hCharacter );
    SteelType val;
    val.set ( static_cast<int> ( pCharacter->GetXP() ) );

    return val;
}


SteelType Application::getPartyArray()
{
    SteelType array;
    SteelType::Container vector;

    for ( int i = 0;i < mpParty->GetCharacterCount();i++ )
    {
        SteelType ptr;
        ptr.set ( mpParty->GetCharacter ( i ) );
        vector.push_back ( ptr );
    }

    array.set ( vector );

    return array;
}

SteelType Application::getReservePartyArray()
{
    SteelType array;
    SteelType::Container vector;

    for ( int i = 0;i < m_reserve_party.GetCharacterCount();i++ )
    {
        SteelType ptr;
        ptr.set ( m_reserve_party.GetCharacter ( i ) );
        vector.push_back ( ptr );
    }

    array.set ( vector );

    return array;
}

SteelType Application::reserveCharacter( const std::string& name)
{
	Character * c = mpParty->RemoveCharacter(name);
	if(c == NULL) throw clan::Exception("reserveCharacter: " + name + " not in party");
	
	m_reserve_party.AddCharacter(c);
	SteelType val;
	val.set(c);
	return val;
}

SteelType Application::deployCharacter( const std::string& name ) 
{
	Character * c = m_reserve_party.RemoveCharacter(name);
	if(c == NULL) throw clan::Exception("deployCharacter: " + name + " not in reserve party");
	
	mpParty->AddCharacter(c);
	SteelType val;
	val.set(c);
	return val;
}



SteelType Application::getCharacterLevel ( const SteelType::Handle hCharacter )
{
    ICharacter *pCharacter = dynamic_cast<ICharacter*> ( hCharacter );

    if ( pCharacter == NULL ) throw TypeMismatch();

    SteelType level;

    level.set ( ( int ) pCharacter->GetLevel() );

    return level;
}

SteelType Application::equip ( SteelType::Handle hCharacter, int slot, const std::string &equipment )
{
    Character * pCharacter = dynamic_cast<Character*> ( hCharacter );

    if ( !pCharacter ) throw TypeMismatch();

    Equipment * pEquipment = dynamic_cast<Equipment*> ( ItemManager::GetNamedItem ( equipment ) );

    SteelType result;

    if ( pEquipment != NULL )
    {
        pCharacter->Equip ( static_cast<Equipment::eSlot> ( slot ), pEquipment );
        result.set ( true );
    }
    else
    {
        result.set ( false );
    }

    return result;
}

SteelType Application::getStatusEffect( const std::string &name )
{
    SteelType var;
    var.set(AbilityManager::GetStatusEffect(name));
    return var;
}

SteelType Application::addStatusEffect ( SteelType::Handle hCharacter, SteelType::Handle hStatusEffect )
{
    ICharacter *pCharacter = GrabHandle<ICharacter*> ( hCharacter );
    StatusEffect* pEffect = GrabHandle<StatusEffect*>(hStatusEffect);

    pCharacter->AddStatusEffect ( pEffect );

    return SteelType();
}

SteelType Application::removeStatusEffect ( SteelType::Handle hCharacter, SteelType::Handle hStatusEffect )
{
    ICharacter *pCharacter = GrabHandle<ICharacter*> ( hCharacter );
    StatusEffect * pEffect = GrabHandle<StatusEffect*> ( hStatusEffect );

    pCharacter->RemoveEffect ( pEffect );

    return SteelType();
}

SteelType Application::statusEffectChance ( SteelType::Handle hCharacter, SteelType::Handle hStatusEffect )
{
    ICharacter *pCharacter = GrabHandle<ICharacter*> ( hCharacter );
    StatusEffect * pEffect = GrabHandle<StatusEffect*> ( hStatusEffect );
    
    SteelType var;
    var.set ( pCharacter->StatusEffectChance(pEffect) );
    return var;
}

SteelType Application::hasEquipment ( SteelType::Handle hICharacter, int slot )
{
    SteelType result;

    ICharacter *iCharacter = GrabHandle<ICharacter*> ( hICharacter );
    Monster * pMonster = dynamic_cast<Monster*> ( iCharacter );

    if ( pMonster ) // Monsters currently can't have equipment
    {
        result.set ( false );
        return result;
    }

    Character *pCharacter = dynamic_cast<Character*> ( iCharacter );

    result.set ( pCharacter->HasEquipment ( static_cast<Equipment::eSlot> ( slot ) ) );
    return result;
}

SteelType Application::getEquipment ( SteelType::Handle hCharacter, int slot )
{
    SteelType result;
    Character *pCharacter = GrabHandle<Character*> ( hCharacter );
    result.set ( pCharacter->GetEquipment ( static_cast<Equipment::eSlot> ( slot ) ) );
    return result;
}

SteelType Application::getEquippedWeaponAttribute ( const SteelType::Handle hICharacter, uint attr, int slot )
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );
    result.set ( pCharacter->GetEquippedWeaponAttribute ( static_cast<Weapon::eAttribute> ( attr ), (Equipment::eSlot)slot ) );

    return result;
}

SteelType Application::getEquippedArmorAttribute ( const SteelType::Handle hICharacter, uint attr )
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );
    result.set ( pCharacter->GetEquippedArmorAttribute ( static_cast<Armor::eAttribute> ( attr ) ) );

    return result;
}

SteelType Application::getItemName ( const SteelType::Handle hItem )
{
    SteelType result;
    Item * pItem = GrabHandle<Item*> ( hItem );
    result.set ( pItem->GetName() );
    return result;
}

SteelType Application::getWeaponAttribute ( const SteelType::Handle hWeapon, uint attr )
{
    SteelType result;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    float value = pWeapon->GetWeaponAttribute ( static_cast<Weapon::eAttribute> ( attr ) );
    result.set ( value );

    return result;
}

SteelType Application::getArmorAttribute ( const SteelType::Handle hArmor, uint attr )
{
    SteelType result;
    Armor * pArmor = GrabHandle<Armor*> ( hArmor );
    float value = pArmor->GetArmorAttribute ( static_cast<Armor::eAttribute> ( attr ) );
    result.set ( value );

    return result;
}

SteelType Application::getCharacterAttribute ( const SteelType::Handle hICharacter, uint attr )
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute> ( attr );

    result.set ( pCharacter->GetAttribute ( theAttr ) );

    return result;
}

SteelType Application::augmentCharacterAttribute(const SteelType::Handle hICharacter, uint i_attr, double augment)
{
    ICharacter * pCharacter = GrabHandle<ICharacter*>(hICharacter);
    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(i_attr);
    
    pCharacter->PermanentAugment(attr,augment);
    
    return SteelType();
}

SteelType Application::getCharacterToggle ( const SteelType::Handle hICharacter, uint attr )
{
    SteelType result;
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute> ( attr );

    result.set ( pCharacter->GetToggle ( theAttr ) );

    return result;
}

SteelType Application::setCharacterToggle ( const SteelType::Handle hICharacter, uint attr, bool toggle )
{
    ICharacter * pCharacter = GrabHandle<ICharacter*> ( hICharacter );
    ICharacter::eCharacterAttribute theAttr = static_cast<ICharacter::eCharacterAttribute> ( attr );

    pCharacter->SetToggle ( theAttr, toggle );

    return SteelType();
}


SteelType Application::getWeaponType ( SteelType::Handle hWeapon )
{
    SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    val.set ( pWeapon->GetWeaponType() );

    return val;
}

SteelType Application::getArmorType ( SteelType::Handle hArmor )
{
    SteelType val;
    Armor* pArmor = GrabHandle<Armor*> ( hArmor );
    val.set ( pArmor->GetArmorType() );

    return val;
}

SteelType Application::getWeaponTypeDamageCategory ( SteelType::Handle hWeaponType )
{
    SteelType val;
    WeaponType* pWeaponType = GrabHandle<WeaponType*> ( hWeaponType );
    val.set ( pWeaponType->GetDamageCategory() );

    return val;
}



SteelType Application::getWeaponAnimation ( SteelType::Handle hWeapon )
{
    SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    val.set ( pWeapon->GetAnimation() );

    return val;
}

SteelType Application::weaponHasAnimation ( SteelType::Handle hWeapon )
{
    SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    val.set ( pWeapon->GetAnimation() != NULL );

    return val;
}

SteelType 	Application::weaponHasAnimationScript(SteelType::Handle hWeapon){
	SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    val.set ( pWeapon->GetAnimationScript() != NULL );

    return val;	
}
SteelType 	Application::runWeaponAnimationScript(SteelType::Handle hWeapon, SteelType::Handle hCharacter, SteelType::Handle hTarget){
	SteelType val;
    Weapon* pWeapon = GrabHandle<Weapon*> ( hWeapon );
    assert(pWeapon->GetAnimationScript());
	ICharacter * pCharacter = GrabHandle<ICharacter*> ( hCharacter );
	ICharacter * pTarget = GrabHandle<ICharacter*> (hTarget );

	ParameterList params;
	params.push_back(ParameterListItem("$_Weapon",pWeapon));
	params.push_back(ParameterListItem("$_Character",pCharacter));
	params.push_back(ParameterListItem("$_Target", pTarget));
	
	val = pWeapon->GetAnimationScript()->ExecuteScript(params);
	
    return val;		
}


SteelType Application::getDamageCategoryResistance ( SteelType::Handle hICharacter, int damage_category )
{
    SteelType val;

    DamageCategory::eDamageCategory type = static_cast<DamageCategory::eDamageCategory> ( damage_category );
    ICharacter* iChar = GrabHandle<ICharacter*> ( hICharacter );

    val.set ( iChar->GetDamageCategoryResistance ( type ) );

    return val;
}



SteelType Application::getUnarmedHitSound ( SteelType::Handle hICharacter )
{
    return SteelType();
}

SteelType Application::getUnarmedMissSound ( SteelType::Handle hICharacter )
{
    return SteelType();
}

SteelType Application::getAnimation ( const std::string& name )
{
    SteelType val;

    Animation * pAnim = GetAbilityManager()->GetAnimation ( name );

    if ( pAnim == NULL ) throw clan::Exception ( "Animation: " + name + " was missing." );

    val.set ( pAnim );

    return val;
}

SteelType Application::getMonsterDrops ( const SteelType::Handle hMonster )
{
    SteelArray array;
    Monster * pMonster = GrabHandle<Monster*>(hMonster);
    
    for(std::list<ItemRef*>::const_iterator iter = pMonster->GetDropsBegin();
        iter != pMonster->GetDropsEnd(); iter++)
    {
        SteelType var;
        var.set ( (*iter)->GetItem() );
        array.push_back(var);
    }
    
    SteelType var;
    var.set(array);
    return var;    
}


SteelType Application::invokeArmor ( SteelType::Handle pICharacter, SteelType::Handle hArmor )
{
    Armor * pArmor = GrabHandle<Armor*> ( hArmor );
    ICharacter* iCharacter = GrabHandle<ICharacter*> ( pICharacter );
    ParameterList params;
    params.push_back ( ParameterListItem ( "$_Character", iCharacter ) );
    pArmor->Invoke ( params );

    return SteelType();
}

SteelType Application::invokeWeapon ( SteelType::Handle pICharacter, SteelType::Handle pTargetChar, SteelType::Handle hWeapon, uint hand, uint invokeTime )
{
    Weapon *pWeapon = GrabHandle<Weapon*> ( hWeapon );
    ICharacter* iCharacter = GrabHandle<ICharacter*> ( pICharacter );
    Weapon::eScriptMode mode = static_cast<Weapon::eScriptMode> ( invokeTime );
    ParameterList params;
    params.push_back ( ParameterListItem ( "$_Character", iCharacter ) );
	params.push_back ( ParameterListItem ( "$_Target", pTargetChar ) );
	params.push_back ( ParameterListItem ( "$_Weapon", hWeapon ) );
	params.push_back ( ParameterListItem ( "$_Hand", (int)hand ) );
	params.push_back ( ParameterListItem ( "$_When", (int)invokeTime ) );
	
    pWeapon->Invoke ( mode, params );

    return SteelType();
}


SteelType Application::attackCharacter ( SteelType::Handle hICharacter, SteelType::Handle hIAttacker, uint i_category, bool melee, int amount )
{
    ICharacter* iCharacter = GrabHandle<ICharacter*> ( hICharacter );
    ICharacter* pAttacker = GrabHandle<ICharacter*> ( hIAttacker );
    DamageCategory::eDamageCategory category = static_cast<DamageCategory::eDamageCategory>(i_category);
    iCharacter->Attacked(pAttacker,category,melee,amount);
    doDamage(hICharacter,amount);

    return SteelType();
}

SteelType Application::isArmor ( SteelType::Handle hEquipment )
{
    Equipment* pEquipment = GrabHandle<Equipment*>(hEquipment);
    SteelType var;
    var.set( pEquipment->IsArmor() );
    return var;
}

SteelType Application::getItemType ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    val.set ( pItem->GetItemType() );

    return val;
}

SteelType Application::getItemValue ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    val.set ( ( int ) pItem->GetValue() );

    return val;
}

SteelType Application::getItemSellValue ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    val.set ( ( int ) pItem->GetSellValue() );

    return val;
}

SteelType Application::forgoAttack ( SteelType::Handle hWeapon )
{
    Weapon * pWeapon = GrabHandle<Weapon*> ( hWeapon );
    SteelType val;
    val.set ( pWeapon->ForgoAttack() );

    return val;
}

SteelType Application::weaponTypeIsRanged ( SteelType::Handle hWeaponType )
{
    WeaponType * pWeaponType = GrabHandle<WeaponType*>(hWeaponType);
    SteelType val;
    val.set ( pWeaponType->IsRanged() );
    
    return val;
}


SteelType Application::isWorldItem ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );

    if ( pRegularItem == NULL )
    {
        val.set ( false );
    }
    else
    {
        val.set ( pRegularItem->GetUseType() == RegularItem::WORLD ||
                  pRegularItem->GetUseType() == RegularItem::BOTH );
    }

    return val;
}

SteelType Application::isBattleItem ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );

    if ( pRegularItem == NULL )
    {
        val.set ( false );
    }
    else
    {
        val.set ( pRegularItem->GetUseType() == RegularItem::BATTLE ||
                  pRegularItem->GetUseType() == RegularItem::BOTH );
    }

    return val;
}


SteelType Application::getItemTargetable ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );

    if ( pRegularItem == NULL )
    {
        val.set ( RegularItem::NO_TARGET );
    }
    else
    {
        val.set ( pRegularItem->GetTargetable() );
    }

    return val;
}

SteelType Application::getItemDefaultTarget ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );

    if ( pRegularItem == NULL )
    {
        val.set ( RegularItem::PARTY );
    }
    else
    {
        val.set ( pRegularItem->GetDefaultTarget() );
    }

    return val;
}

SteelType Application::isReusableItem ( SteelType::Handle hItem )
{
    SteelType val;
    Item* pItem = GrabHandle<Item*> ( hItem );
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );

    if ( pRegularItem == NULL )
    {
        val.set ( false );
    }
    else
    {
        val.set ( pRegularItem->IsReusable() );
    }

    return val;
}

SteelType Application::useItem ( SteelType::Handle hItem, const SteelType& targets )
{
    Item* pItem = GrabHandle<Item*> ( hItem );
    Party * party = IApplication::GetInstance()->GetParty();
    RegularItem * pRegularItem = dynamic_cast<RegularItem*> ( pItem );
    SteelType used;

    if ( pRegularItem != NULL )
    {
        pRegularItem->Invoke ( targets );

        if ( !pRegularItem->IsReusable() )
        {
            used.set ( party->TakeItem ( pItem, 1 ) );
        }
    }
    else
    {
        used.set ( false );
    }

    return used;
}

SteelType Application::getCharacterSP ( const SteelType::Handle hCharacter )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);
    
    SteelType var;
    var.set((int)pChar->GetSP());
    return var;
}

SteelType Application::setCharacterSP ( const SteelType::Handle hCharacter, int sp )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);

    pChar->SetSP(sp);
    
    return SteelType();
}



SteelType Application::randomItem ( uint i_rarity, int min_value, int max_value )
{
    Item::eDropRarity rarity = static_cast<Item::eDropRarity>(i_rarity);
    SteelType var;
    var.set ( ItemManager::GetRandomItem(rarity, min_value, max_value) );
    
    return var;    
}

SteelType Application::generateRandomWeapon ( uint i_rarity,  int min_value, int max_value )
{
    Item::eDropRarity rarity = static_cast<Item::eDropRarity>(i_rarity);
    Weapon * pWeapon = ItemManager::GetRandomWeapon(rarity, min_value,max_value);
    SteelType var;
    var.set(pWeapon);
    
    return var;
}

SteelType Application::generateRandomArmor ( uint i_rarity, int min_value, int max_value )
{
    Item::eDropRarity rarity = static_cast<Item::eDropRarity>(i_rarity);    
    Armor * pArmor = ItemManager::GetRandomArmor(rarity, min_value,max_value);
    SteelType var;
    var.set(pArmor);
    
    return var;
}


SteelType Application::doEquipmentStatusEffectInflictions ( SteelType::Handle hEquipment, SteelType::Handle hTarget )
{
    // Return an array of status effects 
    SteelType effects;
    effects.set(SteelType::Container());
    Equipment* pEquipment = GrabHandle<Equipment*>(hEquipment);
    ICharacter* pTarget = GrabHandle<ICharacter*>(hTarget);
    // Now see if this armor is goign to inflict any status effects
    for(Equipment::StatusEffectInflictionSet::const_iterator iter = pEquipment->GetStatusEffectInflictionsBegin();
        iter != pEquipment->GetStatusEffectInflictionsEnd(); iter++)
    {
        double r = ranf();
        if (r < iter->second->GetModifier())
        {
            double block = ranf();
            if(block < pTarget->StatusEffectChance(iter->second->GetStatusEffect())){
                SteelType var;
                var.set(iter->second->GetStatusEffect());
                effects.add ( var );
                pTarget->AddStatusEffect(iter->second->GetStatusEffect());
            }
        }
    }

    return effects;
}

SteelType Application::giveItems( const Steel::SteelArray& i_items, bool silent )
{
	std::map<Item*,int> items;
	for(Steel::SteelArray::const_iterator it = i_items.begin(); it != i_items.end(); it++){
		if(!(it->isHandle() && it->isValidHandle()))
			throw Steel::TypeMismatch();
		Item * pItem = GrabHandle<Item*>(*it);
		std::map<Item*,int>::iterator find_it = items.find(pItem);
		if(find_it == items.end()){
			items[pItem] = 1;
		}else{
			++find_it->second;
		}
	}
	for(std::map<Item*,int>::const_iterator it = items.begin(); it != items.end(); it++){
		mpParty->GiveItem(it->first,it->second);
		if(!silent){
			ItemGetSingleState state;
			state.SetInverse(false);
			state.SetItem(it->first,it->second);
			RunState(&state);		
		}
	}
#if 0 
	if(items.size() != counts.size()){
		throw Steel::ParamMismatch();
	}
	std::vector<Item*> items_array;
	std::vector<uint> counts_array;
	for(int i = 0;i < items.size(); i++){
		Item * pItem = GrabHandle<Item*>(items[i]);
		items_array.push_back(pItem);
		counts_array.push_back( (int) counts[i] );
		mpParty->GiveItem( pItem, (int) counts[i] );
	}
  
    if(!silent){
		mItemGetState.SetItems(items_array,counts_array);
		RunState(&mItemGetState);
    }
#endif
    return SteelType();
}

SteelType Application::giveItem( const SteelType::Handle hItem, int count, bool silent )
{
	Item * pItem = GrabHandle<Item*>(hItem);
	mpParty->GiveItem(pItem,count);
    if(!silent){
		ItemGetSingleState state;
		state.SetInverse(false);
		state.SetItem(pItem,count);
		RunState(&state);
    }
    return SteelType();
}



SteelType Application::log ( const std::string& str )
{
    clan::Console::write_line ( str );
    return SteelType();
}

SteelType Application::inBattle()
{
    SteelType val;

    for ( int i = 0;i < mStates.size(); i++ )
    {
        State * pState = mStates[i];

        if ( pState == &mBattleState )
        {
            val.set ( true );
            return val;
        }
    }

    val.set ( false );

    return val;
}

SteelType Application::showExperience ( const SteelArray&  characters, const SteelArray& xp_gained,
                                        const SteelArray& oldLevels , const SteelArray& sp_gained)
{
    mExperienceState.Init();

    for ( int i = 0;i < characters.size();i++ )
    {
        Character* c = GrabHandle<Character*> ( characters[i] );
        mExperienceState.AddCharacter ( c, xp_gained[i], oldLevels[i], sp_gained[i] );
    }
    
    RunState(&mExperienceState);

    return SteelType();
}

int Application::DynamicMenu(const std::vector<std::string>& options)
{
	DynamicMenuState state;
	state.Init( options );
	RunState(&state);
	return state.GetSelection();
}

SteelType Application::menu ( const SteelArray& array )
{
    DynamicMenuState* pState = new DynamicMenuState();
    std::vector<std::string> options;

    for ( SteelArray::const_iterator iter = array.begin(); iter != array.end(); iter++ )
    {
        options.push_back ( *iter );
    }

    pState->Init ( options );
    
    RunState(pState);

    int selection = pState->GetSelection();
    SteelType val;
    val.set ( selection );
    delete pState;
    return val;
}

SteelType Application::skilltree ( SteelType::Handle hCharacter, bool buy )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);
    
    mSkillTreeState.Init(pChar,buy);
    
    RunState(&mSkillTreeState);
    
    SteelType var;
    if(mSkillTreeState.GetSelectedSkillNode())
        var.set( mSkillTreeState.GetSelectedSkillNode()->GetRef()->GetSkill() );
    return var;
}

SteelType Application::learnSkill ( SteelType::Handle hCharacter, SteelType::Handle skill, bool silent )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);
    Skill * pSkill = GrabHandle<Skill*>(skill);
    
    pChar->LearnSkill(pSkill->GetName());
	
	if(!silent){
		mSkillGetState.SetSkill(pSkill);
		RunState(&mSkillGetState);
	}
    
    return SteelType();
}

SteelType Application::hasSkill ( SteelType::Handle hCharacter, const std::string& skillName )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);

    SteelType var;
    var.set(pChar->HasSkill(skillName));
    
    return var;
}


SteelType Application::getSkill(const std::string& whichskill)
{

    if(!AbilityManager::SkillExists(whichskill)){
        throw clan::Exception("(getSkill) Skill doesn't exist: " + whichskill);
    }

    Skill * pSkill = AbilityManager::GetSkill(whichskill);
	assert(pSkill);

    SteelType var;
    var.set(pSkill);
    return var;
}



SteelType Application::doSkill ( SteelType::Handle hSkill, SteelType::Handle hICharacter )
{
	std::cout << "Going to grab skill handle" << std::endl;
    Skill * pSkill = GrabHandle<Skill*>(hSkill);
	std::cout << "Skill was okay, now for character" << std::endl;
    ICharacter * pChar = GrabHandle<ICharacter*>(hICharacter);
    
    pSkill->Invoke(pChar,ParameterList());
    
    return SteelType();
}

SteelType Application::equipScreen ( SteelType::Handle hCharacter )
{
    Character * pChar = GrabHandle<Character*>(hCharacter);
    
    mEquipState.Init(pChar);
     
    RunState(&mEquipState);
    
    return SteelType();
}

SteelType Application::shop ( const SteelArray& hItems )
{
    mShopState.Init ( hItems );
    
    RunState(&mShopState);
    
    return SteelType();
}

SteelType Application::sell ( ) 
{
    mShopState.Init();
    
    RunState(&mShopState);
    
    return SteelType();
}


bool Application::Serialize ( std::ostream& out )
{
    mpParty->Serialize(out);    
    mMapState.SerializeState(out);
    WriteString(out,GraphicsManager::GetThemeName());
    return true;
}

bool Application::Deserialize( std::istream& in ) 
{
    mpParty->Deserialize(in);    
    mMapState.DeserializeState(in);
    std::string theme_name = ReadString(in);
    GraphicsManager::SetTheme(theme_name);
    return true;
}

SteelType Application::save()
{
    // TODO: Put up SaveLoadState
    SaveLoadState state;
    state.Init(true);
    RunState(&state);
    return SteelType();
}

SteelType Application::load()
{
    SaveLoadState state;
    state.Init(false);
    RunState(&state);
	SteelType cancelled;
	cancelled.set(!state.Cancelled());
	return cancelled;
}

SteelType Application::statusScreen(bool party)
{
    StatusState state;
	if(!party){
		state.SetParty(&m_reserve_party);
	}else{
		state.SetParty(mpParty);
	}
    RunState(&state);
    return SteelType();
}

SteelType Application::getThemes() 
{
    SteelType val;
    SteelArray array;
    std::list<std::string> theme_list;
    GraphicsManager::GetAvailableThemes(theme_list);
    for(std::list<std::string>::const_iterator it = theme_list.begin();
        it != theme_list.end(); it++){
        SteelType entry;
        entry.set(*it);
        array.push_back(entry);
    }
    val.set(array);
    return val;
}

SteelType Application::setTheme(const std::string& theme_name)
{
    GraphicsManager::SetTheme(theme_name);
    return SteelType();
}

SteelType Application::omegaSlotCount()
{
    Party * pParty = GetParty();
    SteelType var;
    var.set(pParty->GetCommonAttribute(ICharacter::CA_IDOL_SLOTS));
    return var;    
}

SteelType Application::equipOmega(uint slot, const SteelType::Handle& omega){
    Party * pParty = GetParty();
    Omega * pOmega = GrabHandle<Omega*>(omega);
    pParty->EquipOmega(slot,pOmega);
    return SteelType();
}

SteelType Application::unequipOmega ( uint slot )
{
    Party * pParty = GetParty();
    pParty->UnequipOmega(slot);
    return SteelType();
}

SteelType Application::getOmega ( uint slot )
{
    Party * pParty = GetParty();
    SteelType var;
    var.set(pParty->GetOmega(slot));
    return var;
}

SteelType Application::omegaSlotIsEmpty( uint slot ) {
   Party * pParty = GetParty();
   SteelType var;
   var.set(pParty->GetOmega(slot) == NULL);
   return var;
}

SteelType Application::editing() {
    SteelType result;
#if SR2_EDITOR
    result.set(true);
#else
    result.set(false);
#endif
    return result;
}
SteelType Application::editMap() 
{
#if SR2_EDITOR
    RunState(&mMapEditorState);
#endif
    return SteelType();
}

SteelType Application::banner ( const std::string& str, double time )
{
    Banner(str,time * 1000);
    return SteelType();
}


void Application::Banner ( const std::string& str, int time )
{
    mBannerState.Init(str,time);
    RunState(&mBannerState);
}

SteelType Application::closeMap ()
{
	mMapState.BringDown();
#if 0 
	std::cout << "State count at closeMap: " << mStates.size() << std::endl;
	for(std::vector<State*>::const_iterator it= mStates.begin(); it != mStates.end(); it++){
		if((*it) == &mMapState){
			std::cout << "Map State" << std::endl;
		}else if((*it) == &mStartupState){
			std::cout << "Startup State" << std::endl;
		}else{
			std::cout << "Other state" << std::endl;
		}
	}
#endif
	return SteelType();
}

SteelType Application::gameoverScreen()
{
	GameoverState state;
	RunState(&state);
	return SteelType();
}

SteelType Application::configJoystick() 
{
	m_joystick_config.Reset();
	m_joystick_train_state = JS_TRAIN_AXIS;
	m_joystick_train_component.m_dir = IApplication::AXIS_UP;
	Banner("Press Up on Joystick",-1);
	m_joystick_train_component.m_dir = IApplication::AXIS_DOWN;
	Banner("Press Down on Joystick",-1);
	m_joystick_train_component.m_dir = IApplication::AXIS_RIGHT;
	Banner("Press Right on Joystick",-1);
	m_joystick_train_component.m_dir = IApplication::AXIS_LEFT;
	Banner("Press Left on Joystick",-1);
	
	m_joystick_train_state = JS_TRAIN_BUTTON;
	m_joystick_train_component.m_button = IApplication::BUTTON_CONFIRM;
	Banner("Press Confirm Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_CANCEL;
	Banner("Press Cancel Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_ALT;
	Banner("Press Prod Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_MENU;
	Banner("Press Menu Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_L;
	Banner("Press L Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_R;
	Banner("Press R Button on Joystick", -1);
	m_joystick_train_component.m_button = IApplication::BUTTON_START;
	Banner("Press Start Button on Joystick",-1);
	m_joystick_train_component.m_button = IApplication::BUTTON_SELECT;
	Banner("Press Select Button on Joystick",-1);
	
	m_joystick_train_state = JS_TRAIN_IDLE;
	m_joystick_config.FinishedSetup();
	
	std::ofstream out("joystick.set");
	m_joystick_config.Write(out);
	out.close();
	return SteelType();
}


void Application::LoadMainMenu ( clan::DomDocument& doc )
{
    IFactory * pFactory = IApplication::GetInstance()->GetElementFactory();


    clan::DomElement menuElement = doc.get_first_child().to_element();
    clan::DomElement menuoptionNode = menuElement.get_first_child().to_element();

    while ( !menuoptionNode.is_null() )
    {
        MenuOption * menuOption = dynamic_cast<MenuOption*> ( pFactory->createElement ( menuoptionNode.get_node_name() ) );
        menuOption->Load ( menuoptionNode );
        mMainMenuState.AddOption ( menuOption );

        menuoptionNode = menuoptionNode.get_next_sibling().to_element();
    }
}

State* Application::getInputState() const
{
	for(int s = mStates.size()-1; s>=0;s--){
		if(mStates[s]->AcceptInput())
			return mStates[s];
	}
	assert(0);
	return NULL;
}

IApplication * IApplication::GetInstance()
{
    return pApp;
}


Party * Application::GetParty() const
{
    return mpParty;
}



AbilityManager * Application::GetAbilityManager()
{
    return &mAbilityManager;
}


clan::ResourceManager& Application::GetResources()
{
    return m_resources;
}


Application::Application() : mpParty ( 0 ),
        mbDone ( false ),m_draws_total(0)

{
    mpParty = new Party();
	m_threaded_mode = false;
}

Application::~Application()
{

}



clan::Rect Application::GetDisplayRect() const
{
    return clan::Rect ( 0, 0, GetScreenWidth(), GetScreenHeight() );

}

void Application::setupClanLib()
{



}

void Application::teardownClanLib()
{
}



double Application::get_value_for_axis_direction ( IApplication::AxisDirection dir ) const
{
	if(!m_joystick_config.IsSetup())
		return 0.0;
	return m_joystick_config.GetValueFor(dir);
}

IApplication::AxisDirection Application::get_direction_for_value ( IApplication::Axis axis, double value ) const
{
    if ( value == 0.0 ) return AXIS_NEUTRAL;

    if ( axis == IApplication::AXIS_HORIZONTAL )
    {
        if ( get_value_for_axis_direction ( AXIS_LEFT ) > 0 )
        {
            if ( value > 0 ) return AXIS_LEFT;
            else return AXIS_RIGHT;
        }
        else
        {
            if ( value > 0 ) return AXIS_RIGHT;
            else return AXIS_LEFT;
        }
    }
    else if ( axis == IApplication::AXIS_VERTICAL )
    {
        if ( get_value_for_axis_direction ( AXIS_UP ) > 0 )
        {
            if ( value > 0 ) return AXIS_UP;
            else return AXIS_DOWN;
        }
        else
        {
            if ( value > 0 ) return AXIS_DOWN;
            else return AXIS_UP;
        }
    }

    return AXIS_NEUTRAL;
}


void Application::onSignalKeyDown ( const clan::InputEvent &key )
{

    // Handle raw key press
    getInputState()->HandleKeyDown ( key );

    // Do mappings now
    switch ( key.id )
    {
        case clan::keycode_down:
            getInputState()->HandleAxisMove ( IApplication::AXIS_VERTICAL, AXIS_DOWN, get_value_for_axis_direction ( AXIS_DOWN ) );
            break;
        case clan::keycode_up:
            getInputState()->HandleAxisMove ( IApplication::AXIS_VERTICAL, AXIS_UP, get_value_for_axis_direction ( AXIS_UP ) );
            break;
        case clan::keycode_left:
            getInputState()->HandleAxisMove ( IApplication::AXIS_HORIZONTAL, AXIS_LEFT, get_value_for_axis_direction ( AXIS_LEFT ) );
            break;
        case clan::keycode_right:
            getInputState()->HandleAxisMove ( IApplication::AXIS_HORIZONTAL, AXIS_RIGHT, get_value_for_axis_direction ( AXIS_RIGHT ) );
            break;
        case clan::keycode_space:
        case clan::keycode_t:
            getInputState()->HandleButtonDown ( BUTTON_CONFIRM );
            break;
        case clan::keycode_tab:
            getInputState()->HandleButtonDown ( BUTTON_ALT );
            break;
        case clan::keycode_escape:
            getInputState()->HandleButtonDown ( BUTTON_CANCEL );
            break;
        case clan::keycode_enter:
            getInputState()->HandleButtonDown ( BUTTON_START );
            break;
        case clan::keycode_home:
            getInputState()->HandleButtonDown ( BUTTON_MENU );
            break;
        case clan::keycode_m:
            getInputState()->HandleButtonDown ( BUTTON_R );
            break;
        case clan::keycode_n:
            getInputState()->HandleButtonDown ( BUTTON_L );
            break;
    }


}

void Application::onSignalKeyUp ( const clan::InputEvent &key )
{
    getInputState()->HandleKeyUp ( key );

    // Do mappings now

    switch ( key.id )
    {
        case clan::keycode_down:
            getInputState()->HandleAxisMove ( IApplication::AXIS_VERTICAL, AXIS_NEUTRAL, 0.0 );
            break;
        case clan::keycode_up:
            getInputState()->HandleAxisMove ( IApplication::AXIS_VERTICAL, AXIS_NEUTRAL, 0.0 );
            break;
        case clan::keycode_left:
        case clan::keycode_right:
            getInputState()->HandleAxisMove ( IApplication::AXIS_HORIZONTAL, AXIS_NEUTRAL, 0.0 );
            break;
        case clan::keycode_space:
        case clan::keycode_t:
            getInputState()->HandleButtonUp ( BUTTON_CONFIRM );
            break;
        case clan::keycode_tab:
            getInputState()->HandleButtonUp ( BUTTON_ALT );
            break;
        case clan::keycode_escape:
            getInputState()->HandleButtonUp ( BUTTON_CANCEL );
            break;
        case clan::keycode_enter:
            getInputState()->HandleButtonUp ( BUTTON_START );
            break;
        case clan::keycode_home:
            getInputState()->HandleButtonUp ( BUTTON_MENU );
            break;
        case clan::keycode_m:
            getInputState()->HandleButtonUp ( BUTTON_R );
            break;
        case clan::keycode_n:
            getInputState()->HandleButtonUp ( BUTTON_L );
            break;
        case clan::keycode_s:
            getInputState()->HandleButtonUp ( BUTTON_SELECT );
            break;
    }

}

void Application::onSignalJoystickButtonDown ( const clan::InputEvent &event  )
{

    if ( !mStates.size() ) return;
	
	if(!m_joystick_config.IsSetup()){
		// Possibly set it up now

	}else{
		IApplication::Button button = m_joystick_config.GetButtonForId(event.id);
		if(button != IApplication::BUTTON_INVALID){
			m_button_down[button] = true;
			getInputState()->HandleButtonDown(button);			
		}		
	}

}

void Application::onSignalJoystickButtonUp ( const clan::InputEvent &event )
{
    if ( !mStates.size() ) return;

	if(!m_joystick_config.IsSetup()){
		if(m_joystick_train_state == JS_TRAIN_BUTTON){
			m_joystick_config.MapButton(m_joystick_train_component.m_button,event.id);
			mBannerState.BringDown();
		}		
	}else{
		IApplication::Button button = m_joystick_config.GetButtonForId(event.id);
		if(button != IApplication::BUTTON_INVALID && m_button_down[button]){
			m_button_down[button] = false;
			getInputState()->HandleButtonUp(button);			
		}
	}

}

void Application::onSignalJoystickAxisMove ( const clan::InputEvent &event )
{
	if(!m_joystick_config.IsSetup()){
		if(abs(event.axis_pos) > 0.5f){
			if(m_joystick_train_state == JS_TRAIN_AXIS){
				switch(m_joystick_train_component.m_dir){
					case IApplication::AXIS_UP:
					case IApplication::AXIS_DOWN:
						m_joystick_config.MapAxis(IApplication::AXIS_VERTICAL, event.id);
						break;
					case IApplication::AXIS_LEFT:
					case IApplication::AXIS_RIGHT:
						m_joystick_config.MapAxis(IApplication::AXIS_HORIZONTAL,event.id);
						break;
				}
				m_joystick_config.MapAxisValue(m_joystick_train_component.m_dir,event.axis_pos);		
				mBannerState.BringDown();
			}
		}
	}else{
		if ( event.id == m_joystick_config.GetAxis(AXIS_HORIZONTAL) )
		{
			getInputState()->HandleAxisMove ( AXIS_HORIZONTAL, get_direction_for_value ( AXIS_HORIZONTAL, event.axis_pos ), event.axis_pos );
		}
		else if ( event.id == m_joystick_config.GetAxis(AXIS_VERTICAL) )
		{
			getInputState()->HandleAxisMove ( AXIS_VERTICAL, get_direction_for_value ( AXIS_VERTICAL, event.axis_pos ), event.axis_pos );
		}
	}
}

bool Application::IsCutsceneRunning() const {
	return m_threaded_mode; // TODO: SOmething better. 
	// Also, see notes in CutSceneState about maybe a better solution?
}


static IApplication::MouseButton CLMouseToMouseButton(int id){
    switch(id){
        case clan::mouse_left:
            return IApplication::MOUSE_LEFT;
        case clan::mouse_right:
            return IApplication::MOUSE_RIGHT;
        case clan::mouse_middle:
            return IApplication::MOUSE_MIDDLE;
        default:
            return IApplication::MOUSE_UNKNOWN;
    }
}

static uint CLKeyStatesToKeyState(bool shift, bool alt, bool ctrl){
        uint result = 0;
        if(shift) result |= IApplication::KEY_SHIFT;
        if(alt) result |= IApplication::KEY_ALT;
        if(ctrl) result |= IApplication::KEY_CTRL;
        return result;
}

void Application::onSignalMouseDown ( const clan::InputEvent& event ){
    if(!mStates.empty()){
        getInputState()->HandleMouseDown(CLMouseToMouseButton(event.id),event.mouse_pos,CLKeyStatesToKeyState(event.shift,event.alt,event.ctrl));
    }
}

void Application::onSignalMouseUp ( const clan::InputEvent& event ){
    if(!mStates.empty()){
        getInputState()->HandleMouseUp(CLMouseToMouseButton(event.id),event.mouse_pos,CLKeyStatesToKeyState(event.shift,event.alt,event.ctrl));
    }    
}

void Application::onSignalDoubleClick ( const clan::InputEvent& event ){
    if(!mStates.empty()){
        getInputState()->HandleDoubleClick(CLMouseToMouseButton(event.id),event.mouse_pos,CLKeyStatesToKeyState(event.shift,event.alt,event.ctrl));
    }    
}

void Application::onSignalMouseMove ( const clan::InputEvent& event ){
    if(!mStates.empty()){
        getInputState()->HandleMouseMove(event.mouse_pos,CLKeyStatesToKeyState(event.shift,event.alt,event.ctrl));
    }
}


void Application::onSignalLostFocus()
{
	if(!mStates.empty()){
		mStates.back()->Covered();
	}
}

void Application::onSignalQuit()
{
    DynamicMenuState menu;
    std::vector<std::string> options;
    options.push_back("Cancel");
    options.push_back("Quit");
    menu.Init(options);
    RunState(&menu);
    if(menu.GetSelection() == 1){
        mbDone = true;
    }
}



Steel::AstScript * Application::LoadScript ( const std::string &name, const std::string &script )
{
    return mInterpreter->prebuildAst ( name, script );
}

SteelType Application::RunScript ( AstScript * pScript )
{
    // Intentionally letting steel exceptions
    // Get caught by a higher layer
    return mInterpreter->runAst ( pScript );

}

SteelType Application::RunScript ( AstScript *pScript, const ParameterList &params )
{
    return mInterpreter->runAst ( pScript, params );
}

#ifdef SR2_EDITOR
void Application::EditMaps()
{
	editMap();
}
#endif

SteelType Application::debug(){
	SteelType ret;
#ifdef NDEBUG
	ret.set(false);
#else
	ret.set(true);
#endif
	return ret;
}


#ifndef NDEBUG
SteelType	Application::enableInfiniteSP(bool e){
	DebugControl::EnableInfiniteSP(e);
	return SteelType();
}
SteelType	Application::enableInfiniteBP(bool e){
	DebugControl::EnableInfiniteBP(e);
	return SteelType();
}
SteelType	Application::enableInfiniteGold(bool e){
	DebugControl::EnableInfiniteGold(e);
	return SteelType();
}
SteelType	Application::enableAllSkills(bool e){
	DebugControl::EnableAllSkills(e);
	return SteelType();
}
#endif
		


void Application::registerSteelFunctions()
{
    mInterpreter->pushScope();
    SteelFunctor* fn_say = new SteelFunctor2Arg<Application, const std::string&, const std::string&> ( this, &Application::say );
    SteelFunctor* fn_playScene = new SteelFunctor1Arg<Application, const SteelType&> ( this, &Application::playScene );
    SteelFunctor* fn_playSound  = new SteelFunctor1Arg<Application, const std::string&> ( this, &Application::playSound );
    SteelFunctor* fn_loadLevel = new SteelFunctor3Arg<Application, const std::string&, uint, uint> ( this, &Application::loadLevel );
	SteelFunctor* fn_pushLevel = new SteelFunctor3Arg<Application, const std::string&, uint, uint> ( this, &Application::pushLevel );
    	
    SteelFunctor* fn_startBattle = new SteelFunctor4Arg<Application, const std::string &, uint, bool, const std::string&> ( this, &Application::startBattle );
    SteelFunctor* fn_pause = new  SteelFunctor1Arg<Application, uint> ( this, &Application::pause );
    SteelFunctor* fn_choice = new SteelFunctor2Arg<Application, const std::string&, const SteelType::Container &> ( this, &Application::choice );
    SteelFunctor* fn_pop = new SteelFunctor1Arg<Application, bool> ( this, &Application::popLevel );
   // SteelFunctor* fn_giveItem = new SteelFunctor2Arg<Application, const std::string &, uint> ( this, &Application::giveItem );
    
    SteelFunctor*  fn_takeItem = new SteelFunctor3Arg<Application, const SteelType::Handle, uint, bool> ( this, &Application::takeItem );
    SteelFunctor*  fn_getGold = new SteelFunctorNoArgs<Application> ( this, &Application::getGold );
    SteelFunctor*  fn_hasItem = new SteelFunctor2Arg<Application, const std::string &, uint> ( this, &Application::hasItem );
    SteelFunctor*  fn_useItem = new SteelFunctor2Arg<Application, bool, bool> ( this, &Application::selectItem );
    SteelFunctor*  fn_didEvent = new SteelFunctor1Arg<Application, const std::string &> ( this, &Application::didEvent );
    SteelFunctor* fn_doEvent = new  SteelFunctor2Arg<Application, const std::string &, bool> ( this, &Application::doEvent );
    SteelFunctor*  fn_giveGold = new SteelFunctor1Arg<Application, int> ( this, &Application::giveGold );
    SteelFunctor*  fn_addCharacter = new SteelFunctor3Arg<Application, const std::string &, int, bool> ( this, &Application::addCharacter );

    SteelFunctor*  fn_getPartyArray = new SteelFunctorNoArgs<Application> ( this, &Application::getPartyArray );
    SteelFunctor*  fn_getItemName = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getItemName );
    SteelFunctor* fn_getWeaponAttribute = new  SteelFunctor2Arg<Application, const SteelType::Handle, uint> ( this, &Application::getWeaponAttribute );
    SteelFunctor*  fn_getArmorAttribute = new SteelFunctor2Arg<Application, const SteelType::Handle, uint> ( this, &Application::getArmorAttribute );
    
	mInterpreter->addFunction("normal_random",new SteelFunctor2Arg<Application, double, double> ( this, &Application::gaussian ));
    SteelFunctor*  fn_getCharacterName = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getCharacterName );
    SteelFunctor*  fn_getCharacterAttribute = new SteelFunctor2Arg<Application, const SteelType::Handle, uint> ( this, &Application::getCharacterAttribute );
    SteelFunctor*  fn_addExperience = new SteelFunctor2Arg<Application, const SteelType::Handle, int> ( this, &Application::addExperience );
    SteelFunctor*  fn_getExperience = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getExperience );
    SteelFunctor*  fn_getCharacterLevel = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getCharacterLevel );
    SteelFunctor*  fn_getCharacterToggle = new SteelFunctor2Arg<Application, const SteelType::Handle, uint> ( this, &Application::getCharacterToggle );
    SteelFunctor*  fn_setCharacterToggle = new SteelFunctor3Arg<Application, const SteelType::Handle, uint, bool> ( this, &Application::setCharacterToggle );
    SteelFunctor*  fn_getEquippedWeaponAttribute = new SteelFunctor3Arg<Application, const SteelType::Handle, uint, int> ( this, &Application::getEquippedWeaponAttribute );
    SteelFunctor*  fn_getEquippedArmorAttribute = new SteelFunctor2Arg<Application, const SteelType::Handle, uint> ( this, &Application::getEquippedArmorAttribute );
    SteelFunctor*  fn_addStatusEffect = new SteelFunctor2Arg<Application, const SteelType::Handle, const SteelType::Handle> ( this, &Application::addStatusEffect );
    SteelFunctor*  fn_removeStatusEffect = new SteelFunctor2Arg<Application, const SteelType::Handle, const SteelType::Handle> ( this, &Application::removeStatusEffect );
    SteelFunctor*  fn_doDamage = new SteelFunctor2Arg<Application, const SteelType::Handle, int> ( this, &Application::doDamage );

    SteelFunctor*  fn_hasEquipment = new SteelFunctor2Arg<Application, const SteelType::Handle, int> ( this, &Application::hasEquipment );
    SteelFunctor*  fn_getEquipment = new SteelFunctor2Arg<Application, const SteelType::Handle, int> ( this, &Application::getEquipment );
    SteelFunctor*  fn_equip = new SteelFunctor3Arg<Application, const SteelType::Handle, int, const std::string&> ( this, &Application::equip );


    SteelFunctor*  fn_getWeaponType = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getWeaponType );
    SteelFunctor*  fn_getArmorType = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getArmorType );
	
    SteelFunctor*  fn_getWeaponTypeDamageCategory = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getWeaponTypeDamageCategory );
    SteelFunctor*  fn_getWeaponAnimation = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getWeaponAnimation );
    SteelFunctor*  fn_weaponHasAnimation = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::weaponHasAnimation );
    SteelFunctor*  fn_invokeWeapon = new SteelFunctor5Arg<Application, const SteelType::Handle, const SteelType::Handle, const SteelType::Handle, uint, uint> ( this, &Application::invokeWeapon );
    SteelFunctor*  fn_invokeArmor = new SteelFunctor2Arg<Application, const SteelType::Handle, const SteelType::Handle> ( this, &Application::invokeArmor );
    SteelFunctor*  fn_attackCharacter = new SteelFunctor5Arg<Application, const SteelType::Handle, const SteelType::Handle,uint,bool,int> ( this, &Application::attackCharacter );

    SteelFunctor*  fn_runWeaponAnimationScript = new SteelFunctor3Arg<Application, const SteelType::Handle, const SteelType::Handle, const SteelType::Handle> ( this, &Application::runWeaponAnimationScript );
    SteelFunctor*  fn_weaponHasAnimationScript = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::weaponHasAnimationScript );

	
    SteelFunctor*  fn_getDamageCategoryResistance = new SteelFunctor2Arg<Application, const SteelType::Handle, int> ( this, &Application::getDamageCategoryResistance );
    SteelFunctor*  fn_getUnarmedHitSound = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getUnarmedHitSound );
    SteelFunctor*  fn_getUnarmedMissSound = new SteelFunctor1Arg<Application, const SteelType::Handle> ( this, &Application::getUnarmedMissSound );

    SteelFunctor*  fn_getAnimation = new SteelFunctor1Arg<Application, const std::string&> ( this, &Application::getAnimation );
    SteelFunctor*  fn_log = new SteelFunctor1Arg<Application, const std::string&> ( this, &Application::log );
    SteelFunctor*  fn_showExperience = new SteelFunctor4Arg<Application, const SteelArray&, const SteelArray&, const SteelArray&, const SteelArray&> 
                                            ( this, &Application::showExperience );
    SteelFunctor*  fn_selectItemAdv = new SteelFunctor1Arg<Application,uint> ( this, &Application::selectItemAdv );
	
	SteelFunctor*  fn_closeMap = new SteelFunctorNoArgs<Application>(this,&Application::closeMap);
	SteelFunctor*  fn_gameoverScreen = new SteelFunctorNoArgs<Application>(this,&Application::gameoverScreen);




    steelConst ( "$_ITEM_REGULAR", Item::REGULAR_ITEM );
    steelConst ( "$_ITEM_SPECIAL", Item::SPECIAL );
    steelConst ( "$_ITEM_WEAPON", Item::WEAPON );
    steelConst ( "$_ITEM_ARMOR", Item::ARMOR );
    steelConst ( "$_ITEM_RUNE", Item::RUNE );
    steelConst ( "$_ITEM_SYSTEM", Item::SYSTEM );
    steelConst ( "$_ITEM_OMEGA", Item::OMEGA );

    steelConst ( "$_ITEM_TRG_ALL", RegularItem::ALL );
    steelConst ( "$_ITEM_TRG_GROUP", RegularItem::GROUP );
    steelConst ( "$_ITEM_TRG_SINGLE", RegularItem::SINGLE );
    steelConst ( "$_ITEM_TRG_SELF_ONLY", RegularItem::SELF_ONLY );
    steelConst ( "$_ITEM_TRG_NONE", RegularItem::NO_TARGET );

    steelConst ( "$_ITEM_DEF_PARTY", RegularItem::PARTY );
    steelConst ( "$_ITEM_DEF_MONSTERS", RegularItem::MONSTERS );
	
	steelConst ( "$_WEAPON_ATTACK", Weapon::ATTACK );
	steelConst ( "$_WEAPON_HIT", Weapon::HIT );
	steelConst ( "$_WEAPON_CRITICAL", Weapon::CRITICAL );

    steelConst ( "$_HAND", Equipment::EHAND );
    steelConst ( "$_OFFHAND", Equipment::EOFFHAND );
    steelConst ( "$_HEAD", Equipment::EHEAD );
    steelConst ( "$_HANDS", Equipment::EHANDS );
    steelConst ( "$_BODY", Equipment::EBODY );
    steelConst ( "$_FINGER1", Equipment::EFINGER1 );
    steelConst ( "$_FINGER2", Equipment::EFINGER2 );
    steelConst ( "$_FEET", Equipment::EFEET );

    steelConst ( "$_PRE_ATTACK", Weapon::ATTACK_AFTER );
    steelConst ( "$_POST_ATTACK", Weapon::ATTACK_BEFORE );
    steelConst ( "$_FORGO_ATTACK", Weapon::FORGO_ATTACK );

    steelConst ( "$_BASH_DEF", Character::CA_BASH_DEF );
    steelConst ( "$_PIERCE_DEF", Character::CA_PIERCE_DEF );
    steelConst ( "$_SLASH_DEF", Character::CA_SLASH_DEF );
    steelConst ( "$_HOLY_RST", Character::CA_HOLY_RST );
    steelConst ( "$_DARK_RST", Character::CA_DARK_RST );
    steelConst ( "$_FIRE_RST", Character::CA_FIRE_RST );
    steelConst ( "$_WATER_RST", Character::CA_WATER_RST );
    steelConst ( "$_WIND_RST", Character::CA_WIND_RST );
    steelConst ( "$_EARTH_RST", Character::CA_EARTH_RST );

    steelConst ( "$_HP", Character::CA_HP );
    steelConst ( "$_MP", Character::CA_MP );
    steelConst ( "$_BP", Character::CA_BP );
    steelConst ( "$_STR", Character::CA_STR );
    steelConst ( "$_DEF", Character::CA_DEF );
    steelConst ( "$_DEX", Character::CA_DEX );
    steelConst ( "$_EVD", Character::CA_EVD );
    steelConst ( "$_MAG", Character::CA_MAG );
    steelConst ( "$_RST", Character::CA_RST );
    steelConst ( "$_LCK", Character::CA_LCK );
    steelConst ( "$_JOY", Character::CA_JOY );
	
	steelConst ( "$_MP_COST", Character::CA_MP_COST );
	steelConst ( "$_BP_COST", Character::CA_BP_COST );

    steelConst ( "$_HIT", Weapon::HIT );
    steelConst ( "$_ATTACK", Weapon::ATTACK );
    steelConst ( "$_CRITICAL", Weapon::CRITICAL );

    steelConst ( "$_AC", Armor::AC );
    //steelConst ( "$_ARMOR_RST", Armor::RST );

    steelConst ( "$_DRAW_ILL", Character::CA_DRAW_ILL );
    steelConst ( "$_DRAW_STONE", Character::CA_DRAW_STONE );
    steelConst ( "$_DRAW_BERSERK", Character::CA_DRAW_BERSERK );
    steelConst ( "$_DRAW_WEAK", Character::CA_DRAW_WEAK );
    steelConst ( "$_DRAW_PARALYZED", Character::CA_DRAW_PARALYZED );
    steelConst ( "$_DRAW_TRANSLUCENT", Character::CA_DRAW_TRANSLUCENT );
    steelConst ( "$_DRAW_MINI", Character::CA_DRAW_MINI );
    steelConst ( "$_CAN_ACT", Character::CA_CAN_ACT );
    steelConst ( "$_CAN_FIGHT", Character::CA_CAN_FIGHT );
    steelConst ( "$_CAN_CAST", Character::CA_CAN_CAST );
    steelConst ( "$_CAN_SKILL", Character::CA_CAN_SKILL );
    steelConst ( "$_CAN_ITEM", Character::CA_CAN_ITEM );
    steelConst ( "$_CAN_RUN", Character::CA_CAN_RUN );
    steelConst ( "$_ALIVE", Character::CA_ALIVE );
    steelConst ( "$_VISIBLE", Character::CA_VISIBLE );

    steelConst ( "$_MAXHP", Character::CA_MAXHP );
    steelConst ( "$_MAXMP", Character::CA_MAXMP );
    steelConst ( "$_MAXBP", Character::CA_MAXBP );

    steelConst ( "$_BASH", DamageCategory::BASH );
    steelConst ( "$_JAB", DamageCategory::PIERCE );
    steelConst ( "$_SLASH", DamageCategory::SLASH );
    steelConst ( "$_HOLY", DamageCategory::HOLY );
    steelConst ( "$_DARK", DamageCategory::DARK );
    steelConst ( "$_FIRE", DamageCategory::FIRE );
    steelConst ( "$_WATER", DamageCategory::WATER );
    steelConst ( "$_WIND", DamageCategory::WIND );
    steelConst ( "$_EARTH", DamageCategory::EARTH );
    steelConst ( "$_GRAVITY", DamageCategory::GRAVITY );
    steelConst ( "$_ELECTRIC", DamageCategory::ELECTRIC );
    
    steelConst ( "$_DROP_COMMON", Item::COMMON );
    steelConst ( "$_DROP_UNCOMMON", Item::UNCOMMON );
    steelConst ( "$_DROP_RARE", Item::RARE );
    steelConst ( "$_DROP_NEVER", Item::NEVER );
    
    
    steelConst ( "$_NORTH", Direction::NORTH );
    steelConst ( "$_SOUTH", Direction::SOUTH );
    steelConst ( "$_WEST",  Direction::WEST );
    steelConst ( "$_EAST",  Direction::EAST );

    //mInterpreter->addFunction ( "normal_random", fn_gaussian );
    mInterpreter->addFunction ( "log", fn_log );

    mInterpreter->addFunction ( "say", fn_say );
    mInterpreter->addFunction ( "playScene", fn_playScene );
    mInterpreter->addFunction ( "playSound", fn_playSound );
    mInterpreter->addFunction ( "loadLevel", fn_loadLevel );
    mInterpreter->addFunction ( "startBattle", fn_startBattle );
    mInterpreter->addFunction ( "pause", fn_pause );
    mInterpreter->addFunction ( "choice", fn_choice );
	mInterpreter->addFunction ( "pushLevel", fn_pushLevel );
    mInterpreter->addFunction ( "popLevel", fn_pop );
    mInterpreter->addFunction ( "takeItem", fn_takeItem );
    mInterpreter->addFunction ( "getGold", fn_getGold );
    mInterpreter->addFunction ( "selectItem", fn_useItem );
    mInterpreter->addFunction ( "hasItem", fn_hasItem );
    mInterpreter->addFunction ( "getItemName", fn_getItemName );
    mInterpreter->addFunction ( "didEvent", fn_didEvent );
    mInterpreter->addFunction ( "doEvent", fn_doEvent );
    mInterpreter->addFunction ( "giveGold", fn_giveGold );
    mInterpreter->addFunction ( "addCharacter", fn_addCharacter );

    mInterpreter->addFunction ( "getPartyArray", fn_getPartyArray );
    mInterpreter->addFunction ( "attackCharacter", fn_attackCharacter );
    mInterpreter->addFunction ( "getWeaponAttribute", fn_getWeaponAttribute );
    mInterpreter->addFunction ( "getArmorAttribute", fn_getArmorAttribute );

    mInterpreter->addFunction ( "getCharacterAttribute", fn_getCharacterAttribute );
    mInterpreter->addFunction ( "getCharacterLevel", fn_getCharacterLevel );
    mInterpreter->addFunction ( "getExperience", fn_getExperience );
    mInterpreter->addFunction ( "addExperience", fn_addExperience );
    mInterpreter->addFunction ( "getCharacterToggle", fn_getCharacterToggle );
    mInterpreter->addFunction ( "setCharacterToggle", fn_setCharacterToggle );
    mInterpreter->addFunction ( "getCharacterName", fn_getCharacterName );
    mInterpreter->addFunction ( "getEquippedWeaponAttribute", fn_getEquippedWeaponAttribute );
    mInterpreter->addFunction ( "getEquippedArmorAttribute", fn_getEquippedArmorAttribute );
    mInterpreter->addFunction ( "addStatusEffect", fn_addStatusEffect );
    mInterpreter->addFunction ( "removeStatusEffect", fn_removeStatusEffect );
    mInterpreter->addFunction ( "doDamage", fn_doDamage );
    mInterpreter->addFunction ( "hasEquipment", fn_hasEquipment );
    mInterpreter->addFunction ( "getEquipment", fn_getEquipment );
    mInterpreter->addFunction ( "equip", fn_equip );


    mInterpreter->addFunction ( "getWeaponType", fn_getWeaponType );
    mInterpreter->addFunction ( "getArmorType", fn_getArmorType );

    mInterpreter->addFunction ( "getWeaponTypeDamageCategory", fn_getWeaponTypeDamageCategory );
    mInterpreter->addFunction ( "getWeaponAnimation", fn_getWeaponAnimation );
    mInterpreter->addFunction ( "weaponHasAnimation", fn_weaponHasAnimation );
    mInterpreter->addFunction ( "getDamageCategoryResistance", fn_getDamageCategoryResistance );
    mInterpreter->addFunction ( "invokeArmor", fn_invokeArmor );
    mInterpreter->addFunction ( "invokeWeapon", fn_invokeWeapon );
    //mInterpreter->addFunction ( "getHitSound", fn_getHitSound );
    //mInterpreter->addFunction ( "getMissSound", fn_getMissSound );
    mInterpreter->addFunction ( "getUnarmedHitSound", fn_getUnarmedHitSound );
    mInterpreter->addFunction ( "getUnarmedMissSound", fn_getUnarmedMissSound );

    mInterpreter->addFunction ( "getAnimation", fn_getAnimation );
    mInterpreter->addFunction ( "showExperience", fn_showExperience );

	mInterpreter->addFunction ( "runWeaponAnimationScript", fn_runWeaponAnimationScript );
	mInterpreter->addFunction ( "weaponHasAnimationScript", fn_weaponHasAnimationScript );
    mInterpreter->addFunction ( "mainMenu", new SteelFunctorNoArgs<Application> ( this, &Application::mainMenu ) );

    mInterpreter->addFunction ( "getItemType", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::getItemType ) );
    mInterpreter->addFunction ( "getItemValue", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::getItemValue ) );
    mInterpreter->addFunction ( "getItemSellValue", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::getItemSellValue ) );
    mInterpreter->addFunction ( "isBattleItem", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::isBattleItem ) );
    mInterpreter->addFunction ( "isWorldItem", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::isWorldItem ) );
    mInterpreter->addFunction ( "isReusableItem", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::isReusableItem ) );
    mInterpreter->addFunction ( "getItemTargetable", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::getItemTargetable ) );
    mInterpreter->addFunction ( "getItemDefaultTarget", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::getItemDefaultTarget ) );
    mInterpreter->addFunction ( "useItem", new SteelFunctor2Arg<Application, SteelType::Handle, const SteelType&> ( this, &Application::useItem ) );
    mInterpreter->addFunction ( "disposeItem", new SteelFunctor2Arg<Application, SteelType::Handle, uint> ( this, &Application::disposeItem ) );
    mInterpreter->addFunction ( "inBattle", new SteelFunctorNoArgs<Application> ( this, &Application::inBattle ) );

    mInterpreter->addFunction ( "doMPDamage", new SteelFunctor2Arg<Application, SteelType::Handle, int> ( this, &Application::doMPDamage ) );
    mInterpreter->addFunction ( "menu", new SteelFunctor1Arg<Application, const SteelArray&> ( this, &Application::menu ) );
    mInterpreter->addFunction ( "message", new SteelFunctor1Arg<Application, const std::string&> ( this, &Application::message ) );
    mInterpreter->addFunction ( "forgoAttack", new SteelFunctor1Arg<Application, SteelType::Handle> ( this, &Application::forgoAttack ) );

    mInterpreter->addFunction ( "getStatusEffect", new SteelFunctor1Arg<Application,const std::string&> (this, &Application::getStatusEffect));
    mInterpreter->addFunction ( "statusEffectChance", new SteelFunctor2Arg<Application,SteelType::Handle,SteelType::Handle>( this, &Application::statusEffectChance ) );;
//        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &webtype);
//       SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);
    mInterpreter->addFunction ( "kill", new SteelFunctor1Arg<Application,SteelType::Handle>( this, &Application::kill ));
    mInterpreter->addFunction ( "raise", new SteelFunctor1Arg<Application,SteelType::Handle>( this, &Application::raise ));

    mInterpreter->addFunction ( "skilltree", new SteelFunctor2Arg<Application,SteelType::Handle,bool>( this, &Application::skilltree ));
    
    mInterpreter->addFunction ( "getCharacterSP", new SteelFunctor1Arg<Application,SteelType::Handle>( this, &Application::getCharacterSP) );
    mInterpreter->addFunction ( "setCharacterSP", new SteelFunctor2Arg<Application,SteelType::Handle,int>( this, &Application::setCharacterSP) );
    mInterpreter->addFunction ( "doSkill", new SteelFunctor2Arg<Application,SteelType::Handle,SteelType::Handle>( this, &Application::doSkill) );
    mInterpreter->addFunction ( "getSkill", new SteelFunctor1Arg<Application,const std::string&>(this,&Application::getSkill) );
    mInterpreter->addFunction ( "learnSkill", new SteelFunctor3Arg<Application,SteelType::Handle,SteelType::Handle,bool>(this,&Application::learnSkill) );
    mInterpreter->addFunction ( "hasSkill", new SteelFunctor2Arg<Application,SteelType::Handle,const std::string&>(this,&Application::hasSkill) );

    mInterpreter->addFunction ( "augmentCharacterAttribute", new SteelFunctor3Arg<Application,SteelType::Handle,uint,double>(this,&Application::augmentCharacterAttribute) );
    mInterpreter->addFunction ( "generateRandomWeapon", new SteelFunctor3Arg<Application,uint,int,int>(this,&Application::generateRandomWeapon));
    mInterpreter->addFunction ( "generateRandomArmor", new SteelFunctor3Arg<Application,uint,int,int>(this,&Application::generateRandomArmor));
    mInterpreter->addFunction ( "giveItems", new SteelFunctor2Arg<Application,const Steel::SteelArray&,bool>(this,&Application::giveItems) );
    mInterpreter->addFunction ( "doEquipmentStatusEffectInflictions", new SteelFunctor2Arg<Application,SteelType::Handle,SteelType::Handle>(this,&Application::doEquipmentStatusEffectInflictions) );
    mInterpreter->addFunction ( "isArmor", new SteelFunctor1Arg<Application,SteelType::Handle>(this,&Application::isArmor) );
    mInterpreter->addFunction ( "weaponTypeIsRanged", new SteelFunctor1Arg<Application,SteelType::Handle>(this,&Application::weaponTypeIsRanged) );
    mInterpreter->addFunction ( "getNamedItem", new SteelFunctor1Arg<Application,const std::string&>(this,&Application::getNamedItem) );
    mInterpreter->addFunction ( "equipScreen", new SteelFunctor1Arg<Application,SteelType::Handle>(this,&Application::equipScreen) );
    mInterpreter->addFunction ( "randomItem", new SteelFunctor3Arg<Application,uint,int,int>(this,&Application::randomItem) );
    mInterpreter->addFunction ( "getMonsterDrops", new SteelFunctor1Arg<Application,const SteelType::Handle>(this,&Application::getMonsterDrops) );
    mInterpreter->addFunction ( "shop", new SteelFunctor1Arg<Application,const SteelArray&>(this,&Application::shop) );
    mInterpreter->addFunction ( "sell", new SteelFunctorNoArgs<Application>(this,&Application::sell) );
    mInterpreter->addFunction ( "save", new SteelFunctorNoArgs<Application>(this,&Application::save) );
    mInterpreter->addFunction ( "load", new SteelFunctorNoArgs<Application>(this,&Application::load) );
    mInterpreter->addFunction ( "statusScreen", new SteelFunctor1Arg<Application,bool>(this,&Application::statusScreen) );
    mInterpreter->addFunction ( "getThemes", new SteelFunctorNoArgs<Application>(this,&Application::getThemes) );
    mInterpreter->addFunction ( "setTheme", new SteelFunctor1Arg<Application,const std::string&>(this,&Application::setTheme ) );
    mInterpreter->addFunction ( "selectItemAdv", new SteelFunctor1Arg<Application,uint>(this,&Application::selectItemAdv) );
    mInterpreter->addFunction ( "equipOmega", new SteelFunctor2Arg<Application,uint,const SteelType::Handle&>(this,&Application::equipOmega) );
    mInterpreter->addFunction ( "unequipOmega", new SteelFunctor1Arg<Application,uint>(this,&Application::unequipOmega) );
    mInterpreter->addFunction ( "omegaSlotCount", new SteelFunctorNoArgs<Application>(this,&Application::omegaSlotCount) );
    mInterpreter->addFunction ( "getOmega", new SteelFunctor1Arg<Application,uint>(this,&Application::getOmega ) );
    mInterpreter->addFunction ( "omegaSlotIsEmpty", new SteelFunctor1Arg<Application,uint>(this,&Application::omegaSlotIsEmpty) );
    mInterpreter->addFunction ( "banner", new SteelFunctor2Arg<Application,const std::string&,double>(this,&Application::banner) );
	mInterpreter->addFunction ( "closeMap", fn_closeMap );
	mInterpreter->addFunction ( "gameoverScreen", fn_gameoverScreen );
	mInterpreter->addFunction ( "giveItem", new SteelFunctor3Arg<Application,SteelType::Handle,int,bool>(this,&Application::giveItem) );
	mInterpreter->addFunction ( "configJoystick", new SteelFunctorNoArgs<Application>(this,&Application::configJoystick));
	
	mInterpreter->addFunction ( "stopMusic", new SteelFunctorNoArgs<Application>(this,&Application::stopMusic));
	mInterpreter->addFunction ( "startMusic", new SteelFunctorNoArgs<Application>(this,&Application::startMusic));
	mInterpreter->addFunction ( "pushMusic", new SteelFunctor1Arg<Application, const std::string&>(this,&Application::pushMusic));
	mInterpreter->addFunction ( "popMusic", new SteelFunctorNoArgs<Application>(this,&Application::popMusic));
	
	mInterpreter->addFunction ( "deployCharacter", new SteelFunctor1Arg<Application,const std::string&>(this,&Application::deployCharacter) );
	mInterpreter->addFunction ( "reserveCharacter", new SteelFunctor1Arg<Application,const std::string&>(this,&Application::reserveCharacter) );
	
	mInterpreter->addFunction ( "getReservePartyArray", new SteelFunctorNoArgs<Application>(this,&Application::getReservePartyArray) );
	    
    mInterpreter->addFunction ( "debug", new SteelFunctorNoArgs<Application>(this,&Application::debug) );
    mInterpreter->addFunction ( "editing", new SteelFunctorNoArgs<Application>(this,&Application::editing) );
    mInterpreter->addFunction ( "editMap", new SteelFunctorNoArgs<Application>(this,&Application::editMap) );

#ifndef NDEBUG
	mInterpreter->addFunction ( "enableInfiniteBP", new SteelFunctor1Arg<Application,bool>(this,&Application::enableInfiniteBP) );
	mInterpreter->addFunction ( "enableInfiniteSP", new SteelFunctor1Arg<Application,bool>(this,&Application::enableInfiniteSP) );	
	mInterpreter->addFunction ( "enableInfiniteGold", new SteelFunctor1Arg<Application,bool>(this,&Application::enableInfiniteGold) );	
	mInterpreter->addFunction ( "enableAllSkills", new SteelFunctor1Arg<Application,bool>(this,&Application::enableAllSkills) );		
#endif	
}

void Application::queryJoystick()
{
#if 0

    if ( m_window.get_ic().get_joystick_count() )
    {
        clan::InputDevice& joystick = m_window.get_ic().get_joystick ( 0 );
        mStates.back()->HandleAxisMove ( AXIS_HORIZONTAL, joystick.get_axis ( 0 ) );
        mStates.back()->HandleAxisMove ( AXIS_VERTICAL, joystick.get_axis ( 1 ) );
    }

#endif
}

void Application::draw()
{
    clan::Rect dst = GetDisplayRect();
    
    m_window.get_gc().set_scissor(dst, m_window.get_gc().get_texture_image_y_axis());

    //std::vector<State*>::iterator end = mStates.end();

    for ( std::deque<State*>::iterator iState = mStates.begin();
            iState != mStates.end(); iState++ )
    {
        State * pState = *iState;
		clan::Canvas canvas(m_window);
        pState->Draw ( dst, canvas );
        if ( pState->LastToDraw() ) break; // Don't draw any further.

    }

    m_window.get_gc().reset_scissor();
}

void Application::run(bool process_functors)
{
    State * backState = mStates.back();
	
	if(backState->Threaded()) m_threaded_mode = true;
	try{
		backState->SteelInit ( mInterpreter );
		backState->Start();
		unsigned int then = clan::System::get_time();
	#ifndef NDEBUG
		m_draw_start_time = clan::System::get_time();
	#endif

		while ( !mbDone &&  !backState->IsDone() )
		{
			queryJoystick();
			
			mFunctorMutex.lock();
			
			if(process_functors && !m_mainthread_functors.empty()){
				std::function<void()> func = m_mainthread_functors.front().m_functor;
				func();
				m_mainthread_functors.front().m_event.set();						
				m_mainthread_functors.pop();
			}
			mFunctorMutex.unlock();

			unsigned int now = clan::System::get_time();

			if ( now - then > MS_BETWEEN_MOVES )
			{
				bool disableMOs = false;
				for(int i=0;i<mStates.size();i++){
					if(mStates[i]->DisableMappableObjects()){
						disableMOs = true;
						break;
					}
				}
				if ( !disableMOs )
				{
					mMapState.MoveMappableObjects();
					mStates.back()->MappableObjectMoveHook();
				}

				then = now;
			}


			draw();

			m_window.flip(0);


			clan::KeepAlive::process();
#if SHOW_FPS
			m_draws_total++;
			//long  = clan::System::get_time();
			if(now - m_draw_start_time > 3000){
				float fps = float(m_draws_total) / (float(now - m_draw_start_time) / 1000.0);
				std::cout << "FPS : " << fps << std::endl;
				m_draw_start_time = now;
				m_draws_total = 0;
			}
#endif
			
			clan::System::sleep ( 10 );
		}


		mStates.back()->Finish();

		mStates.back()->SteelCleanup ( mInterpreter );
		// TODO: Or is this backState->Threaded()? Is it any different?
		if(mStates.back()->Threaded())
			m_threaded_mode = false;

		mStates.pop_back();
	}catch ( SteelException ex ){
		if(!mStates.empty()){
			mStates.back()->Finish();
			mStates.back()->SteelCleanup ( mInterpreter );		
			mStates.pop_back();
		}
        showError ( ex.getLine(), ex.getScript(), ex.getMessage() );
		mbDone = true;
    }

}

void Application::loadscript ( std::string &o_str, const std::string & filename, clan::FileSystem fs )
{
	clan::IODevice file = fs.open_file(filename);
    char buffer[1024];
	memset(buffer,0,sizeof(buffer));
  
	int count = 0;
    while ( count = file.read(buffer, sizeof(buffer)-1) )
    {
		buffer[count] = 0;
		o_str += buffer;
    }
}

Level* Application::GetCurrentLevel() const
{
    return mMapState.GetCurrentLevel();
}

clan::Point Application::GetCurrentLevelCenter() const
{
	return mMapState.GetCurrentCenter();
}

clan::IODevice Application::OpenResource(const std::string& str)
{
	return m_resource_dir.open_file(str);
}

int Application::main ( const std::vector<std::string> args )
{
	//SteelInterpreter interpreter;
	main_thread_id = std::this_thread::get_id();
	mInterpreter = new SteelInterpreter();

    GraphicsManager::initialize();
    ItemManager::initialize();
    SoundManager::initialize();
    CharacterManager::initialize();
	AbilityManager::initialize();


#ifndef NDEBUG

    clan::ConsoleWindow console ( "Stone Ring Debug", 80, 100 );
#endif
    int njoystick = -1;
    setupClanLib();
	

	
	std::string data_file = "stone_ring2.sr2";
	bool data_dir=false; 
	bool dump_equipment = false;

    for ( int i = 0;i < args.size();i++ )
    {
 		std::string string = args[i];

        if ( string.substr ( 0, 5 ) == "--js=" )
        {
            njoystick = atoi ( string.substr ( 5 ).c_str() );
        }
        else if(string.substr( 0, 2 ) == "-f" )
		{
			data_dir = false;
			if(args.size() > i+1)
				data_file = args[i+1];
		}else if(string.substr( 0, 2 ) == "-d")
		{
			if(args.size() > i+1){
				data_dir = true;
				data_file = args[i+1];
				std::cerr << data_file << " used as package" << std::endl;
			}
		}else if(string.substr( 0, 2 ) == "-m"){
			SoundManager::SetMusicMaxVolume(0.0);
			SoundManager::SetSoundVolume(0.0);
		}else if(string.substr(0,12) == "--dump-equip"){
			dump_equipment = true;
		}
			
    }
    
    m_resource_path = data_file;

	m_button_down[IApplication::BUTTON_ALT] = false;
	m_button_down[IApplication::BUTTON_CONFIRM] = false;
	m_button_down[IApplication::BUTTON_CANCEL] = false;
	m_button_down[IApplication::BUTTON_L] = false;
	m_button_down[IApplication::BUTTON_R] = false;
	m_button_down[IApplication::BUTTON_SELECT] = false;
	m_button_down[IApplication::BUTTON_START] = false;
	m_button_down[IApplication::BUTTON_MENU] = false;	
	
	
	mbDone = false;
    
    //clan::Display::get_buffer()
    try
    {
		std::ifstream joystick_in("joystick.set", std::ios::in);
		if(joystick_in.good()){
			m_joystick_config.Read(joystick_in);
		}
		


		//mInterpreter = &interpreter;
        registerSteelFunctions();

		// TODO: Get the package from the command line
		clan::FileSystem vfs(data_file, !data_dir);
		
		m_resource_dir = vfs;
		
		m_zip_provider.SetVirtualDirectory(m_resource_dir);

		mInterpreter->setFileProvider(&m_zip_provider);

        m_resources = clan::XMLResourceManager::create ( clan::XMLResourceDocument("Media/resources_cl30.xml", m_resource_dir) );

#ifdef NDEBUG
        std::string name = String_load ( "Configuration/name", m_resources );
#else
        std::string name = String_load ( "Configuration/name", m_resources ) + " (DEBUG)";
#endif
        mGold = String_load ( "Game/Currency", m_resources );


        clan::DisplayWindowDescription desc;
        desc.set_title ( name );
        desc.set_size ( clan::Size ( WINDOW_WIDTH, WINDOW_HEIGHT ), true );


        m_window = clan::DisplayWindow ( desc );
        

        std::string battleConfig = String_load ( "Configuration/BattleConfig", m_resources );
        mBattleConfig.Load ( battleConfig );
        mBattleState.SetConfig ( &mBattleConfig );
        
        std::string defaultTheme = String_load ("Game/DefaultTheme", m_resources );
        GraphicsManager::SetTheme(defaultTheme);
		
		
		m_max_party_size = clan::XMLResourceManager::get_doc(m_resources).get_integer_resource("Game/ActivePartyMaximumCharacters",4);

        //for(int i =0; i < m_window.get_buffer_count(); i++)
        //  m_window.get_buffer(i).to_format(clan::PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));

        m_window.get_gc().clear ( clan::Colorf ( 0.0f, 0.0f, 0.0f ) );

		
        std::string utilityConfig = String_load ( "Configuration/UtilityScripts", m_resources );
        mUtilityScripts.Load ( utilityConfig );
#if SR2_EDITOR        
        mMapEditorState.Init(m_window);
#endif     
    }
    catch ( clan::Exception error )
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;

#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }
    catch( XMLException& e )
	{
		e.dump(std::cerr);
		return 1;
	}
	catch ( AlreadyDefined ad) 
	{
		std::cerr << "Steel Exception: Already defined symbol: " << ad.GetName() << std::endl;
		return 1;
	}
    catch ( SteelException ex )
    {
        std::cerr << "Steel Exception on line " << ex.getLine()
                  << " of " << ex.getScript() << ':' << ex.getMessage() << std::endl;
#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }
  

    m_startTime = clan::System::get_time();

    try
    {
        clan::InputDevice keyboard = m_window.get_ic().get_keyboard();
        clan::InputDevice mouse = m_window.get_ic().get_mouse();

        clan::Slot slot_quit = m_window.sig_window_close().connect ( this, &Application::onSignalQuit );
        clan::Slot slot_key_down = keyboard.sig_key_down().connect ( this, &Application::onSignalKeyDown );
        clan::Slot slot_key_up  = keyboard.sig_key_up().connect ( this, &Application::onSignalKeyUp );
        
        clan::Slot slot_mouse_up = mouse.sig_key_up().connect ( this, &Application::onSignalMouseUp  );
        clan::Slot slot_mouse_down = mouse.sig_key_down().connect ( this, &Application::onSignalMouseDown  );
        clan::Slot slot_dbl_click = mouse.sig_key_dblclk().connect ( this, &Application::onSignalDoubleClick  );
        clan::Slot slot_mouse_move = mouse.sig_pointer_move().connect ( this, &Application::onSignalMouseMove  );
		
		m_window.sig_lost_focus().connect ( this, &Application::onSignalLostFocus );
		m_window.sig_window_minimized().connect( this, &Application::onSignalLostFocus );

        clan::Slot joystickDown;
        clan::Slot joystickUp;
        clan::Slot joystickAxis;
		
		m_joystick_train_state = JS_TRAIN_IDLE;

        if ( njoystick > 0 &&  njoystick < m_window.get_ic().get_joystick_count() )
        {
            std::cout << "Joystick count = " << m_window.get_ic().get_joystick_count();
    #if 1
            clan::InputDevice& joystick = m_window.get_ic().get_joystick ( njoystick );
            joystickDown = joystick.sig_key_down().connect ( this, &Application::onSignalJoystickButtonDown );
            joystickUp = joystick.sig_key_up().connect ( this, &Application::onSignalJoystickButtonUp );
            joystickAxis = joystick.sig_axis_move().connect ( this, &Application::onSignalJoystickAxisMove );
    #endif
        }

        m_window.get_gc().clear ( clan::Colorf ( 0.0f, 0.0f, 0.0f ) );

				
        showRechargeableOnionSplash();


		
		mbDone = false;

		
		std::function<void()> load_f = std::bind ( &AppUtils::LoadGameplayAssets, "", m_resources );
		LoadingState loader;
		loader.init(load_f);
		RunState(&loader);
        RunState(&mStartupState);

#ifndef NDEBUG
		console.display_close_message();
		console.wait_for_key();
#endif


        teardownClanLib();
    }
    catch ( clan::Exception error )
    {
        while ( mStates.size() )
            mStates.pop_back();
        
        std::cerr << "Exception caught" << std::endl;
        std::cerr << error.message.c_str() << std::endl;
    }
    catch( AlreadyDefined ad ){
        std::cerr << "Already defined: " << ad.GetName() << std::endl;
    }

    mInterpreter->popScope();
	
    return 0;

}


std::string     Application::GetResourcePath()const{
	return m_resource_path;
}


AstScript* Application::GetUtility ( Utility util ) const
{
    switch ( util )
    {
        case XP_FOR_LEVEL:
            return mUtilityScripts.GetScript ( "tnl" );
        case LEVEL_FOR_XP:
            return mUtilityScripts.GetScript ( "lnt" );
        case ON_ATTACK:
            return mUtilityScripts.GetScript ( "on_attack" );
    }

    assert ( 0 );

    return NULL;
}

uint Application::GetMinutesPlayed() const
{
    return (clan::System::get_time() - m_startTime) / 60000;
}


void Application::showRechargeableOnionSplash()
{
}

void Application::showIntro()
{
	clan::Canvas canvas(m_window);

    clan::InputDevice keyboard = m_window.get_ic().get_keyboard();
    clan::Image splash = clan::Image::resource( canvas, "Configuration/splash", m_resources ).get();
    clan::Image background  = clan::Image::resource( canvas, "Configuration/splashbg", m_resources ).get();

    // clan::Canvas *gc = m_window.get_gc();

    int displayX = ( WINDOW_WIDTH - splash.get_width() ) / 2;
    int displayY = ( WINDOW_HEIGHT - splash.get_height() ) / 2;



    while ( !keyboard.get_keycode ( clan::keycode_enter ) )
    {
        if ( m_window.get_ic().get_joystick_count() )
        {
            clan::InputDevice& joystick = m_window.get_ic().get_joystick ( 0 );

            if ( joystick.get_keycode ( 5 ) ) break;
        }


        background.draw ( canvas, 0, 0 );

        splash.draw ( canvas, static_cast<float> ( displayX ),
                      static_cast<float> ( displayY ) );

        m_window.flip();
		clan::KeepAlive::process(1);
		clan::System::sleep(0);
    }
#ifndef NDEBUG
	std::cout << "Finished, now release enter..." << std::endl;
#endif

    // Wait for them to release the key before moving on.
    while ( keyboard.get_keycode ( clan::keycode_enter ) ) clan::KeepAlive::process();


}

int Application::calc_fps ( int frame_time )
{
    static int fps_result = 0;
    static int fps_counter = 0;
    static int total_time = 0;

    total_time += frame_time;

    if ( total_time >= 1000 )   // One second has passed
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
    static int main ( const std::vector<std::string> &args )
    {

    	//std::cout << "Usable space: " << malloc_usable_size() << std::endl;
    	std::cout << sizeof(Application) << std::endl;
        clan::SetupCore setup_core;
        clan::SetupDisplay setup_display;
        clan::SetupGL setup_gl;
        clan::SetupSound setup_sound;
        clan::SoundOutput output(44100);
		
		std::vector<std::string> nargs;
		for(int i=0;i<args.size();i++)
			nargs.push_back(args[i]);

        Application app;
        pApp = &app;
        return app.main ( nargs );
    }
};

clan::Application app ( &Program::main );


// doko





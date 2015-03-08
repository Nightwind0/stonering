#include "AnimationState.h"
#include "IApplication.h"
#include "BattleState.h"
#include "WeaponType.h"
#include "SoundManager.h"
#include "ICharacterGroup.h"
#include "GraphicsManager.h"
#include "sr_defines.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace StoneRing;

class AnimationRunner: public SteelRunner<AnimationState> {
public:
	AnimationRunner( SteelInterpreter* pInterpreter, AnimationState* pState )
		: SteelRunner<AnimationState>( pInterpreter, pState, &AnimationState::FunctorCompleted ) {
	}
	virtual ~AnimationRunner() {
	}
	virtual void run() {
		// TODO: Should I support an AstScript here too, like the normal runner does?
		try {
			if( m_pFunctor ) {

				m_result = m_pFunctor->Call( m_pInterpreter, SteelType::Container() );
			}
#if 1
			std::cerr << "Cut scene functor finished. Waiting for tasks to finish." << std::endl;
#endif

			// wait for all tasks to finish
			m_callee->WaitFinishedEvent();
		} catch( Steel::SteelException ex ) {
			// TODO: On the main thread do an exception screen
			std::cerr << "Exception in　animation: " << ex.getScript() << ':' << ex.getMessage() << " on line " << ex.getLine() << std::endl;

		}

		( m_callee->*m_callback )();
	}

};


AnimationState::AnimationState( BattleState& parent ):
	m_parent( parent ),m_bDone( false ) {

}


AnimationState::~AnimationState() {
}


void AnimationState::Init( SteelType::Functor pFunctor ) {
	m_functor = pFunctor;
	m_bDone = false;
}

void AnimationState::AddTask( AnimationState::Task* task ) {
	if(task->running() || task->expired()) return;
	add_task(task);
}


BattleState::SpriteTicket AnimationState::AddSprite( clan::Sprite sprite ) {
	return m_parent.add_sprite( sprite );
}


clan::Rectf AnimationState::GetCharacterRect( ICharacter* ichar ) const {
	return m_parent.get_character_rect( ichar );
}
clan::Rectf AnimationState::GetGroupRect( ICharacterGroup* igroup ) const {
	return m_parent.get_group_rect( igroup );
}
clan::Rectf AnimationState::GetSpriteRect( BattleState::SpriteTicket sprite ) const {
	return m_parent.get_sprite_rect( sprite );
}
void AnimationState::SetSpritePos( BattleState::SpriteTicket sprite, const clan::Pointf& pt ) {
	m_parent.set_sprite_pos( sprite, pt );
}




bool AnimationState::IsDone() const {
	return m_bDone;
}


void AnimationState::WaitFinishedEvent() {
	while( active_tasks_left() ) {
		start( get_top_task() );
		waitFor( get_top_task() );
	}
}



void AnimationState::HandleKeyDown( const clan::InputEvent &key ) {
}

void AnimationState::HandleKeyUp( const clan::InputEvent &key ) {
	if(key.id == clan::keycode_t){
		std::cout << "Current tasks: " << std::endl;
		for(auto it : m_tasks){
			std::cout << '\t' << it->GetName() <<  " state " << it->GetState() << " %" << 100.0 * it->percentage() << std::endl;
		}
	}
}


void AnimationState::Draw( const clan::Rect& screenRect, clan::Canvas& GC ) {
	draw_functor( screenRect, GC );
}


void AnimationState::draw_functor( const clan::Rect& screenRect, clan::Canvas& GC ) {
	bool notasks = false;
	bool emergency = false;
	std::unique_lock<std::recursive_mutex> lock(m_task_mutex);
	for( auto it = std::begin(m_tasks); it != std::end(m_tasks); it++ ){
		Task * pTask = *it;
		if(!pTask->running() ||  pTask->expired()) 
			continue;
		try{
			pTask->update( m_pInterpreter );
		}catch(Steel::SteelException ex){
			emergency = true;
			std::cerr << "Quitting task " << pTask->GetName() <<  " after SteelException: " << ex.getScript() << ':' << ex.getMessage() << std::endl;
		}catch(Steel::ParamMismatch pm){
			emergency = true;
			std::cerr << "Param mismatch within animation task update. Task is: " << pTask->GetName() << " of functor: " << m_functor->getIdentifier() << std::endl;
		}
		if( pTask->finished() || emergency ) {

			pTask->finish( m_pInterpreter );
			// Do something with waitFor?
#ifndef NDEBUG
			//std::cout << "Finished task: " << pTask->GetName() << '@' << std::hex << pTask << std::endl;
#endif
			//m_wait_event.set();
		}
	}
}


bool AnimationState::LastToDraw() const { // Should we continue drawing more states?
	return false;
}

bool AnimationState::DisableMappableObjects() const { // Should the app move the MOs?
	return true;
}

void AnimationState::Update() { // Do stuff right after the mappable object movement
}



bool AnimationState::active_tasks_left() const {
	std::unique_lock<std::recursive_mutex> lock(m_task_mutex);
	for(auto it = std::begin(m_tasks); it != std::end(m_tasks); it++){
		if((*it)->running())
			return true;
	}
	return false;
}


void AnimationState::Start() {
	m_bDone = false;
	m_pRunner->setFunctor( m_functor );
	m_steel_thread.start( m_pRunner );
}

void AnimationState::FunctorCompleted() {
	std::cout << "Functor complete" << std::endl;
	m_bDone = true;
}


void AnimationState::SteelInit( SteelInterpreter *pInterpreter ) {
		using namespace Steel;
		m_pInterpreter = pInterpreter;
		m_pRunner = new AnimationRunner( pInterpreter, this );
		pInterpreter->pushScope();
		pInterpreter->addFunction( "sine_wave", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::sine_wave ) );
		pInterpreter->addFunction( "arc_over", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::arc_over ) );
		pInterpreter->addFunction( "arc_under", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::arc_under ) );
		pInterpreter->addFunction( "createSprite", "anim", new SteelFunctor1Arg<AnimationState, const std::string&>( this, &AnimationState::createSprite ) );
		pInterpreter->addFunction( "getCharacterSprite", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::getCharacterSprite ) );
		pInterpreter->addFunction( "addWeaponSprite", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::addWeaponSprite ) );
		pInterpreter->addFunction( "removeSprite", "anim", new SteelFunctor1Arg<AnimationState, int>( this, &AnimationState::removeSprite ) );
		pInterpreter->addFunction( "getCharacterLocale", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getCharacterLocale ) );
		pInterpreter->addFunction( "getGroupLocale", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getGroupLocale ) );
		pInterpreter->addFunction( "getScreenLocale", "anim", new SteelFunctor1Arg<AnimationState, int>( this, &AnimationState::getScreenLocale ) );
		pInterpreter->addFunction( "getSpriteLocale", "anim", new SteelFunctor2Arg<AnimationState, int, int>( this, &AnimationState::getSpriteLocale ) );
		pInterpreter->addFunction( "setLocaleOffset", "anim", new SteelFunctor3Arg<AnimationState, SteelType::Handle, int, int>( this, &AnimationState::setLocaleOffset ) );
		pInterpreter->addFunction( "createPath", "anim", new SteelFunctor4Arg<AnimationState, SteelType::Handle, SteelType::Handle, SteelType::Functor, double>( this, &AnimationState::createPath ) );
		pInterpreter->addFunction( "changePathStart", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::changePathStart ) );
		pInterpreter->addFunction( "changePathEnd", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::changePathEnd ) );
		pInterpreter->addFunction( "getCharacterLocus", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getCharacterLocus ) );
		pInterpreter->addFunction( "isLeftOf", "anim", new SteelFunctor2Arg<AnimationState,int,int>(this, &AnimationState::isLeftOf ) );



		pInterpreter->addFunction( "setPathSpeedFunction", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Functor>( this, &AnimationState::setPathSpeedFunction ) );
		pInterpreter->addFunction( "setPathCompletion", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, double>( this, &AnimationState::setPathCompletion ) );
		pInterpreter->addFunction( "setPathFlags", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::setPathFlags ) );

		pInterpreter->addFunction( "moveSprite", "anim", new SteelFunctor2Arg<AnimationState, int, SteelType::Handle>( this, &AnimationState::moveSprite ) );
		pInterpreter->addFunction( "createRotation", "anim", new SteelFunctor4Arg<AnimationState, SteelType::Functor, double,double, int>( this, &AnimationState::createRotation ) );
		pInterpreter->addFunction( "createStretch", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Functor, SteelType::Functor>( this, &AnimationState::createStretch ) );
		pInterpreter->addFunction( "stretchSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::stretchSpriteTimed ) );

		pInterpreter->addFunction( "rotateSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::rotateSpriteTimed ) );
		pInterpreter->addFunction( "movePathTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );

		pInterpreter->addFunction( "moveSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );
/*
 * 		SteelType setSpriteRotation(int sprite, double radians);
		SteelType flipSprite(int sprite, bool flip_x, bool flip_y);
		SteelType scaleSprite(int sprite, double scale_x, double scale_y);
		SteelType setSpriteLocation(int sprite, SteelType::Handle hLocale);
		SteelType closestCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTarget);
		SteelType nearCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner);
		SteelType farCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner); 		
		*/
		pInterpreter->addFunction( "setSpriteRotation", "anim", new SteelFunctor2Arg<AnimationState,int,double>(this, &AnimationState::setSpriteRotation) );
		pInterpreter->addFunction( "flipSprite", "anim", new SteelFunctor3Arg<AnimationState,int,bool,bool>(this,&AnimationState::flipSprite) );
		pInterpreter->addFunction( "scaleSprite","anim", new SteelFunctor3Arg<AnimationState,int,double,double>(this,&AnimationState::scaleSprite) );
		pInterpreter->addFunction( "setSpriteLocation", "anim", new SteelFunctor2Arg<AnimationState,int,SteelType::Handle>(this,&AnimationState::setSpriteLocation) );
		pInterpreter->addFunction( "closestCorner", "anim", new SteelFunctor2Arg<AnimationState,SteelType::Handle,SteelType::Handle>(this,&AnimationState::closestCorner) );
		pInterpreter->addFunction( "nearCorner", "anim", new SteelFunctor3Arg<AnimationState,SteelType::Handle, SteelType::Handle,int>(this,&AnimationState::nearCorner) );
		pInterpreter->addFunction( "farCorner", "anim", new SteelFunctor3Arg<AnimationState,SteelType::Handle, SteelType::Handle,int>(this,&AnimationState::farCorner) );
		
		pInterpreter->addFunction( "syncTo", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::syncTo ) );
		pInterpreter->addFunction( "rotateSprite", "anim", new SteelFunctor2Arg<AnimationState, int, SteelType::Handle>( this, &AnimationState::rotateSprite ) );
		pInterpreter->addFunction( "createShaker", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Functor, int>( this, &AnimationState::createShaker ) );
		pInterpreter->addFunction( "shake", "anim",new SteelFunctor3Arg<AnimationState, SteelType::Handle, SteelType::Handle, double>( this, &AnimationState::shake ) );
		pInterpreter->addFunction( "createFade", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Functor>( this, &AnimationState::createFade ) );
		pInterpreter->addFunction( "fadeSprite", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::fadeSprite ) );
		pInterpreter->addFunction( "createColorizer", "anim", new SteelFunctor3Arg<AnimationState, SteelType::Functor, SteelType::Functor, SteelType::Functor>( this, &AnimationState::createColorizer ) );

		pInterpreter->addFunction( "setSpriteColor", "anim", Steel::create_functor(this,&AnimationState::setSpriteColor) );
		pInterpreter->addFunction( "colorizeSprite", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::colorizeSprite ) );
		pInterpreter->addFunction( "startAfter", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::startAfter ) );
		pInterpreter->addFunction( "doAfter", "anim", new SteelFunctor2Arg<AnimationState, double, SteelType::Functor>( this, &AnimationState::doAfter ) );
		pInterpreter->addFunction( "doFunction", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Functor>( this, &AnimationState::doFunction ) );
		pInterpreter->addFunction( "pause", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::pause ) );
		pInterpreter->addFunction( "chainTasks", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::chainTasks ) );
		pInterpreter->addFunction( "waitFor", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::waitFor ) );
		pInterpreter->addFunction( "waitForAll", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::waitForAll ) );
		pInterpreter->addFunction( "start", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::start ) );
		pInterpreter->addFunction( "startAll", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::startAll ) );

		pInterpreter->addFunction( "createOrbit", "anim", new SteelFunctor5Arg<AnimationState, SteelType::Functor, SteelType::Functor,double, double, bool>(this, &AnimationState::createOrbit) );
		pInterpreter->addFunction( "orbitSprite", "anim", new SteelFunctor3Arg<AnimationState,int,SteelType::Handle,SteelType::Handle>(this, &AnimationState::orbitSprite) );
		pInterpreter->addFunction( "orbitSpriteTimed", "anim", new SteelFunctor4Arg<AnimationState,int,SteelType::Handle,SteelType::Handle,double>(this, &AnimationState::orbitSpriteTimed) );
		pInterpreter->addFunction( "floatCharacter", "anim", new SteelFunctor3Arg<AnimationState,SteelType::Handle, SteelType::Functor, double>(this, &AnimationState::floatCharacter ) );
		
		pInterpreter->addFunction( "setSpriteZorder", "anim", Steel::create_functor(this,&AnimationState::setSpriteZorder) );
		pInterpreter->addFunction( "getSpriteZorder", "anim", Steel::create_functor(this,&AnimationState::getSpriteZorder) );
		SteelConst( pInterpreter, "$_TOP_LEFT", ( int )Locale::TOP_LEFT );
		SteelConst( pInterpreter, "$_TOP_RIGHT", ( int )Locale::TOP_RIGHT );
		SteelConst( pInterpreter, "$_TOP_CENTER", ( int )Locale::TOP_CENTER );
		SteelConst( pInterpreter, "$_MIDDLE_LEFT", ( int )Locale::MIDDLE_LEFT );
		SteelConst( pInterpreter, "$_MIDDLE_RIGHT", ( int )Locale::MIDDLE_RIGHT );
		SteelConst( pInterpreter, "$_CENTER", ( int )Locale::CENTER );
		SteelConst( pInterpreter, "$_BOTTOM_LEFT", ( int )Locale::BOTTOM_LEFT );
		SteelConst( pInterpreter, "$_BOTTOM_RIGHT", ( int )Locale::BOTTOM_RIGHT );
		SteelConst( pInterpreter, "$_BOTTOM_CENTER", ( int )Locale::BOTTOM_CENTER );
		SteelConst( pInterpreter, "$_MOVE_NORMAL", ( int )Path::NORMAL );
		SteelConst( pInterpreter, "$_MOVE_NO_VERTICAL", ( int )Path::NO_VERTICAL );
		SteelConst( pInterpreter, "$_MOVE_NO_HORIZONTAL", ( int )Path::NO_HORIZONTAL );


		SteelConst( pInterpreter, "$_AXIS_ROLL", ( int )Rotation::ROLL );
		SteelConst( pInterpreter, "$_AXIS_YAW", ( int )Rotation::YAW );
		SteelConst( pInterpreter, "$_AXIS_PITCH", ( int )Rotation::PITCH );
}

void AnimationState::SteelCleanup( SteelInterpreter *pInterpreter ) {
	pInterpreter->popScope();
	delete m_pRunner;
	m_pRunner = nullptr;
}

clan::Pointf AnimationState::GetGroupOffset( ICharacterGroup* igroup ) const {
	return m_parent.get_offset( igroup );
}

BattleState::SpriteTicket AnimationState::GetSpriteForChar( ICharacter* iChar ) {
	return m_parent.get_sprite_for_char( iChar );
}

clan::Pointf AnimationState::GetSpriteOffset( BattleState::SpriteTicket sprite ) const {
	return m_parent.get_offset( sprite );
}

void AnimationState::SetShadowOffset(ICharacter *ichar, const clan::Pointf& pt){
	return m_parent.set_shadow_offset(ichar,pt);
}

void AnimationState::SetGroupOffset( ICharacterGroup* igroup, const clan::Pointf& pt ) {
	m_parent.set_offset( igroup, pt );
}

void AnimationState::SetSpriteOffset( BattleState::SpriteTicket sprite, const clan::Pointf& pt ) {
	m_parent.set_offset( sprite, pt );
}


SteelType AnimationState::sine_wave( double p ) {
	SteelType var;
	var.set( sin( p ) );
	return var;
}

SteelType AnimationState::arc_over( double p ) {
	SteelType var;
	var.set( sin( p * M_PI * 2.0f ) );
	return var;
}

SteelType AnimationState::arc_under( double p ) {
	SteelType var;
	var.set( -sin( p * M_PI ) );
	return var;
}

SteelType AnimationState::createSprite( const std::string& sprite_ref ) {
	BattleState::SpriteTicket ticket = BattleState::UNDEFINED_SPRITE_TICKET;
	std::function<void()> func = [&](){
		clan::Sprite sprite = GraphicsManager::CreateSprite( sprite_ref );
		sprite.set_alignment(clan::origin_center);
		ticket = AddSprite(sprite);
	};
	IApplication::GetInstance()->RunOnMainThread( func );
	SteelType var;
	var.set( ticket );
	return var;
}

SteelType AnimationState::getCharacterSprite( SteelType::Handle iCharacter ) {
	ICharacter* ichar = Steel::GrabHandle<ICharacter*>( iCharacter );
	SteelType var;
	var.set( m_parent.get_sprite_for_char( ichar ) );
	return var;
}

SteelType AnimationState::addWeaponSprite( SteelType::Handle hWeapon ) {
	SteelType var;
	Weapon * pWeapon = Steel::GrabHandle<Weapon*>(hWeapon);
	if( pWeapon ) { 
		int sprite = m_parent.add_sprite( pWeapon->GetSprite() );
		var.set( sprite );	
	} else {
		var.set( BattleState::UNDEFINED_SPRITE_TICKET );
	}

	return var;
}

SteelType AnimationState::getSpriteZorder(int sprite){
    SteelType var;
    var.set(m_parent.get_zorder(sprite));
    return var;
}

SteelType AnimationState::setSpriteZorder(int sprite, int zorder){
    m_parent.set_zorder(sprite,zorder);
    return SteelType();	
}

SteelType AnimationState::removeSprite( int sprite ) {
	m_parent.remove_sprite( sprite );
	return SteelType();
}

SteelType AnimationState::getCharacterLocale( SteelType::Handle iCharacter, int corner ) {
	Locale* locale = new Locale( Locale::CHARACTER, ( Locale::Corner )corner );
	locale->SetIChar( Steel::GrabHandle<ICharacter*>( iCharacter ) );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getCharacterLocus( SteelType::Handle iCharacter, int corner ) {
	Locale* locale = new Locale( Locale::CHARACTER_LOCUS, ( Locale::Corner )corner );
	locale->SetIChar( Steel::GrabHandle<ICharacter*>( iCharacter ) );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getGroupLocale( SteelType::Handle iGroup, int corner ) {
	Locale* locale = new Locale( Locale::GROUP, ( Locale::Corner )corner );
	locale->SetGroup( Steel::GrabHandle<ICharacterGroup*>( iGroup ) );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getScreenLocale( int corner ) {
	Locale* locale = new Locale( Locale::SCREEN, ( Locale::Corner )corner );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getSpriteLocale( int sprite, int corner ) {
	Locale* locale = new Locale( Locale::SPRITE, ( Locale::Corner )corner );
	locale->SetSprite(sprite);
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::setLocaleOffset( SteelType::Handle hLocale, int x, int y ) {
	Locale * locale = Steel::GrabHandle<Locale*>( hLocale );
	locale->SetOffset( clan::Point( x, y ) );
	return SteelType();
}


SteelType AnimationState::createPath( SteelType::Handle hStartLocale, SteelType::Handle hEndLocale,	SteelType::Functor functor, double pixels_per_ms ) {
	Path * path = new Path();
	path->m_start = *Steel::GrabHandle<Locale*>( hStartLocale );
	path->m_end = *Steel::GrabHandle<Locale*>( hEndLocale );
	path->m_functor = functor;
	path->m_speed = pixels_per_ms;
	m_handles.push_back( path );
	SteelType var;
	var.set( path );
	return var;
}

SteelType AnimationState::changePathStart( SteelType::Handle hPath, SteelType::Handle hStartLocale ) {
	Path * path = Steel::GrabHandle<Path*>( hPath );
	Locale* start = Steel::GrabHandle<Locale*>( hStartLocale );

	path->m_start = *start;
	return SteelType();
}

SteelType AnimationState::changePathEnd( SteelType::Handle hPath, SteelType::Handle hEndLocale ) {
	Path * path = Steel::GrabHandle<Path*>( hPath );
	Locale* end = Steel::GrabHandle<Locale*>( hEndLocale );

	path->m_start = *end;
	return SteelType();
}

SteelType AnimationState::setPathSpeedFunction( SteelType::Handle hPath, SteelType::Functor functor ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_speed_functor = functor;
	return SteelType();
}

SteelType AnimationState::setPathCompletion( SteelType::Handle hPath, double completion ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_completion = completion;
	return SteelType();
}

SteelType AnimationState::setPathFlags( SteelType::Handle hPath, int flags ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_flags = flags;
	return SteelType();
}

SteelType AnimationState::setSpriteColor(int sprite, double r, double g, double b){
	GetSprite(sprite).set_color(clan::Colorf(float(r),float(g),float(b)));
	return SteelType();
}

SteelType AnimationState::moveSprite( int sprite, SteelType::Handle hpath ) {
	PathTask* task = new PathTask( *this );
#ifndef NDEBUG
	//std::cout << "PathTask created: " << std::hex << task << std::endl;
#endif
	task->SetSprite( sprite );
	task->init( Steel::GrabHandle<Path*>( hpath ) );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::moveSpriteTimed( int sprite, SteelType::Handle hpath, double seconds ) {
	TimedPathTask* task = new TimedPathTask( *this );
#ifndef NDEBUG
	std::cout << "TimedPathTask created: " << std::hex << task << std::endl;
#endif
	task->SetSprite( sprite );
	task->SetDuration( seconds );
	task->init( Steel::GrabHandle<Path*>( hpath ) );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::createRotation( SteelType::Functor functor, double start_degrees, double degrees, int axis ) {
	Rotation* rotation = new Rotation();
	rotation->m_axis = ( Rotation::Axis )axis;
	rotation->m_degrees = degrees;
	rotation->m_start_degrees = start_degrees;
	rotation->m_functor = functor;
	m_handles.push_back( rotation );
	SteelType var;
	var.set( rotation );
	return var;
}

SteelType AnimationState::createStretch( SteelType::Functor width_f, SteelType::Functor height_f) {
	Stretch* stretch = new Stretch();
	stretch->m_width_functor = width_f;
	stretch->m_height_functor = height_f;

	m_handles.push_back( stretch );
	SteelType var;
	var.set( stretch );
	return var;
}

SteelType AnimationState::syncTo( SteelType::Handle hTask, SteelType::Handle hWithTask ) {
	Task* task = Steel::GrabHandle<Task*>( hTask );
	Task* withTask = Steel::GrabHandle<Task*>( hWithTask );
	task->SyncTo( withTask );
	SteelType var;
	var.set( withTask );
	return var;
}

SteelType AnimationState::doAfter( double after, SteelType::Functor f ) {
	FunctionTask * task = new FunctionTask( *this );
	task->init( after, f );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::doFunction( SteelType::Functor f ) {
	return doAfter( 0, f );
}

SteelType AnimationState::closestCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
		
	
	// This isn't a true "closest" as it doesn't consider the middle points
	bool targetOnLeft = false;
	bool targetAbove = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	if(targetPt.y < refPt.y)
		targetAbove = true;
	
	if(targetOnLeft && targetAbove)
		ret.set(Locale::BOTTOM_LEFT);
	else if(targetOnLeft && !targetAbove)
		ret.set(Locale::TOP_LEFT);
	else if(targetAbove)
		ret.set(Locale::BOTTOM_RIGHT);
	else{
		ret.set(Locale::TOP_RIGHT);
	}
	
	return ret;
}

SteelType AnimationState::nearCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
	
	
	bool targetOnLeft = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	switch(corner){
		case Locale::BOTTOM_LEFT:
			ret.set(Locale::BOTTOM_RIGHT);
			break;
		case Locale::MIDDLE_LEFT:
			ret.set(Locale::MIDDLE_RIGHT);
			break;
		case Locale::TOP_LEFT:
			ret.set(Locale::TOP_RIGHT);
			break;
	}
	
	return ret;
}

SteelType AnimationState::farCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
	
	
	bool targetOnLeft = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	switch(corner){
		case Locale::BOTTOM_RIGHT:
			ret.set(Locale::BOTTOM_LEFT);
			break;
		case Locale::MIDDLE_RIGHT:
			ret.set(Locale::MIDDLE_LEFT);
			break;
		case Locale::TOP_RIGHT:
			ret.set(Locale::TOP_LEFT);
			break;
	}
	
	return ret;	
}// Returns the corner on the opposite
	


SteelType AnimationState::setSpriteRotation(int sprite, double radians){
	GetSprite(sprite).set_angle(clan::Angle::from_radians(radians));
	return SteelType();
}
SteelType AnimationState::flipSprite(int sprite,bool flip_x, bool flip_y){
	float cur_x, cur_y;
	GetSprite(sprite).get_scale(cur_x,cur_y);
	
	if(flip_x)
		cur_x = -cur_x;
	if(flip_y)
		cur_y = -cur_y;
	
	GetSprite(sprite).set_scale(cur_x,cur_y);
	return SteelType();
}
SteelType AnimationState::scaleSprite(int sprite, double x, double y){
	GetSprite(sprite).set_scale(x,y);
	return SteelType();
}
SteelType AnimationState::setSpriteLocation(int sprite, SteelType::Handle hLocale){
	Locale * pLoc = Steel::GrabHandle<Locale*>( hLocale );
	m_parent.set_sprite_pos( sprite, GetPosition(*pLoc) );
	return SteelType();
}


SteelType AnimationState::rotateSprite( int sprite, SteelType::Handle hRotation ) {
	Rotation * rot = Steel::GrabHandle<Rotation*>( hRotation );
	RotationTask * task = new RotationTask( *this );
#ifndef NDEBUG
	std::cout << "RotationTask created: " << std::hex << task << std::endl;
#endif
	task->init( *rot );
	task->SetSprite( sprite );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::createOrbit(SteelType::Functor radius_functor, SteelType::Functor speed_functor, double start_degrees, double deg, bool clockwise){
	Orbit * orb = new Orbit();
	orb->m_radius_functor = radius_functor;
	orb->m_speed_functor = speed_functor;
	orb->m_degrees = deg;
	orb->m_start_angle = start_degrees;
	orb->m_clockwise = clockwise;
	SteelType var;
	var.set(orb);
	m_handles.push_back(orb);
	return var;
}
SteelType AnimationState::orbitSprite(int sprite,SteelType::Handle hOrbit, SteelType::Handle hLocale){
	Orbit * orb = Steel::GrabHandle<Orbit*>( hOrbit );
	Locale * locale = Steel::GrabHandle<Locale*> ( hLocale );
	OrbitTask * task = new OrbitTask ( *this );
	task->init(*orb, *locale);
	task->SetSprite(sprite);
	SteelType var;
	var.set ( task );
	m_handles.push_back ( task );
	return var;
}
SteelType AnimationState::orbitSpriteTimed(int sprite, SteelType::Handle hOrbit, SteelType::Handle hLocale, double seconds){
	Orbit * orb = Steel::GrabHandle<Orbit*>( hOrbit );
	Locale * locale = Steel::GrabHandle<Locale*> ( hLocale );
	orb->m_duration = seconds;
	TimedOrbitTask * task = new TimedOrbitTask( *this );
	task->init(*orb, *locale);
	task->SetSprite( sprite );
	SteelType var;
	var.set ( task );
	m_handles.push_back(task);
	return var;
}


SteelType AnimationState::rotateSpriteTimed( int sprite, SteelType::Handle hRotation, double seconds ) {
	Rotation * rot = Steel::GrabHandle<Rotation*>( hRotation );
	rot->m_duration = seconds;
	TimedRotationTask * task = new TimedRotationTask( *this );
#ifndef NDEBUG
	std::cout << "TimedRotationTask created: " << std::hex << task << std::endl;
#endif
	task->init( *rot );
	task->SetSprite( sprite );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::stretchSpriteTimed( int sprite, SteelType::Handle hStretch, double seconds ) {
	Stretch * stretch  = Steel::GrabHandle<Stretch*>( hStretch );
	stretch->m_duration = seconds;
	TimedStretchTask * task = new TimedStretchTask( *this );
	task->SetDuration(seconds);
#ifndef NDEBUG
	std::cout << "TimedStretchTask created: " << std::hex << task << std::endl;
#endif
	task->init( *stretch );
	task->SetSprite( sprite );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}



SteelType AnimationState::createShaker( SteelType::Functor magnitude, int flags ) {
	Shaker * shaker = new Shaker();
	shaker->m_functor = magnitude;
	shaker->m_flags = flags;
	m_handles.push_back( shaker );
	SteelType var;
	var.set( shaker );
	return var;
}

SteelType AnimationState::shake( SteelType::Handle hlocale, SteelType::Handle hShaker, double seconds ) {
	Locale loc = *Steel::GrabHandle<Locale*>( hlocale );
	Shaker * shaker = Steel::GrabHandle<Shaker*>( hShaker );
	ShakerTask * task = new ShakerTask( *this );
	task->SetDuration( seconds );
	task->init( *shaker, loc );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::floatCharacter(SteelType::Handle hCharacter,SteelType::Functor float_amt, double time){
	ICharacter * ch = Steel::GrabHandle<ICharacter*>(hCharacter);
	FloatTaskTimed * task = new FloatTaskTimed(*this);
	m_handles.push_back(task);
	task->SetDuration(time);
	task->init(float_amt,ch);
	SteelType var;
	var.set(task);
	return var;
}

SteelType AnimationState::createFade( SteelType::Functor functor ) {
	Fade * fade = new Fade();
	fade->m_functor = functor;
	m_handles.push_back( fade );
	SteelType var;
	var.set( fade );
	return var;
}

SteelType AnimationState::fadeSprite( int sprite, SteelType::Handle hFade, double seconds ) {
	Fade * fade = Steel::GrabHandle<Fade*>( hFade );
	FadeTask * task = new FadeTask( *this );
	task->SetSprite( sprite );
	task->SetDuration( seconds  );
	task->SetFunctor( fade->m_functor );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::createSimpleColorizer( double r, double g, double b, double seconds ) {
	return SteelType();
}

SteelType AnimationState::createColorizer( SteelType::Functor r_func, SteelType::Functor g_func, SteelType::Functor b_func ) {
	Colorizer * col = new Colorizer();
	SteelType var;
	var.set( col );
	col->m_red = r_func;
	col->m_green = g_func;
	col->m_blue = b_func;
	m_handles.push_back( col );
	return var;
}

SteelType AnimationState::colorizeSprite( int sprite, SteelType::Handle hColorizer, double seconds ) {
	ColorizeTask * task = new ColorizeTask( *this );

	task->init( *Steel::GrabHandle<Colorizer*>( hColorizer ) );
	task->SetSprite(sprite);
	task->SetDuration(seconds);
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::startAfter( SteelType::Handle htask, SteelType::Handle hnexttask ) {
	Task* task = Steel::GrabHandle<Task*>( htask );
	Task* next = Steel::GrabHandle<Task*>( hnexttask );
	task->SetNextTask( next );
	SteelType var;
	var.set( next );
	return var;
}

SteelType AnimationState::pause( double seconds ) {
#ifndef NDEBUG
	std::cout << "Pausing for " << seconds << " seconds" << std::endl;
#endif
	clan::System::pause( seconds * 1000.0 );
	return SteelType();
}


SteelType AnimationState::isLeftOf(int sprite, int other_sprite){
  clan::Rectf a = m_parent.get_sprite_rect(sprite);
  clan::Rectf b = m_parent.get_sprite_rect(other_sprite);
  SteelType ret;
  ret.set ( bool(a.get_center().x > b.get_center().x) );
  return ret;
}

SteelType AnimationState::chainTasks( const Steel::SteelArray& array ) {
	Task * lastTask = NULL;
	for( int i = 0; i < array.size(); i++ ) {
		SteelType::Handle htask = ( SteelType::Handle )array[i];
		Task * task = Steel::GrabHandle<Task*>( htask );
		if( lastTask ) {
			lastTask->SetNextTask( task );
		}

		lastTask = task;
	}
	SteelType var;
	var.set( lastTask ); // this way you can wait on the chainTasks for the last one to complete
	return var;
}

SteelType AnimationState::waitFor( SteelType::Handle waitOn ) {
	SteelType val;
	val.set( waitOn );
	Task * pTask = Steel::GrabHandle<Task*>( waitOn );
#ifndef NDEBUG
	//std::cout << "Waiting on:" << pTask->GetName() << std::hex << pTask << std::endl;
#endif
#if 0
	std::cout << "Going to lock ftm" << std::endl;
	m_finished_task_mutex.lock();
	if( m_finished_tasks.find( pTask ) != m_finished_tasks.end() ) {
		std::cout << "Task already finished." << std::endl;
		m_finished_task_mutex.unlock();
		return val;
	}
	m_finished_task_mutex.unlock();
	std::cout << "Unlocked ftm after initial check" << std::endl;
#endif
	if( !pTask->running() ) {
		AddTask( pTask );
	}
	while( !pTask->expired() ) {
	}
	//std::cout << "Done waiting for: " << pTask->GetName() << std::endl;
	return val;
}

AnimationState::Task* AnimationState::get_top_task() const {
	std::unique_lock<std::recursive_mutex> lock(m_task_mutex);
	for(auto it = std::begin(m_tasks); it != std::end(m_tasks); it++){
		if(!(*it)->expired())
			return *it;
	}
	return nullptr;
}

SteelType AnimationState::waitForAll( const Steel::SteelArray& alltasks ) {
	startAll( alltasks ); // start in case some aren't started
	while( active_tasks_left() ) {
		waitFor( get_top_task() );
	}
	return SteelType();
}

SteelType AnimationState::start( SteelType::Handle hTask ) {
	Task * task = Steel::GrabHandle<Task*>( hTask );
	if( !task->running() && !task->expired() ) {
		AddTask( task );
	}
	SteelType var;
	var.set( task );
	return var;
}

void AnimationState::add_task( AnimationState::Task* task ) {
	m_task_mutex.lock();
	if(!task->expired() && !task->running()){
#ifndef NDEBUG
		//std::cout << "Adding task: " << task->GetName() << '@' << std::hex << task <<" to " << std::dec << m_tasks.size() << " existing tasks" << std::endl;
#endif
		task->start( m_pInterpreter );
		m_tasks.push_back(task);
	}
	m_task_mutex.unlock();
}


SteelType AnimationState::startAll( const Steel::SteelArray& alltasks ) {
	for( int i = 0; i < alltasks.size(); i++ ) {
		Task * task = Steel::GrabHandle<Task*>( alltasks[i] );
		if( !task->running() && !task->expired() ) {
			add_task(task);
		}
	}
	return SteelType();
}





void AnimationState::Finish() { // Hook to clean up or whatever after being popped
	std::cout << "AnimationState::Finish" << std::endl;

	m_task_mutex.lock();
	m_tasks.clear();
	m_task_mutex.unlock();	
	for( std::list<SteelType::IHandle*>::const_iterator it = m_handles.begin(); it != m_handles.end(); it++ ) {
		//std::cout << std::hex << ( long )*it << std::endl;
		delete *it;
	}
	m_handles.clear();

}



void AnimationState::Darken( int mode, float r, float g, float b, float a ) {
	m_parent.SetDarkMode( mode, r, g, b, a );
}

void AnimationState::ClearDark( int mode ) {
	m_parent.ClearDarkMode( mode );
}







/**
 *  Tasks
 *
 *
 * */

AnimationState::Locale::Locale() {

}

AnimationState::Locale::~Locale() {
}

AnimationState::Locale::Locale( AnimationState::Locale::Type type, AnimationState::Locale::Corner corner ) {
	m_type = type;
	m_corner = corner;
}

void AnimationState::Locale::SetGroup( ICharacterGroup* pGroup ) {
	m_target.as_group = pGroup;
}

void AnimationState::Locale::SetIChar( ICharacter* pChar ) {
	m_target.as_char = pChar;
}

void AnimationState::Locale::SetSprite( BattleState::SpriteTicket sprite ) {
	m_target.as_sprite = sprite;
}

void AnimationState::Locale::SetOffset( const clan::Point& offset ) {
	m_offset = offset;
}

void AnimationState::Locale::SetType( AnimationState::Locale::Type type ) {
	m_type = type;
}

void AnimationState::Locale::SetCorner( AnimationState::Locale::Corner corner ) {
	m_corner = corner;
}

ICharacter* AnimationState::Locale::GetChar() const {
	assert( m_type == CHARACTER || m_type == CHARACTER_LOCUS );
	return m_target.as_char;
}

ICharacterGroup* AnimationState::Locale::GetGroup() const {
	assert( m_type == GROUP );
	return m_target.as_group;
}

BattleState::SpriteTicket AnimationState::Locale::GetSprite() const {
	assert( m_type == SPRITE );
	return m_target.as_sprite;
}

clan::Sprite AnimationState::GetSprite( BattleState::SpriteTicket sprite ) {
	return m_parent.get_sprite( sprite );
}

float AnimationState::Task::percentage() const {
	float perc = 0.0f;
	if( m_sync_task ) {
		perc =  m_sync_task->percentage();
	} else {
		perc = _percentage();
	}
	// Multiplying a vector by zero moves it to the corner..
	if( perc == 0.0f ) {
		perc = 0.000001f;
	}
	
	if( perc > 1.0f){
		perc = 1.0f;
	}

	return perc;
}


clan::Pointf AnimationState::Task::get_position( const AnimationState::Locale& locale) const {
		return m_state.GetPosition(locale);
}
clan::Pointf AnimationState::Task::get_mid_point( const clan::Pointf& start, const clan::Pointf& end, float p ) {
	return start * ( 1.0f - p ) + end * p;
}

// M = S * (1-p) + E * p
// Solve for p
// M / E

clan::Pointf AnimationState::GetPosition( const AnimationState::Locale& locale ) const {
	clan::Rectf rect;
	switch( locale.GetType() ) {
		case Locale::CHARACTER:
			rect = GetCharacterRect( locale.GetChar() );
			break;
		case Locale::CHARACTER_LOCUS:
			rect = m_parent.get_character_locus_rect( locale.GetChar() );
			break;
		case Locale::SPRITE:
			rect = GetSpriteRect( locale.GetSprite() );
			break;
		case Locale::SCREEN:
			rect = IApplication::GetInstance()->GetDisplayRect();
			break;
		case Locale::GROUP:
			rect = GetGroupRect( locale.GetGroup() );
			break;
	}

	clan::Pointf point;

	switch( locale.GetCorner() ) {
		case Locale::TOP_LEFT:
			point =  rect.get_top_left();
			break;
		case Locale::TOP_RIGHT:
			point = rect.get_top_right();
			break;
		case Locale::TOP_CENTER:
			point = rect.get_top_left();
			point.x += rect.get_width() / 2.0f;
			break;
		case Locale::MIDDLE_LEFT:
			point = rect.get_center();
			point.x -= rect.get_width() / 2.0f;
			break;
		case Locale::MIDDLE_RIGHT:
			point = rect.get_center();
			point.x += rect.get_width() / 2.0f;
			break;
		case Locale::CENTER:
			point = rect.get_center();
			break;
		case Locale::BOTTOM_LEFT:
			point = rect.get_bottom_left();
			break;
		case Locale::BOTTOM_RIGHT:
			point = rect.get_bottom_right();
			break;
		case Locale::BOTTOM_CENTER:
			point = rect.get_bottom_left();
			point.x += rect.get_width() / 2.0f;
			break;
	}

	point += locale.GetOffset();
	return point;
}

AnimationState::PathTask::~PathTask() {

}


void AnimationState::PathTask::init( Path* path ) {
	m_path = path;
}


void AnimationState::PathTask::SetCompletion( float completion ) {
	m_path->m_completion = completion;
}

void AnimationState::PathTask::SetFlags( int flags ) {
	m_path->m_flags = flags;
}

void AnimationState::PathTask::SetSpeed( float pixels_per_ms ) {
	m_path->m_speed = pixels_per_ms;
}


void AnimationState::PathTask::SetStart( const AnimationState::Locale& start ) {
	m_path->m_start = start;
}

void AnimationState::PathTask::SetEnd( const AnimationState::Locale& end ) {
	m_path->m_end = end;
}


void AnimationState::PathTask::SetSpeedFunctor( SteelType::Functor functor ) {
	m_path->m_speed_functor = functor;
}

void AnimationState::PathTask::_start( SteelInterpreter* pInterpreter ) {
	m_cur_pos = get_position( m_path->m_start );
	m_percentage_so_far = 0.0f;
	m_start_time = clan::System::get_time();
}

void AnimationState::PathTask::update( SteelInterpreter* pInterpreter ) {
	assert( m_path );
	float speed = m_path->m_speed;
	if( m_path->m_speed_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		speed = ( double )m_path->m_speed_functor->Call( pInterpreter, params );
	}
	clan::Pointf start = get_position( m_path->m_start );
	clan::Pointf true_end = get_position( m_path->m_end );
	
	//(-dy, dx) and (dy, -dx).

	//clan::Pointf end = (start + true_end) * m_completion;
	clan::Pointf end = true_end;

	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}
	
	clan::Vec2<float> vec(end.x-start.x,end.y-start.y);
	vec.normalize();
	vec *= speed;
	float elapsed = float( clan::System::get_time() - m_start_time );
	

	float p = percentage();
	bool done = false;

	m_cur_pos += vec * elapsed;

	double dis = 0.0f;
	if( m_path->m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		dis = ( double )m_path->m_functor->Call( pInterpreter, params );
	}
	// TODO: Adjust Y component by function output
	clan::Pointf adj_pos = m_cur_pos;
	adj_pos += clan::Pointf( -( end.y - start.y ), ( end.x - start.x ) ).normalize() * float(dis);
	m_state.SetSpritePos( m_sprite, adj_pos );
	m_cur_pos = adj_pos;

	m_percentage_so_far = m_cur_pos.distance( start ) / end.distance(start);
	if( done ) m_percentage_so_far = 1.0f;
}

bool AnimationState::PathTask::finished() {
	return percentage() >= 1.0f;
}

float AnimationState::PathTask::_percentage() const {
	return m_percentage_so_far/ m_path->m_completion;
}


void AnimationState::TimedPathTask::init( Path* path ) {
	m_path = path;
}

void AnimationState::TimedPathTask::_start( SteelInterpreter* pInterpreter ) {
}

void AnimationState::TimedPathTask::update( SteelInterpreter* pInterpreter ) {
	assert( m_path );
	clan::Pointf start = get_position( m_path->m_start );
	clan::Pointf end = get_position( m_path->m_end );
	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}
	
	float speed = start.distance(end) / duration();
	
	clan::Vec2<float> vec(end.x-start.x,end.y-start.y);
	vec.normalize();
	vec *= speed;
	float elapsed = float( clan::System::get_time() - m_start_time );
	
	clan::Pointf cur_pos = m_state.GetSpriteRect(m_sprite).get_center();
	cur_pos += vec * elapsed;
	
	double dis = 0.0f;
	if( m_path->m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		dis = ( double )m_path->m_functor->Call( pInterpreter, params );
	}
	clan::Pointf adj_pos = cur_pos;
	adj_pos += clan::Pointf( -( end.y - start.y ), ( end.x - start.x ) ).normalize() * float(dis);
	
	
	m_state.SetSpritePos( m_sprite, adj_pos );
}

void AnimationState::TimedPathTask::SetStart( const Locale& start ) {
	m_path->m_start = start;
}

void AnimationState::TimedPathTask::SetFlags( int flags ) {
	m_path->m_flags = flags;
}

void AnimationState::TimedPathTask::SetEnd( const Locale& end ) {
	m_path->m_end = end;
}

void AnimationState::TimedStretchTask::init( const Stretch& stretch ) {
	m_stretch = stretch;
}

void AnimationState::TimedStretchTask::_start( SteelInterpreter* pInterpreter ) {
	m_start_time = clan::System::get_time();
	TimedTask::_start(pInterpreter);
}

void AnimationState::TimedStretchTask::update( SteelInterpreter* pInterpreter ) {
	//TimedTask::update();
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float width = ( double )m_stretch.m_width_functor->Call( pInterpreter, params );
	float height = ( double )m_stretch.m_height_functor->Call ( pInterpreter, params );
	clan::Sprite sprite = m_state.GetSprite( m_sprite );
	sprite.set_scale(width,height);
}



void AnimationState::TimedStretchTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_scale(1.0,1.0);
}

void AnimationState::FloatTaskTimed::init( SteelType::Functor float_amt, ICharacter* iChar ) {
	m_float_functor = float_amt;
	m_character = iChar;
}

void AnimationState::FloatTaskTimed::SetDuration( float duration ) {
	m_duration = duration;
}

void AnimationState::FloatTaskTimed::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	double height = 	m_float_functor->Call(pInterpreter,params);
	m_state.SetShadowOffset(m_character,clan::Pointf(0.0f,height));
}

void AnimationState::FloatTaskTimed::_start( SteelInterpreter* pInterpreter ) {
}


void AnimationState::FloatTaskTimed::cleanup() {
	StoneRing::AnimationState::Task::cleanup();
}



void AnimationState::RotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
}

void AnimationState::RotationTask::_start( SteelInterpreter* pInterpreter ) {
	m_original_degrees = m_state.GetSprite(m_sprite).get_angle().to_degrees();
	m_completion_degrees = 0.0f;
	m_degrees = m_rotation.m_start_degrees + m_original_degrees;
	m_last_time = clan::System::get_time();
}

void AnimationState::RotationTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float speed = ( double )m_rotation.m_functor->Call( pInterpreter, params );
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees +=  delta ;
	m_completion_degrees += fabs(delta);
#if 1 // This can kinda cause an over-rotation... 
	if( m_completion_degrees > m_rotation.m_degrees ){
	    if(speed >= 0.0){
	      m_degrees = m_rotation.m_degrees + m_original_degrees;
	    }else{
	      m_degrees = m_original_degrees - m_rotation.m_degrees;
	    }
	}
#endif
	clan::Sprite sprite = m_state.GetSprite( m_sprite );
	switch( m_rotation.m_axis ) {
		case Rotation::PITCH:
			//sprite.rotate_pitch( clan::Angle::from_degrees( delta ) );
			break;
		case Rotation::YAW:
			//sprite.rotate_yaw( clan::Angle::from_degrees( delta ) );
			break;
		case Rotation::ROLL:
			sprite.set_angle( clan::Angle::from_degrees( m_degrees ) );
			break;
	}
	m_last_time = clan::System::get_time();
}

bool AnimationState::RotationTask::finished() {
	return percentage() >= 1.0f;
}


float AnimationState::RotationTask::_percentage() const {
	return  m_completion_degrees / m_rotation.m_degrees;
}

void AnimationState::RotationTask::cleanup() {
#if 0 
	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			//m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( m_original_degrees ) );
			break;
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( 0.0f ) );
			break;
	}
#endif
}


void AnimationState::TimedRotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
}

void AnimationState::TimedRotationTask::_start( SteelInterpreter* pInterpreter ) {
	m_degrees = 0;
	TimedTask::_start(pInterpreter);
}
void AnimationState::TimedRotationTask::update( SteelInterpreter* pInterpreter ) {
	float speed = 0.1f;
	if( m_rotation.m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		speed = ( double )m_rotation.m_functor->Call( pInterpreter, params );
	}
	
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += abs( delta );
	if( m_degrees > m_rotation.m_degrees )
		delta -=  m_degrees - m_rotation.m_degrees; // lessen the delta by how much we overshot
	
#ifndef NDEBUG
	//std::cout << "Angle is " << angle << std::endl;
#endif
	switch( m_rotation.m_axis ) {
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( angle ) );
			break;
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( m_degrees ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( angle ) );
			break;
	}
}
bool AnimationState::TimedRotationTask::finished() {
	return TimedTask::finished();
}

void AnimationState::TimedRotationTask::cleanup() {
	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( 0.0f ) );
			break;
	}
}


void AnimationState::OrbitTask::init(const Orbit & orbit, const Locale& locale){
	m_orbit = orbit;
	m_origin = locale;
}
void AnimationState::OrbitTask::_start(SteelInterpreter* pInterpreter){
	m_completion_degrees = 0.0f;
	m_last_radius = 0.0f;
	m_degrees = m_orbit.m_start_angle;
	m_state.SetSpritePos(m_sprite,m_state.GetPosition(m_origin));
	m_last_time = clan::System::get_time();	
}
void AnimationState::OrbitTask::update(SteelInterpreter* pInterpreter){
	//const clan::Pointf origin = m_state.GetPosition(m_origin);
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	assert(m_orbit.m_radius_functor);
	assert(m_orbit.m_speed_functor);
	float radius = ( double ) m_orbit.m_radius_functor->Call(pInterpreter, params );
	float speed = ( double )m_orbit.m_speed_functor->Call( pInterpreter, params );
	float delta = speed * float( clan::System::get_time() - m_last_time );
	//std::cout << "R=" << radius << " p= " << percentage() << std::endl;
	m_degrees += delta ;
	m_completion_degrees += fabs(delta);
	if( m_completion_degrees > m_orbit.m_degrees ){
		delta -=  m_degrees - m_orbit.m_degrees; // lessen the delta by how much we overshot
		m_degrees = m_orbit.m_degrees;
	}
	
	// First we have to calculate an origin, we derive it 
	// This is crazy shit
	float angle = ( clan::PI / 180.0f ) * m_degrees;
	clan::Pointf cpoint( cos( angle ), sin( angle ) );
	//		
	clan::Pointf current = m_state.GetSpriteRect(m_sprite).get_center();
	//clan::Pointf origin = current - cpoint * m_last_radius;
	clan::Pointf origin = m_state.GetPosition(m_origin);
	
	// Now we have to calculate our new point from our derived origin
	clan::Pointf new_point = origin +  cpoint * radius; // Here's our new orbit point
	
	// Now we create a vector from the current to the new_point
	clan::Vec2<float> radius_change_vec(new_point.x-current.x,new_point.y-current.y);
	
	current += radius_change_vec; // Move by vector (note: intentionally not normalized)
	
	// Now to calculate the orbit motion vector
	clan::Vec2<float> orbit_vector( -sin(angle), cos(angle) );
	//orbit_vector.normalize();
	
	current += orbit_vector * delta;
	
	m_last_radius = radius;	
	m_state.SetSpritePos(m_sprite,current);
	m_last_time =  clan::System::get_time();
}
float AnimationState::OrbitTask::_percentage() const {
	return m_completion_degrees / m_orbit.m_degrees;
}
bool AnimationState::OrbitTask::finished(){
	return (m_completion_degrees >= m_orbit.m_degrees);
}
void AnimationState::OrbitTask::cleanup(){
}
void AnimationState::TimedOrbitTask::init( const AnimationState::Orbit& rot, const AnimationState::Locale& origin ) {
	m_orbit = rot;
	m_locale = origin;
}

void AnimationState::TimedOrbitTask::_start( SteelInterpreter* pInterpreter) {
	m_degrees = m_orbit.m_start_angle;
}

void AnimationState::TimedOrbitTask::update( SteelInterpreter* pInterpreter ) {
	float speed = 0.1f;
	const clan::Pointf origin = m_state.GetPosition(m_locale);	

	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	assert(m_orbit.m_speed_functor);
	assert(m_orbit.m_radius_functor);
	speed = ( double )m_orbit.m_speed_functor->Call( pInterpreter, params );
	
	float radius = (double)m_orbit.m_radius_functor->Call( pInterpreter, params );
	
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += abs( delta );

	float angle = ( clan::PI / 180.0f ) * m_degrees;
	clan::Pointf cpoint( cos( angle ), sin( angle ) );
	clan::Pointf current = origin +  cpoint * radius; // C + R * (cos A, sin A)		
	m_state.SetSpritePos(m_sprite,current);		
}

void AnimationState::TimedOrbitTask::cleanup() {
	StoneRing::AnimationState::Task::cleanup();
}

bool AnimationState::TimedOrbitTask::finished() {
	return percentage() >= 1.0;
}

void AnimationState::TimedStretchTask::SetDuration( float duration ) {
	m_duration = duration;
}



void AnimationState::ShakerTask::init( const Shaker& shaker, const Locale& locale ) {
      
	m_locale = locale;
	m_shaker = shaker;
	m_starting_offset = m_state.GetSpriteOffset( m_locale.GetSprite() );	
}

void AnimationState::ShakerTask::_start( SteelInterpreter* pInterpreter ) {
	TimedTask::_start(pInterpreter);
	pick_rand_dir();
}

void AnimationState::ShakerTask::pick_rand_dir() {
	do {
		int d = rand() % 4;
		if( d == 0 )
			m_dir = Direction::NORTH;
		else if( d == 1 )
			m_dir = Direction::EAST;
		else if( d == 2 )
			m_dir = Direction::WEST;
		else m_dir = Direction::SOUTH;
	} while( !dir_legal() );
}

bool AnimationState::ShakerTask::dir_legal() const {
	if( m_shaker.m_flags & Shaker::NO_HORIZONTAL && ( m_dir == Direction::EAST || m_dir == Direction::WEST ) )
		return false;
	if( m_shaker.m_flags & Shaker::NO_VERTICAL && ( m_dir == Direction::NORTH || m_dir == Direction::SOUTH ) )
		return false;

	return true;
}

void AnimationState::ShakerTask::update( SteelInterpreter* pInterpreter ) {
	//TimedTask::update(pInterpreter);
	float motion = 0.0f;
	if( m_shaker.m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		motion = ( double )m_shaker.m_functor->Call( pInterpreter, params );
	}
	// TODO: Call a method which calls a method on the battle state
	// that sets an offset to either a sprite, a group, or the screen.
	clan::Vec2<float> dir = clan::Vec2<float>(m_dir.ToScreenVector()) * motion;
	clan::Pointf dir_pt( dir.x, dir.y );
	switch( m_locale.GetType() ) {
		case Locale::SPRITE:
			m_state.SetSpriteOffset( m_locale.GetSprite(), m_starting_offset + dir_pt );
			break;
		case Locale::CHARACTER:
			m_state.SetSpriteOffset( m_state.GetSpriteForChar( m_locale.GetChar() ),
						 m_starting_offset + dir_pt );
			break;
		case Locale::GROUP:
			m_state.SetGroupOffset( m_locale.GetGroup(), m_starting_offset + dir_pt );
			break;
	}
/*
	if(m_osc){
		pick_opposite_dir();
	}else{
		pick_rand_dir();
	}
	m_osc = !m_osc;
	*/
        pick_rand_dir();
}

void AnimationState::ShakerTask::pick_opposite_dir() {
	m_dir = m_dir.opposite();
}



void AnimationState::ShakerTask::cleanup(){
	switch( m_locale.GetType() ) {
		case Locale::SPRITE:
			m_state.SetSpriteOffset( m_locale.GetSprite(), clan::Pointf(0.0f,0.0f) );
			break;
		case Locale::CHARACTER:
			m_state.SetSpriteOffset( m_state.GetSpriteForChar(m_locale.GetChar()),clan::Pointf(0.0f,0.0f) );
			break;
		case Locale::GROUP:
			m_state.SetGroupOffset( m_locale.GetGroup(), clan::Pointf(0.0f,0.0f) );
			break;
	}	
}

void AnimationState::FadeTask::SetDuration( float duration ) {
	m_duration = duration;
}

void AnimationState::FadeTask::_start( SteelInterpreter* pInterpreter ) {
	TimedTask::_start(pInterpreter);
}

void AnimationState::FadeTask::update( SteelInterpreter* pInterpreter ) {
	TimedTask::update();
	float alpha = 1.0f - percentage();
	if(  m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		alpha = ( double )m_functor->Call( pInterpreter, params );
	}
	m_state.GetSprite( m_sprite ).set_alpha( alpha );
}


void AnimationState::FadeTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_alpha( 1.0f );
}

void AnimationState::ColorizeTask::init( const Colorizer& colorizer ) {
	m_colorizer = colorizer;
}

void AnimationState::ColorizeTask::SetDuration( float duration ) {
	m_duration = duration;
}


void AnimationState::ColorizeTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float r = ( double )m_colorizer.m_red->Call( pInterpreter, params );
	float g = ( double )m_colorizer.m_green->Call( pInterpreter, params );
	float b = ( double )m_colorizer.m_blue->Call( pInterpreter, params );

	m_state.GetSprite( m_sprite ).set_color( clan::Colorf( r, g, b,	m_state.GetSprite( m_sprite ).get_alpha()) );
}

void AnimationState::ColorizeTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_color( clan::Colorf( 1.0f, 1.0f, 1.0f ) );
}

void AnimationState::DarkenTask::init( int mode, float duration, SteelType::Functor r,
																																							SteelType::Functor g, SteelType::Functor b, SteelType::Functor a ) {
	m_mode = mode;
	m_duration = duration;
	m_red = r;
	m_green = g;
	m_blue = b;
	m_alpha = a;
}

void AnimationState::DarkenTask::_start( SteelInterpreter* pInterpreter ) {
}

void AnimationState::DarkenTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float r = ( double )m_red->Call( pInterpreter, params );
	float g = ( double )m_green->Call( pInterpreter, params );
	float b = ( double )m_blue->Call( pInterpreter, params );
	float a = ( double )m_alpha->Call( pInterpreter, params );

	m_state.Darken( m_mode, r, g, b, a );
}

void AnimationState::DarkenTask::cleanup() {
	m_state.ClearDark( m_mode );
}


void AnimationState::FunctionTask::init( double duration, SteelType::Functor f ) {
	SetDuration( duration );
	m_functor = f;
}

void AnimationState::FunctionTask::_start( SteelInterpreter* pInterpreter ) {
}

void AnimationState::FunctionTask::update( SteelInterpreter* ) {

}

void AnimationState::FunctionTask::cleanup() {
	//m_functor->Call( pInterpreter, SteelType::Container() );
}






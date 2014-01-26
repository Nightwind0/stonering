#include "LoadingState.h"
#include "GraphicsManager.h"

using StoneRing::LoadingState;
using StoneRing::GraphicsManager;

LoadingState::LoadingState() {
}
LoadingState::~LoadingState() {
}


void LoadingState::init( std::function< void() >& func ) {
	m_loading_func = func;
	m_sprite = GraphicsManager::CreateMonsterSprite("Green Tea Mochi", "idle");
}

void LoadingState::on_thread_finished() {
	m_bDone = true;
}

void LoadingState::run_function() {
	m_loading_func();
	on_thread_finished();
}


bool LoadingState::IsDone() const {
	return m_bDone;
}

void LoadingState::Draw( const clan::Rect &screenRect, clan::Canvas& GC ) {
	if(m_bDraw /* check m_bDraw and animate something. Draw "Loading..." ? */){
		GC.fill_rect(screenRect,clan::Colorf(0.0f,0.0f,0.0f));
		m_sprite.update(clan::System::get_time()-m_last_update);
		m_sprite.draw(GC,screenRect.get_center().x, screenRect.get_center().y);
	}
	m_last_update = clan::System::get_time();
}

bool LoadingState::LastToDraw() const {
	return m_bDraw;
}


bool LoadingState::DisableMappableObjects() const {
	return true;
}

void LoadingState::MappableObjectMoveHook() {
}

void LoadingState::Start() {
	m_bDone = false;
	m_bDraw = false;
	// using std::placeholders::_1;
    std::function<void()> thread_f = std::bind( &LoadingState::run_function, this );
   
	m_thread = std::thread(thread_f);
	// Start a timer
	m_timer.func_expired().set(this,&LoadingState::OnTimer);
	m_timer.start(1000,false);
	m_last_update = clan::System::get_time();
}
void LoadingState::Finish() {
	m_thread.join();
}

void LoadingState::OnTimer() {
	// start drawing
	m_bDraw = true;
}


#include "LoadingState.h"
#include "GraphicsManager.h"

using StoneRing::LoadingState;
using StoneRing::GraphicsManager;

LoadingState::LoadingState() {
}
LoadingState::~LoadingState() {
}


void LoadingState::init( std::function< void() >& func ) {
	m_fade_out = false;
	m_loading_func = func;
	m_sprite = GraphicsManager::CreateMonsterSprite("Stoked Green Tea Mochi", "idle");
}

void LoadingState::on_thread_finished() {
	m_timer.stop();
	if(m_bDraw)
		m_fade_out = true;
	else
		m_bDone = true;
	m_fadeout_start = clan::System::get_time();
}

void LoadingState::run_function() {
	m_loading_func();
	on_thread_finished();
}


bool LoadingState::IsDone() const {
	return m_bDone;
}

void LoadingState::Draw( const clan::Rect &screenRect, clan::Canvas& GC ) {
	
	double alpha = 1.0f;
	if (m_fade_out){
		alpha = 1.0f - float(clan::System::get_time() - m_fadeout_start) / 500.0f; // fade out over 1/2 a second
		if(alpha <= 0.0f) {
			alpha = 0.0f;
			m_bDone = true;
		}
	}
	if(m_bDraw){
		GC.fill_rect(screenRect,clan::Colorf(0.0f,0.0f,0.0f));
		m_sprite.update(clan::System::get_time()-m_last_update);
		m_sprite.set_alpha(alpha);
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
	m_timer.start(300,false);
	m_last_update = clan::System::get_time();
}
void LoadingState::Finish() {
	m_thread.join();
}

void LoadingState::OnTimer() {
	// start drawing
	m_bDraw = true;
}


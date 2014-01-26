#ifndef _LOADING_STATE_H
#define _LOADING_STATE_H
#include <functional>
#include <future>
#include "State.h"
#include <ClanLib/core.h>

namespace StoneRing {
	
	class LoadingState : public State {
	public:
		LoadingState();
		virtual ~LoadingState();
		void init(std::function<void()>& func);
		virtual bool IsDone() const;		
		virtual void Draw( const clan::Rect &screenRect, clan::Canvas& GC );
		virtual bool LastToDraw() const;
		virtual bool AcceptInput() const { return true; }
		virtual bool DisableMappableObjects() const;
		virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
		virtual void Start();
		virtual void Finish(); // Hook to clean up or whatever after being popped	
		void OnTimer();
	private:
		bool m_bDone;
		bool m_bDraw;
		void run_function();
		void on_thread_finished();
		uint64_t m_last_update;
		std::function<void()> m_loading_func;
		std::thread m_thread;
		clan::Timer m_timer;
		clan::Sprite m_sprite;
	};
}
#endif
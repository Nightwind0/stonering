#ifndef SR2_STATE_H
#define SR2_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#ifndef _WINDOWS_
#include "steel/SteelInterpreter.h"
#include "steel/SteelType.h"
#else
#include "SteelInterpreter.h"
#include "SteelType.h"
#endif
#include "IApplication.h"
#include <thread>
#include <queue>
#include <mutex>

using Steel::SteelInterpreter;

namespace StoneRing {

class State {
public:
	struct Event {  
		IApplication::Button m_buttonEvent;
		struct AxisEvent {
			IApplication::Axis m_axis;
			IApplication::AxisDirection m_dir;
			float m_pos;
		}m_axisEvent;
		clan::InputEvent m_keyEvent;            
		bool m_up;
		uint m_key_state;
		clan::Point m_mouse_pos;
		IApplication::MouseButton m_button;
		enum EventType { BUTTON, AXIS, KEY, KICK, MOUSE_UP, MOUSE_DOWN, MOUSE_DBL, MOUSE_MOVE };
		EventType m_type;            
	};	
	virtual bool IsDone() const = 0;
	virtual bool Threaded() const { return false; }
	virtual void Covered() {} // called when another state is pushed over this one

	virtual void Draw( const clan::Rect &screenRect, clan::Canvas& GC ) = 0;
	virtual bool LastToDraw() const = 0; // Should we continue drawing more states?
	virtual bool AcceptInput() const { return true; }
	virtual bool DisableMappableObjects() const = 0; // Should the app move the MOs?
	virtual void Update() = 0; // Do stuff right after the mappable object movement
	virtual void Start() = 0;
	virtual void SteelInit( SteelInterpreter * );
	virtual void SteelCleanup( SteelInterpreter * );
	virtual void Finish() = 0; // Hook to clean up or whatever after being popped
	void ProcessEvents();
    void EnqueueEvent(const Event& event);    
	void SteelConst( SteelInterpreter*, const std::string &name, int value );
	void SteelConst( SteelInterpreter*, const std::string &name, double value );
protected:
	// Handle raw key events
	virtual void HandleKeyDown( const clan::InputEvent &key ) {}
	virtual void HandleKeyUp( const clan::InputEvent &key ) {}
	// Handle joystick / key events that are processed according to mappings
	virtual void HandleButtonUp( const IApplication::Button& button ) {}
	virtual void HandleButtonDown( const IApplication::Button& button ) {}
	virtual void HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos ) {}
	virtual void HandleMouseUp( const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ) {}
	virtual void HandleMouseDown( const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ) {}
	virtual void HandleDoubleClick( const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ) {}
	virtual void HandleMouseMove( const clan::Point& pos, uint key_state ) {}	
private:
	SteelInterpreter m_interpreter;
	std::mutex m_queue_mutex;
    std::queue<Event> m_event_queue;	
};

inline void State::SteelInit( SteelInterpreter* ) {
}

inline void State::SteelCleanup( SteelInterpreter* ) {
}

inline void State::SteelConst( SteelInterpreter* pInterpreter, const std::string &name, int value ) {
	SteelType val;
	val.set( value );
	pInterpreter->declare_const( name, val );
}

inline void State::SteelConst( SteelInterpreter* pInterpreter, const std::string &name, double value ) {
	SteelType val;
	val.set( value );
	pInterpreter->declare_const( name, val );
}

}


#endif





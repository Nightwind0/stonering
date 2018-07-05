#include "State.h"

namespace StoneRing {
    
    void State::EnqueueEvent(const StoneRing::State::Event& event)
    {
        m_queue_mutex.lock();
        m_event_queue.push(event);
        m_queue_mutex.unlock();
    }
   
    void State::ProcessEvents()
    {
        if(!IsDone()){
            m_queue_mutex.lock();
            if(!m_event_queue.empty()){
                const Event event = m_event_queue.front();
                switch(event.m_type){
                    case Event::KICK:
                        //Kick(); // TODO: WTF was Kick for??
                        break;
                    case Event::BUTTON:
                        if(event.m_up)
                            HandleButtonUp(event.m_buttonEvent);
                        else
                            HandleButtonDown(event.m_buttonEvent);
                        break;
                    case Event::AXIS:
                        HandleAxisMove(event.m_axisEvent.m_axis,event.m_axisEvent.m_dir,event.m_axisEvent.m_pos);
                        break;
                    case Event::KEY:                        
                        if(event.m_up)
                            HandleKeyUp(event.m_keyEvent);
                        else
                            HandleKeyDown(event.m_keyEvent);
                        break;
                        // TODO: Handle new types (Mouse)
                }

                m_event_queue.pop();              
            }
            m_queue_mutex.unlock();
        }
    }
}
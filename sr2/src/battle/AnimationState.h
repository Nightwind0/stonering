#ifndef __SR2_ANIMATION_STATE_H
#define __SR2_ANIMATION_STATE_H


#include "sr_defines.h"
#include "State.h"
#include "SteelRunner.h"
#include "BattleState.h"
#include "Direction.h"
#include <set>
#include <mutex>
#include <atomic>

namespace StoneRing
{


    class BattleState;
    class Animation;
    class Phase;
    class ICharacterGroup;
    class ICharacter;


    class AnimationState : public State
    {
    public:
  //      AnimationState(BattleState& parent, ICharacterGroup* pCasterGroup, ICharacterGroup* pTargetGroup, ICharacter* pCaster, ICharacter * pTarget);
		AnimationState(BattleState& parent);
		
        virtual ~AnimationState();

		void Init(SteelType::Functor pFunctor);

		virtual bool IsDone() const;
		virtual void HandleKeyDown(const clan::InputEvent &key);
		virtual void HandleKeyUp(const clan::InputEvent &key);
		virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
		virtual bool LastToDraw() const; // Should we continue drawing more states?
		virtual bool DisableMappableObjects() const; // Should the app move the MOs?
		virtual void Update(); // Do stuff right after the mappable object movement
		virtual void Start();
		virtual void SteelInit      (SteelInterpreter *);
		virtual void SteelCleanup   (SteelInterpreter *);
		virtual void Finish(); // Hook to clean up or whatever after being popped
		virtual bool Threaded()const { return true; }
		void WaitFinishedEvent();
		void FunctorCompleted();
		
		clan::Rectf GetCharacterRect(ICharacter* ichar)const;
		clan::Rectf GetGroupRect(ICharacterGroup* ichar)const;
		clan::Rectf GetSpriteRect(BattleState::SpriteTicket sprite)const;
		clan::Sprite GetSprite(BattleState::SpriteTicket sprite);
		BattleState::SpriteTicket AddSprite(clan::Sprite sprite);
		void SetSpritePos(BattleState::SpriteTicket sprite,const clan::Pointf& pt);
		void SetSpriteOffset(BattleState::SpriteTicket sprite, const clan::Pointf& pt);
		clan::Pointf GetSpriteOffset(BattleState::SpriteTicket sprite)const;
		void SetShadowOffset(ICharacter *ichar, const clan::Pointf& pt);
		void SetGroupOffset(ICharacterGroup* igroup, const clan::Pointf& pt);
		clan::Pointf GetGroupOffset(ICharacterGroup* igroup)const;
		BattleState::SpriteTicket GetSpriteForChar(ICharacter* iChar);
		void Darken(int mode, float r,float g, float b, float a);
		void ClearDark(int mode);
	
    private:
		void draw(const clan::Rect& screenRect, clan::Canvas& GC);
		void draw_functor(const clan::Rect& screenRect, clan::Canvas& GC);

        //void move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage);
		
		class Locale: public SteelType::IHandle {
		public:			
			enum Type {
				CHARACTER,
				CHARACTER_LOCUS,
				GROUP,
				SPRITE,
				SCREEN
			};
			
			enum Corner {
				TOP_LEFT,
				TOP_RIGHT,
				TOP_CENTER,
				MIDDLE_LEFT,
				MIDDLE_RIGHT,
				CENTER,
				BOTTOM_LEFT,
				BOTTOM_RIGHT,
				BOTTOM_CENTER
				/*AWAY,
				TOWARD*/
			};
			Locale();
			Locale(Type type, Corner corner);
			virtual ~Locale();			
			void SetOffset(const clan::Point& offset);
			void SetIChar(ICharacter* pChar);
			void SetGroup(ICharacterGroup* pGroup);
			void SetSprite(BattleState::SpriteTicket sprite);
			void SetType(Type type);
			void SetCorner(Corner corner);
			Type GetType()const { return m_type; }
			Corner GetCorner()const { return m_corner; }
			ICharacter* GetChar() const;
			ICharacterGroup* GetGroup() const;
			BattleState::SpriteTicket GetSprite() const;
			clan::Point GetOffset() const { return m_offset; }
		private:
			Type m_type;
			Corner m_corner;
			union Target{
				BattleState::SpriteTicket as_sprite;
				ICharacter* as_char;
				ICharacterGroup* as_group;
			}m_target;
			clan::Point m_offset;
		};
	public:
		clan::Pointf GetPosition(const Locale& locale)const;	
	private:
		
		class Task : public SteelType::IHandle {
			enum TaskState {
				IDLE,
				RUNNING,
				EXPIRED
			};
			std::atomic<TaskState> m_task_state;
		public:
			Task(AnimationState& state):m_task_state(IDLE),m_state(state),m_sync_task(NULL),m_start_after(NULL),m_sprite(BattleState::UNDEFINED_SPRITE_TICKET){
			}
			virtual ~Task(){}
			virtual std::string GetName() const=0;
			void SetFunctor(SteelType::Functor functor){
				m_functor = functor;
			}
			void SetSprite(BattleState::SpriteTicket sprite){
				m_sprite = sprite;
			}
			void SyncTo(Task* other){
				m_sync_task = other;
			}			
			void SetNextTask(Task* next){
				m_start_after = next;
			}
			void start(SteelInterpreter* pInterpreter){
				m_task_state = RUNNING;
#ifndef NDEBUG
				std::cout << "Task " << GetName() << " starting." << std::endl;
#endif				
				if(m_start_after && !m_start_after->running()){ 
					// TODO: Not this... m_start_after->start(pInterpreter);
				}
				_start(pInterpreter);
			}
			virtual void update(SteelInterpreter* pInterpreter)=0;
			virtual void draw(const clan::Rect& screenRect, clan::Canvas &gc){}
			virtual bool finished()=0;
			// Finish will be called when the task is finished
			void finish(SteelInterpreter* pInterpreter){
				m_task_state = EXPIRED;
				cleanup();
#ifndef NDEBUG
				std::cout << "Task " << GetName() << " finished and marked expired" << std::endl;
#endif
				if(m_start_after){
					m_state.AddTask(m_start_after);
					m_start_after->start(pInterpreter);
					m_start_after = NULL;
				}
			}
			std::string GetState() const {
				switch((TaskState)m_task_state){
					case IDLE:
						return "idle";
					case RUNNING:
						return "running";
					case EXPIRED:
						return "expired";
				}
				return "unknown";
			}
			bool expired() const{ return m_task_state == EXPIRED; }
			// Cleanup will be called at the end of the animation
			virtual void cleanup(){}
			float percentage()const;
			bool running()const{ return m_task_state == RUNNING;}
		protected:   

			virtual void _start(SteelInterpreter* pInterpreter)=0;
			virtual float _percentage()const=0;
			clan::Pointf get_position(const Locale& locale)const;
			clan::Pointf get_mid_point(const clan::Pointf& start, const clan::Pointf& end, float p);			
			Task* m_sync_task;			
			AnimationState& m_state;
			SteelType::Functor m_functor;
			BattleState::SpriteTicket m_sprite;
			Task* m_start_after;
		};
		
		class TimedTask: public Task{
		public:
			TimedTask(AnimationState& state):Task(state){
			};
			virtual ~TimedTask(){}
			void SetDuration(float seconds){
				m_duration = seconds;
			}			
			virtual float duration() const {
				return m_duration;
			}
			virtual bool finished() {
				return percentage() >= 1.0f;
			}
			virtual void update(){
			}
		protected:
			virtual void _start(SteelInterpreter* pInterpreter){
				m_start_time = clan::System::get_time();
			}			
			virtual float _percentage()const{
				return float(clan::System::get_time()-m_start_time) / (duration()*1000.0f);
			}

			clan::ubyte64 m_start_time;
			float m_duration;
		};
		
		class Path: public SteelType::IHandle{
		public:
			Path():m_completion(1.0),m_flags(0){}
			virtual ~Path(){}
			enum Flags {
				NORMAL=0,
				NO_VERTICAL=1,
				NO_HORIZONTAL=2,				
			};				
			Locale m_start;
			Locale m_end;
			double m_speed;
			int m_flags;
			double m_completion;
			SteelType::Functor m_functor;
			SteelType::Functor m_speed_functor;
		};
		
		class Rotation: public SteelType::IHandle{
		public:
			virtual ~Rotation(){}
			enum Axis {
				PITCH,
				ROLL,
				YAW
			};
			
			Axis m_axis;
			SteelType::Functor m_functor;
			double m_duration;
			double m_degrees;
			double m_start_degrees;
		};
		
		class Orbit: public SteelType::IHandle{
		public:
			virtual ~Orbit(){}
			bool m_clockwise;
			SteelType::Functor m_radius_functor;
			SteelType::Functor m_speed_functor;
			double m_duration;
			double m_degrees;
			double m_start_angle;
		private:
		};
		
		class Stretch: public SteelType::IHandle{
		public:
			virtual ~Stretch(){}
			SteelType::Functor m_width_functor;
			SteelType::Functor m_height_functor;
			double m_duration;
		};
		
		class Shaker: public SteelType::IHandle{
		public:
			virtual ~Shaker(){}
			SteelType::Functor m_functor;
			enum Flags { 
				NORMAL=Path::NORMAL,
				NO_HORIZONTAL=Path::NO_HORIZONTAL,
				NO_VERTICAL=Path::NO_VERTICAL
			};
			
			int m_flags;
		};
		
		class Fade: public SteelType::IHandle{
		public:
			virtual ~Fade(){}
			double m_duration;
			SteelType::Functor m_functor;
		};
		
		class Colorizer: public SteelType::IHandle{
		public:
			virtual ~Colorizer(){}
			SteelType::Functor m_red;
			SteelType::Functor m_green;
			SteelType::Functor m_blue;
		};

		
		
		class PathTask: public Task {
		public:		
			PathTask(AnimationState& state):Task(state){
			}
			virtual ~PathTask();
			virtual std::string GetName() const { return "PathTask";}
			// Here the functor is used to determine the Y-component of the vector tangental to the path
			void init(Path*);
			virtual void update(SteelInterpreter* pInterpreter);
			virtual bool finished();	
			virtual void SetSpeed(float pixels_per_ms);
			void SetCompletion(float completion);
			void SetFlags(int flags);
			void SetSpeedFunctor(SteelType::Functor functor);
			void SetStart(const Locale& start);
			void SetEnd(const Locale& end);
		private:
			virtual void _start(SteelInterpreter* pInterpreter);			
			virtual float _percentage()const;
			Path* m_path;
			clan::Pointf m_cur_pos;
			clan::ubyte64 m_start_time;
			float m_percentage_so_far;
		};
		
		class TimedPathTask: public TimedTask {
		public:
			TimedPathTask(AnimationState& state):TimedTask(state){
			};
			virtual ~TimedPathTask(){}
			virtual std::string GetName() const { return "TimedPathTask";}
			void init(Path*);

			virtual void update(SteelInterpreter*);
			void SetFlags(int flags);			
			void SetStart(const Locale& start);
			void SetEnd(const Locale& end);			
		private:
			virtual void _start(SteelInterpreter*);			
			Path* m_path;
		};		
		
		class RotationTask: public Task {
		public:
			RotationTask(AnimationState& state):Task(state){
			}
			virtual ~RotationTask(){}
			virtual std::string GetName() const { return "RotationTask";}
			void init(const Rotation& rotation);

			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			virtual float _percentage()const;
			float m_completion_degrees;
			float m_degrees;
			float m_original_degrees;
			clan::ubyte64 m_last_time;
			Rotation m_rotation;
		};	
		
		class TimedRotationTask: public TimedTask {
		public:
			TimedRotationTask(AnimationState& state):TimedTask(state){
			}
			virtual ~TimedRotationTask(){}
			virtual std::string GetName() const { return "TimedRotationTask";}
			void init(const Rotation& rot);

			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			Rotation m_rotation;
			float m_start_degrees;
			float m_completion_degrees;
			float m_degrees;	
			clan::ubyte64 m_last_time;			
		};
		
		class OrbitTask: public Task {
		public:
			OrbitTask(AnimationState& state):Task(state){
			}
			virtual ~OrbitTask(){}
			virtual std::string GetName() const { return "OrbitTask";}
			void init(const Orbit& rotation, const Locale& locale);

			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			virtual float _percentage()const;
			float m_start_degrees;
			float m_completion_degrees;
			float m_degrees;
			float m_last_radius;
			Locale m_origin;
			clan::ubyte64 m_last_time;
			Orbit m_orbit;
		};	

		class TimedOrbitTask: public TimedTask {
		public:
			TimedOrbitTask(AnimationState& state):TimedTask(state){
			}
			virtual ~TimedOrbitTask(){}
			virtual std::string GetName() const { return "TimedOrbitTask";}
			void init(const Orbit& orb, const Locale& origin);

			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			Orbit m_orbit;
			Locale m_locale;
			float m_degrees;	
			clan::ubyte64 m_last_time;				
		};		
		
		class ShakerTask: public TimedTask { 
		public:			
			ShakerTask(AnimationState& state):TimedTask(state){
			}
			virtual ~ShakerTask(){}
			virtual std::string GetName() const { return "ShakerTask"; }
			void init(const Shaker&, const Locale&);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			void pick_opposite_dir();
			void pick_rand_dir();
			bool dir_legal()const;
			Direction m_dir;
			clan::Pointf m_starting_offset;
			Shaker m_shaker;
			Locale m_locale;
			bool m_osc;
		};
		
		class TimedStretchTask : public TimedTask { 
		public:
			TimedStretchTask(AnimationState& state):TimedTask(state){
			}
			virtual ~TimedStretchTask(){}
			virtual std::string GetName() const { return "StretchTask"; }
			void init(const Stretch&);
			void SetDuration(float duration);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();			
		private:
			virtual void _start(SteelInterpreter*);			
			Stretch m_stretch;
		};
		
		class FadeTask: public TimedTask {
		public:
			FadeTask(AnimationState& state):TimedTask(state){
			}
			virtual ~FadeTask(){}
			virtual std::string GetName() const { return "FadeTask";}
			void SetDuration(float duration);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
		};
		
		class ColorizeTask : public TimedTask { 
		public:
			ColorizeTask(AnimationState& state):TimedTask(state){
			}
			virtual ~ColorizeTask(){}
			virtual std::string GetName() const { return "ColorizeTask"; }
			void init(const Colorizer&);
			void SetDuration(float duration);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();
		private:		
			Colorizer m_colorizer;
		};
		
		class FloatTaskTimed: public TimedTask  {
		public:
			FloatTaskTimed(AnimationState& state):TimedTask(state){
			}
			virtual ~FloatTaskTimed(){}
			virtual std::string GetName() const { return "FloatTaskTimed"; }
			void init(SteelType::Functor float_amt, ICharacter* iChar);
			void SetDuration(float duration);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();			
		private:
			virtual void _start(SteelInterpreter*);			
			SteelType::Functor m_float_functor;
			ICharacter* m_character;
		};
		
		class DarkenTask : public TimedTask {
		public:
			DarkenTask(AnimationState& state):TimedTask(state){
			}
			virtual ~DarkenTask(){}
			virtual std::string GetName() const { return "DarkenTask"; }
			void init(int mode, float duration,SteelType::Functor r, SteelType::Functor g,
					  SteelType::Functor b, SteelType::Functor a);

			virtual void update(SteelInterpreter*);	
			virtual void cleanup();
		private:
			virtual void _start(SteelInterpreter*);			
			SteelType::Functor m_red;
			SteelType::Functor m_green;
			SteelType::Functor m_blue;
			SteelType::Functor m_alpha;
			int m_mode;
		};
		
		class FunctionTask : public TimedTask { 
		public:
			FunctionTask(AnimationState& state):TimedTask(state){
			}
			virtual ~FunctionTask(){}
			virtual std::string GetName() const { return "FunctionTask"; }
			void init(double duration, SteelType::Functor f);

			virtual void update(SteelInterpreter*);
			virtual void cleanup();						
		private:
			virtual void _start(SteelInterpreter*);			
		};
		
		
		void add_task(Task* pTask);
		bool active_tasks_left() const;
		Task* get_top_task() const;
		
		// In paths, 
		// These functions return the Y component of the vector tangental to the straight line between
		// the two locales of the path. 
		// They can be used for rotations and scaling and colors, etc as well
		SteelType sine_wave(double p);
		SteelType arc_over(double p);
		SteelType arc_under(double p);
		SteelType createSprite(const std::string& sprite_ref); // Returns a SpriteTicket (int)
	        SteelType getSpriteZorder(int sprite);
		SteelType setSpriteZorder(int sprite, int zorder);
		SteelType getCharacterSprite(SteelType::Handle iCharacter);
		SteelType addWeaponSprite(SteelType::Handle hWeapon);
		SteelType removeSprite(int sprite);
		SteelType getCharacterLocale(SteelType::Handle iCharacter, int corner);
		SteelType getCharacterLocus(SteelType::Handle iCharacter, int corner);
		SteelType getGroupLocale(SteelType::Handle iCharacter, int corner);
		SteelType getScreenLocale(int corner);
		SteelType closestCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTarget);
		SteelType nearCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner);
		SteelType farCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner); // Returns the corner on the opposite
		SteelType getSpriteLocale(int sprite, int corner);
		SteelType setLocaleOffset(SteelType::Handle hLocale, int x, int y);
		SteelType createPath(SteelType::Handle hStartLocale, SteelType::Handle hEndLocale,
								   SteelType::Functor functor, double pixels_per_ms);
		SteelType changePathStart(SteelType::Handle hPath, SteelType::Handle hStartLocale);
		SteelType changePathEnd(SteelType::Handle hPath, SteelType::Handle hEndLocale);
		
		// If you supply a speed function, instead of using whatever speed was assigned
		// to the path, it will call the speed function (passing percentage in) to determine the number of pixels per ms
		// to move. This only works with Path, not TimedPath
		SteelType setPathSpeedFunction(SteelType::Handle hPath, SteelType::Functor functor);
		SteelType setPathCompletion(SteelType::Handle hPath,double completion);
		SteelType setPathFlags(SteelType::Handle hPath, int flags);
		SteelType moveSprite(int sprite, SteelType::Handle hpath);
		SteelType moveSpriteTimed(int sprite, SteelType::Handle hpath, double seconds);
		SteelType createRotation(SteelType::Functor speed_functor, double start_degrees, double degrees, int axis);
		SteelType createOrbit(SteelType::Functor radius_functor, SteelType::Functor speed_functor, double start_degrees, double total_deg, bool clockwise);
		SteelType orbitSprite(int sprite,SteelType::Handle hOrbit, SteelType::Handle hLocale);
		SteelType orbitSpriteTimed(int sprite, SteelType::Handle hOrbit, SteelType::Handle hLocale, double seconds);
		SteelType createStretch(SteelType::Functor width_functor, SteelType::Functor height_functor);
		SteelType stretchSpriteTimed(int sprite, SteelType::Handle hStretch, double seconds);
		
		SteelType setSpriteRotation(int sprite, double radians);
		SteelType flipSprite(int sprite, bool flip_x, bool flip_y);
		SteelType scaleSprite(int sprite, double scale_x, double scale_y);
		SteelType setSpriteColor(int sprite, double r, double g, double b);
		SteelType setSpriteLocation(int sprite, SteelType::Handle hLocale);
		SteelType floatCharacter(SteelType::Handle hCharacter,SteelType::Functor float_amt, double time);
		SteelType isLeftOf(int spriteA, int spriteB);
		
		// TODO: Set sprite alpha and color
		
		// This causes the percentage (p) of hWithTask to be used as the p of hTask
		// So, for example you can have a rotation coordinated with a speed based path movement
		// where you don't know the right amount of time for the rotation. or a fade, etc.
		// Any duration or speed or amount that hTask originally had is ignored
		SteelType syncTo(SteelType::Handle hTask, SteelType::Handle hWithTask);
		SteelType rotateSprite(int sprite, SteelType::Handle hRotation);
		SteelType rotateSpriteTimed(int sprite, SteelType::Handle hRotation, double seconds);
		SteelType createShaker(SteelType::Functor magnitude, int flags);
		SteelType shake(SteelType::Handle hlocale, SteelType::Handle hShaker, double seconds);
		SteelType createFade(SteelType::Functor functor); 
		SteelType fadeSprite(int sprite, SteelType::Handle hFade, double seconds);
		SteelType createSimpleColorizer(double r, double g, double b, double seconds);
		SteelType createColorizer(SteelType::Functor r_func, SteelType::Functor g_func, SteelType::Functor b_func);
		SteelType colorizeSprite(int sprite, SteelType::Handle, double seconds);
		SteelType startAfter(SteelType::Handle htask, SteelType::Handle hnexttask);
		SteelType pause(double seconds);
		SteelType doAfter(double after, SteelType::Functor f); // Executes f at finish
		SteelType doFunction(SteelType::Functor f);
		// sets each task in the array to start after the prior task
		SteelType chainTasks(const Steel::SteelArray& array);
		SteelType waitFor(SteelType::Handle hTask); // automatically starts the task if not started
		SteelType waitForAll(const Steel::SteelArray& alltasks);
		SteelType start(SteelType::Handle hTask); // starts and for convenience returns the task
		SteelType startAll(const Steel::SteelArray& alltasks);
	

        BattleState& m_parent;
 		SteelRunner<AnimationState>* m_pRunner;		
		
 		SteelType::Functor m_functor;	
 		std::vector<Task*> m_tasks;
		std::list<SteelType::IHandle*> m_handles;

		SteelInterpreter * m_pInterpreter;
		clan::Event m_wait_event;
		//clan::Event m_finish_event;
		clan::Thread m_steel_thread;
		mutable std::recursive_mutex m_task_mutex;		
        clan::ubyte64 m_phase_start_time;
        bool m_bDone;
	public:
		void AddTask(Task* task);
		uint& get_time();
    };


};



#endif

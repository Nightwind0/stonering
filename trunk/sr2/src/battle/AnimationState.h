#ifndef __SR2_ANIMATION_STATE_H
#define __SR2_ANIMATION_STATE_H


#include "sr_defines.h"
#include "State.h"
#include "Animation.h"
#include "SteelRunner.h"
#include "BattleState.h"
#include "Direction.h"
#include <set>

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

        void Init(Animation* pAnimation, ICharacterGroup* pCasterGroup, ICharacterGroup* pTargetGroup, ICharacter* pCaster, ICharacter* pTarget);
		void Init(SteelType::Functor pFunctor);

        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void SteelInit      (SteelInterpreter *);
        virtual void SteelCleanup   (SteelInterpreter *);
        virtual void Finish(); // Hook to clean up or whatever after being popped
		virtual bool Threaded()const { return m_functor_mode; }
		void WaitFinishedEvent();
		void FunctorCompleted();
		
		CL_Rectf GetCharacterRect(ICharacter* ichar);
		CL_Rectf GetGroupRect(ICharacterGroup* ichar);
		CL_Rectf GetSpriteRect(BattleState::SpriteTicket sprite);
		CL_Sprite GetSprite(BattleState::SpriteTicket sprite);
		BattleState::SpriteTicket AddSprite(CL_Sprite sprite);
		void SetSpritePos(BattleState::SpriteTicket sprite,const CL_Pointf& pt);	
		void Darken(int mode, float r,float g, float b, float a);
		void ClearDark(int mode);
    private:
        CL_Pointf GetFocusOrigin(const SpriteMovement::Focus&, ICharacter* pTarget);
		void draw(const CL_Rect& screenRect, CL_GraphicContext& GC);
		void draw_functor(const CL_Rect& screenRect, CL_GraphicContext& GC);
		void EndPhase();
        bool NextPhase();
        void StartPhase();
		void move_sprite(ICharacter *pActor, ICharacter* pTarget, SpriteAnimation* anim, SpriteMovement* movement, float percentage);
        void apply_alter_sprite(AlterSprite* pSprite);
        //void move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage);
		
		class Locale: public SteelType::IHandle {
		public:
			
			enum Type {
				CHARACTER,
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
			void SetOffset(const CL_Point& offset);
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
			CL_Point GetOffset() const { return m_offset; }
		private:
			Type m_type;
			Corner m_corner;
			union Target{
				BattleState::SpriteTicket as_sprite;
				ICharacter* as_char;
				ICharacterGroup* as_group;
			}m_target;
			CL_Point m_offset;
		};
		
		class Task : public SteelType::IHandle {
		public:
			Task(AnimationState& state):m_state(state),m_sync_task(NULL),m_start_after(NULL){
				m_started = false;
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
			virtual void start(SteelInterpreter* pInterpreter){
				m_started = true;
				if(m_start_after && !m_start_after->started()){
					// TODO: Not this... m_start_after->start(pInterpreter);
				}
			}
			virtual void update(SteelInterpreter* pInterpreter)=0;
			virtual void draw(const CL_Rect& screenRect, CL_GraphicContext &gc){}
			virtual bool finished()=0;
			// Finish will be called when the task is finished
			virtual void finish(){
				m_started = false;
				if(m_start_after){
					m_state.AddTask(m_start_after);
					m_start_after = NULL;
				}
			}
			// Cleanup will be called at the end of the animation
			virtual void cleanup(){}
			float percentage()const;
			bool started()const{ return m_started;}
		protected:   
			virtual float _percentage()const=0;
			CL_Pointf get_position(const Locale& locale)const;
			CL_Pointf get_mid_point(const CL_Pointf& start, const CL_Pointf& end, float p);			
			Task* m_sync_task;			
			AnimationState& m_state;
			SteelType::Functor m_functor;
			BattleState::SpriteTicket m_sprite;
			bool m_started;
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
			virtual void start(SteelInterpreter* pInterpreter){
				Task::start(pInterpreter);
				m_start_time = CL_System::get_time();
			}
			virtual float duration() const {
				return m_duration;
			}
			virtual bool finished() {
				return percentage() >= 1.0f;
			}
		protected:
			virtual float _percentage()const{
				return float(CL_System::get_time()-m_start_time) / duration();
			}

			uint m_start_time;
			float m_duration;
		};
		
		class Path: public SteelType::IHandle{
		public:
			Path(){m_flags = 0;}
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
			virtual void start(SteelInterpreter* pInterpreter);
			virtual void update(SteelInterpreter* pInterpreter);
			virtual bool finished();	
			virtual void SetSpeed(float pixels_per_ms);
			void SetCompletion(float completion);
			void SetFlags(int flags);
			void SetSpeedFunctor(SteelType::Functor functor);
			void SetStart(const Locale& start);
			void SetEnd(const Locale& end);
		private:
			virtual float _percentage()const;
			Path* m_path;
			CL_Pointf m_cur_pos;
			uint m_start_time;
			float m_percentage_so_far;
		};
		
		class TimedPathTask: public TimedTask {
		public:
			TimedPathTask(AnimationState& state):TimedTask(state){
			};
			virtual ~TimedPathTask(){}
			virtual std::string GetName() const { return "TimedPathTask";}
			void init(Path*);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			void SetFlags(int flags);			
			void SetStart(const Locale& start);
			void SetEnd(const Locale& end);			
		private:
			Path* m_path;
		};		
		
		class RotationTask: public Task {
		public:
			RotationTask(AnimationState& state):Task(state){
			}
			virtual ~RotationTask(){}
			virtual std::string GetName() const { return "RotationTask";}
			void init(const Rotation& rotation);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			virtual float _percentage()const;
			float m_start_degrees;
			float m_completion_degrees;
			float m_degrees;
			float m_cur_degrees;
			uint m_last_time;
			Rotation m_rotation;
		};	
		
		class TimedRotationTask: public TimedTask {
		public:
			TimedRotationTask(AnimationState& state):TimedTask(state){
			}
			virtual ~TimedRotationTask(){}
			virtual std::string GetName() const { return "TimedRotationTask";}
			void init(const Rotation& rot);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			virtual bool finished();
			virtual void cleanup();
		private:
			Rotation m_rotation;
		};
		
		class ShakerTask: public TimedTask { 
		public:			
			ShakerTask(AnimationState& state):TimedTask(state){
			}
			virtual ~ShakerTask(){}
			virtual std::string GetName() const { return "ShakerTask"; }
			void init(const Shaker&, const Locale&);
			void SetDuration(float duration);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			virtual void finish();
		private:
			void pick_rand_dir();
			bool dir_legal()const;
			Direction m_dir;
			Shaker m_shaker;
			Locale m_locale;
			float m_duration;
		};
		
		class FadeTask: public TimedTask {
		public:
			FadeTask(AnimationState& state):TimedTask(state){
			}
			virtual ~FadeTask(){}
			virtual std::string GetName() const { return "FadeTask";}
			void SetDuration(float duration);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			virtual void cleanup();
			virtual bool finished();
		private:
			float m_duration;
		};
		
		class ColorizeTask : public TimedTask { 
		public:
			ColorizeTask(AnimationState& state):TimedTask(state){
			}
			virtual ~ColorizeTask(){}
			virtual std::string GetName() const { return "ColorizeTask"; }
			void init(const Colorizer&);
			void SetDuration(float duration);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);
			virtual void cleanup();
		private:
			Colorizer m_colorizer;
		};
		
		class DarkenTask : public TimedTask {
		public:
			DarkenTask(AnimationState& state):TimedTask(state){
			}
			virtual ~DarkenTask(){}
			virtual std::string GetName() const { return "DarkenTask"; }
			void init(int mode, float duration,SteelType::Functor r, SteelType::Functor g,
					  SteelType::Functor b, SteelType::Functor a);
			virtual void start(SteelInterpreter*);
			virtual void update(SteelInterpreter*);	
			virtual void cleanup();
		private:
			SteelType::Functor m_red;
			SteelType::Functor m_green;
			SteelType::Functor m_blue;
			SteelType::Functor m_alpha;
			int m_mode;
		};
		
		
	
		
		
		// In paths, 
		// These functions return the Y component of the vector tangental to the straight line between
		// the two locales of the path. 
		// They can be used for rotations and scaling and colors, etc as well
		SteelType sine_wave(double p);
		SteelType arc_over(double p);
		SteelType arc_under(double p);
		SteelType createSprite(const std::string& sprite_ref); // Returns a SpriteTicket (int)
		SteelType getCharacterSprite(SteelType::Handle iCharacter);
		SteelType getWeaponSprite(SteelType::Handle iCharacter, int hand);
		SteelType removeSprite(int sprite);
		SteelType getCharacterLocale(SteelType::Handle iCharacter, int corner);
		SteelType getGroupLocale(SteelType::Handle iCharacter, int corner);
		SteelType getScreenLocale(int corner);
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
		SteelType createRotation(SteelType::Functor functor, double degrees, int axis);
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
		// sets each task in the array to start after the prior task
		SteelType chainTasks(const Steel::SteelArray& array);
		SteelType waitFor(SteelType::Handle hTask); // automatically starts the task if not started
		SteelType waitForAll(const Steel::SteelArray& alltasks);
		SteelType start(SteelType::Handle hTask); // starts and for convenience returns the task
		SteelType startAll(const Steel::SteelArray& alltasks);
	

        BattleState& m_parent;
        ICharacterGroup* m_pCasterGroup;
        ICharacterGroup* m_pTargetGroup;
        ICharacter* m_pCaster;
        ICharacter* m_pTarget;
		SteelRunner<AnimationState>* m_pRunner;		
		
        Animation * m_pAnim;
		SteelType::Functor m_functor;	
        std::list<Phase*>::const_iterator m_phase_iterator;
		std::vector<Task*> m_tasks;
		std::set<Task*> m_finished_tasks;
		std::list<SteelType::IHandle*> m_handles;
		std::set<BattleState::SpriteTicket> m_added_sprites;
		SteelInterpreter * m_pInterpreter;
		CL_Event m_wait_event;
		//CL_Event m_finish_event;
		CL_Thread m_steel_thread;
		mutable CL_Mutex m_task_mutex;		
		mutable CL_Mutex m_finished_task_mutex;
        uint m_phase_start_time;
		bool m_functor_mode;
        bool m_bDone;
	public:
		void AddTask(Task* task);
    uint& get_time();
    };


};



#endif

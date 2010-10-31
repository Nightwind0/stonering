#ifndef __SR2_ANIMATION_STATE_H
#define __SR2_ANIMATION_STATE_H


#include "sr_defines.h"
#include "State.h"
#include "Animation.h"


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
        AnimationState(BattleState& parent, ICharacterGroup* pCasterGroup, ICharacterGroup* pTargetGroup, ICharacter* pCaster, ICharacter * pTarget);
        virtual ~AnimationState();

        void Init(Animation* pAnimation);

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

    private:
        CL_Pointf GetFocusOrigin(const SpriteMovement::Focus&, ICharacter* pTarget);
	void EndPhase();
        bool NextPhase();
        void StartPhase();
	void move_sprite(ICharacter *pActor, ICharacter* pTarget, SpriteAnimation* anim, SpriteMovement* movement, float percentage);
        //void move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage);
	/*
	SteelType getSpritePoint(SteelType::Handle hSprite);
	SteelType setSpritePoint(SteelType::Handle hSprite, SteelType::Handle hPoint);
	SteelType translatePoint(SteelType::Handle hPoint, float x, float y); // Add to point
	SteelType addPointToPoint(SteelType::Handle hPoint, SteelType::Handle hOther);
	SteelType multiplyPoint(SteelType::Handle hPoint, float x, float y);
	SteelType multiplyPointByPoint(SteelType::Handle hPoint, SteelType::Handle hOther);
	SteelType getPointX(SteelType::Handle hPoint);
	SteelType getPointY(SteelType::Handle hPoint);
	SteelType setPoint(SteelType::Handle hPoint, float x, float y);
	SteelType createPoint(void);
	SteelType releasePoint(SteelType::Handle hPoint);
	SteelType createColor(float r, float g, float b);
	SteelType releaseColor(SteelType::Handle hColor);
	SteelType setSpriteScale(SteelType::Handle hSprite, float scale);
	SteelType setSpriteRotation(SteelType::Handle hSprite, float radians);
	SteelType setSpritePitch(SteelType::Handle hSprite, float pitch);
	SteelType setSpriteYaw(SteelType::Handle hSprite, float yaw);
	SteelType setSpriteAlpha(SteelType::Handle hSprite, float alpha);
	SteelType setSpriteColor(SteelType::Handle hSprite, SteelType::Handle hColor);
	*/
	

        BattleState& m_parent;
        ICharacterGroup* m_pCasterGroup;
        ICharacterGroup* m_pTargetGroup;
        ICharacter* m_pCaster;
        ICharacter* m_pTarget;
        Animation * m_pAnim;
        std::list<Phase*>::const_iterator m_phase_iterator;
        uint m_phase_start_time;
        bool m_bDone;
    };


};



#endif

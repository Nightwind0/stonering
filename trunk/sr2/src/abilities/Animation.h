#ifndef SR_ANIMATION_H
#define SR_ANIMATION_H


#include "Element.h"
#include <ClanLib/core.h>
#include <list>
#include "sr_defines.h"
#include "ScriptElement.h"
#include "SpriteRef.h"
#include "BattleState.h"

namespace StoneRing
{

    class SpriteStub;
    class SpriteMovement;
    class AlterSprite;
    class AnimationMask;
    class PlaySound;


    class AlterSprite : public Element
    {
    public:
        AlterSprite(){};
        virtual ~AlterSprite(){}
        virtual eElement WhichElement() const
        {
            return EALTERSPRITE;
        }
        eWho GetWho() const;
        enum eAlter
        {
            HIDE, SMALLER_SIZE, LARGER_SIZE, HALF_SIZE, DOUBLE_SIZE, NEGATIVE,
            X_FLIP, Y_FLIP, GRAYSCALE, GREENSCALE, REDSCALE, BLUESCALE
        };

    private:
        static eAlter alter_from_string(const std::string &str);
        eWho m_eWho;
        eAlter m_eAlter;

    };

    class SpriteStub : public Element
    {
    public:
        SpriteStub();
        virtual ~SpriteStub();
        virtual eElement WhichElement() const
        {
            return ESPRITESTUB;
        }
        enum eWhich { MAIN, OFF };

        eWhich Which() const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        eWhich m_eWhich;

    };

    class BattleSprite : public Element
    {
    public:
        BattleSprite();
        virtual ~BattleSprite();

        virtual eElement WhichElement() const { return EBATTLESPRITE; }


        eWho GetWho() const;
        eBattleSprite GetWhich() const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap );
        eWho m_eWho;
        eBattleSprite m_eWhich;
    };

    class SpriteAnimation : public Element
    {
    public:
        SpriteAnimation();
        virtual ~SpriteAnimation();
        virtual eElement WhichElement() const
        {
            return ESPRITEANIMATION;
        }

        std::string GetName() const
        {
            return m_name;
        }
        bool HasSpriteRef() const
        {
            return m_pSpriteRef != NULL;
        }
        bool HasSpriteStub() const
        {
            return m_pStub != NULL;
        }
        bool HasAlterSprite() const
        {
            return m_pAlterSprite != NULL;
        }
        bool HasSpriteMovement() const
        {
            return m_pMovement != NULL;
        }
        bool HasBattleSprite() const
        {
            return m_pBattleSprite;
        }

        SpriteRef *GetSpriteRef() const
        {
            return m_pSpriteRef;
        }
        SpriteStub *GetSpriteStub() const
        {
            return m_pStub;
        }
        BattleSprite *GetBattleSprite() const
        {
            return m_pBattleSprite;
        }

        SpriteMovement *GetSpriteMovement() const
        {
            return m_pMovement;
        }
        AlterSprite *GetAlterSprite() const
        {
            return m_pAlterSprite;
        }
        
        	
	BattleState::SpriteTicket GetSpriteTicket() const;
	
	void SetSpriteTicket(BattleState::SpriteTicket ticket);
	
	bool ShouldSkip() const 
	{
	    return m_bSkip;
	}
	void Skip() 
	{
	    m_bSkip = true;
	}
	void Unskip()
	{
	    m_bSkip = false;
	}
	
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual bool handle_element(eElement element, Element *pElement);
        virtual void load_finished();

	bool m_bSkip;
        std::string m_name;
        SpriteRef *m_pSpriteRef;
        BattleSprite * m_pBattleSprite;
        SpriteStub *m_pStub;
        SpriteMovement *m_pMovement;
        AlterSprite *m_pAlterSprite;
	BattleState::SpriteTicket m_sprite;
    };

    class SpriteMovement : public Element
    {
    public:
        SpriteMovement();
        virtual ~SpriteMovement();
        virtual eElement WhichElement() const
        {
            return ESPRITEMOVEMENT;
        }

        enum eFocus  { SCREEN, CASTER, TARGET, CASTER_GROUP, TARGET_GROUP, CASTER_LOCUS, TARGET_LOCUS };
        enum eFocusX { X_CENTER, TOWARDS, AWAY, LEFT, RIGHT };
        enum eFocusY { Y_CENTER, TOP, BOTTOM };
        enum eFocusZ { BACK, FRONT };


        struct Focus
        {
            eFocus meFocusType;
            eFocusX meFocusX;
            eFocusY meFocusY;
            eFocusZ meFocusZ;
        };

        enum eMovementDirection { STILL, N, E, S, W, NE, NW, SE, SW, MOVE_AWAY, MOVE_TOWARDS, END_FOCUS };
        enum eMovementStyle {STRAIGHT, ARC_OVER, ARC_UNDER, SINE, XONLY, YONLY, CIRCLE };
	enum eMovementCircleDir { CLOCKWISE, COUNTERCLOCKWISE, ROTATE_AWAY, ROTATE_TOWARDS };

        Focus GetInitialFocus() const;
        bool HasEndFocus() const;
        Focus GetEndFocus() const;
        bool ForEachTarget() const;
        float Rotation() const { return m_rotation; } // how many degrees to rotate about it's center during its movement
        float Periods() const; // how many periods for sine
        float Amplitude() const; // amplitude for arc/sine
        int Distance() const; // For movement without an end focus, the distance in pixels to move. (Ignored if there is an end focus)
        float Completion() const;
	bool Invert() const;
	float circleDegrees()const;
	float circleStartAngle() const;
	float circleRadius() const; // in pixels
	eMovementCircleDir circleDirection() const;


        eMovementDirection GetMovementDirection() const;
        eMovementStyle GetMovementStyle() const;
        CL_DomElement CreateDomElement(CL_DomDocument &doc) const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        //virtual bool handleElement(eElement element, Element * pElement);
        //virtual void handleText(const std::string &text);

    private:
        static eFocusX focusXFromString(const std::string &str);
        static eFocusY focusYFromString(const std::string &str);
        static eFocusZ focusZFromString(const std::string &str);
        static eFocus focusTypeFromString ( const std::string &str );
        static eMovementDirection movementDirectionFromString ( const std::string &str );
        static eMovementStyle   movementStyleFromString( const std::string &str );
	static eMovementCircleDir circleMovementFromString( const std::string &str );

        bool m_bEndFocus;
        Focus m_initial_focus;
        Focus m_end_focus;
        bool m_bForEach;
        float m_periods;
        float m_amplitude;
        int m_nDistance;
        float m_rotation;
	float m_fCompletion;
	bool m_bInvert;
	float m_fCircleDegrees;
	float m_fCircleRadius;
	float m_fCircleAngle;
	
        eMovementDirection m_eMovementDirection;
        eMovementStyle m_eMovementStyle;
	eMovementCircleDir m_eMovementCircleDir;
    };

    class Phase : public Element
    {
    public:
        Phase();
        virtual ~Phase();
        virtual eElement WhichElement() const
        {
            return EPHASE;
        }
        CL_DomElement CreateDomElement(CL_DomDocument &) const;

        bool InParallel()const{
            return m_bParallel;
        }
        uint GetDurationMs() const;

        void Execute();

        std::list<SpriteAnimation*>::const_iterator GetSpriteAnimationsBegin() const ;
        std::list<SpriteAnimation*>::const_iterator GetSpriteAnimationsEnd() const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        bool m_bParallel;
        uint m_nDuration;

        ScriptElement *m_pScript;
        std::list<SpriteAnimation*> m_sprite_animations;
    };


    class Animation : public Element
    {
    public:
        Animation();
        virtual ~Animation();
        virtual eElement WhichElement() const
        {
            return EANIMATION;
        }

        CL_DomElement CreateDomElement(CL_DomDocument &) const;
        std::string GetName() const;
        enum eType { BATTLE, WORLD };
        eType GetType() const;

        std::list<Phase*>::const_iterator GetPhasesBegin() const;
        std::list<Phase*>::const_iterator GetPhasesEnd() const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        std::list<Phase*> m_phases;
        eType m_eType;
        std::string m_name;
    };





};

#endif





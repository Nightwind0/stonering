#ifndef SR_ANIMATION_H
#define SR_ANIMATION_H


#include "Element.h"
#include <ClanLib/core.h>
#include <list>
#include "sr_defines.h"
#include "ScriptElement.h"
#include "SpriteRef.h"


namespace StoneRing{

    class SpriteStub;
    class SpriteMovement;
    class AlterSprite;
    class AnimationMask;

    class AlterSprite : public Element
    {
    public:
        AlterSprite(){};
        virtual ~AlterSprite(){}
        virtual eElement WhichElement() const { return EALTERSPRITE; }
        eWho GetWho() const;
        enum eAlter 
        {
            HIDE, SMALLER_SIZE, LARGER_SIZE, HALF_SIZE, DOUBLE_SIZE, NEGATIVE,
            X_FLIP, Y_FLIP, GRAYSCALE, GREENSCALE, REDSCALE, BLUESCALE 
        }; 

    private:
        static eWho who_from_string(const std::string &str);
        static eAlter alter_from_string(const std::string &str);
        eWho m_eWho; 
        eAlter m_eAlter;

    };

    class SpriteStub : public Element
    {
    public:
        SpriteStub();
        virtual ~SpriteStub();
        virtual eElement WhichElement() const { return ESPRITESTUB; }
        enum eBindTo { NONE, WEAPON, CHARACTER };

        std::string GetName() const;
        eBindTo GetBindTo() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        eBindTo m_eBindTo;
        std::string m_name;
    };

    class SpriteAnimation : public Element
    {
    public:
        SpriteAnimation();
        virtual ~SpriteAnimation();
        virtual eElement WhichElement() const { return ESPRITEANIMATION; }

        std::string GetName() const { return m_name; }
        bool HasSpriteRef() const { return m_pSpriteRef != NULL; }
        bool HasSpriteStub() const { return m_pStub != NULL; }
        bool HasAlterSprite() const { return m_pAlterSprite != NULL; }
        bool HasSpriteMovement() const { return m_pMovement != NULL; }

        SpriteRef *GetSpriteRef() const { return m_pSpriteRef; }
        SpriteStub *GetSpriteStub() const { return m_pStub; }
        SpriteMovement *GetSpriteMovement() const { return m_pMovement; }
        AlterSprite *GetAlterSprite() const { return m_pAlterSprite; }
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_finished();

        std::string m_name;
        SpriteRef *m_pSpriteRef;
        SpriteStub *m_pStub;
        SpriteMovement *m_pMovement;
        AlterSprite *m_pAlterSprite;
    };

    class SpriteMovement : public Element
    {
    public:
        SpriteMovement();
        virtual ~SpriteMovement();
        virtual eElement WhichElement() const{ return ESPRITEMOVEMENT; }

        enum eFocus  { SCREEN, CASTER, TARGET, CASTER_GROUP, TARGET_GROUP };
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
        enum eMovementStyle {STRAIGHT, ARC_OVER, ARC_UNDER, SINE };

        Focus GetInitialFocus() const;
        bool HasEndFocus() const;
        Focus GetEndFocus() const;
        
        eMovementDirection GetMovementDirection() const;
        eMovementStyle GetMovementStyle() const;
        CL_DomElement CreateDomElement(CL_DomDocument &doc) const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        //virtual bool handleElement(eElement element, Element * pElement);
        //virtual void handleText(const std::string &text);

    private:
        static eFocusX focusXFromString(const std::string &str);
        static eFocusY focusYFromString(const std::string &str);
        static eFocusZ focusZFromString(const std::string &str);
        static eFocus focusTypeFromString ( const std::string &str );
        static eMovementDirection movementDirectionFromString ( const std::string &str );
        static eMovementStyle   movementStyleFromString( const std::string &str );

        bool m_bEndFocus;
        Focus m_initial_focus;
        Focus m_end_focus;
        eMovementDirection m_eMovementDirection;
        eMovementStyle m_eMovementStyle;
    };

    class Phase : public Element
    {
    public:
        Phase();
        virtual ~Phase();
        virtual eElement WhichElement() const{ return EPHASE; }   
        CL_DomElement CreateDomElement(CL_DomDocument &) const;

        bool InParallel()const;
        uint GetDurationMs() const;

        void Execute();

        std::list<SpriteAnimation*>::const_iterator GetSpriteAnimationsBegin() const ;
        std::list<SpriteAnimation*>::const_iterator GetSpriteAnimationsEnd() const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
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
        virtual eElement WhichElement() const{ return EANIMATION; } 

        CL_DomElement CreateDomElement(CL_DomDocument &) const;
        std::string GetName() const;
        enum eType { BATTLE, WORLD };
        eType GetType() const;

        std::list<Phase*>::const_iterator GetPhasesBegin() const;
        std::list<Phase*>::const_iterator GetPhasesEnd() const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        std::list<Phase*> m_phases;
        eType m_eType;
        std::string m_name;
    };





};

#endif





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
        virtual eElement whichElement() const { return EALTERSPRITE; }
        eWho getWho() const;
        enum eAlter 
        {
            HIDE, SMALLER_SIZE, LARGER_SIZE, HALF_SIZE, DOUBLE_SIZE, NEGATIVE,
            X_FLIP, Y_FLIP, GRAYSCALE, GREENSCALE, REDSCALE, BLUESCALE 
        }; 

    private:
        static eWho whoFromString(const std::string &str);
        static eAlter alterFromString(const std::string &str);
        eWho meWho; 
        eAlter meAlter;

    };

    class SpriteStub : public Element
    {
    public:
        SpriteStub();
        virtual ~SpriteStub();
        virtual eElement whichElement() const { return ESPRITESTUB; }
        enum eBindTo { NONE, WEAPON, CHARACTER };

        std::string getName() const;
        eBindTo getBindTo() const;

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        eBindTo meBindTo;
        std::string mName;
    };

    class SpriteAnimation : public Element
    {
    public:
        SpriteAnimation();
        virtual ~SpriteAnimation();
        virtual eElement whichElement() const { return ESPRITEANIMATION; }

        std::string getName() const { return mName; }
        bool hasSpriteRef() const { return mpSpriteRef != NULL; }
        bool hasSpriteStub() const { return mpStub != NULL; }
        bool hasAlterSprite() const { return mpAlterSprite != NULL; }
        bool hasSpriteMovement() const { return mpMovement != NULL; }

        SpriteRef *getSpriteRef() const { return mpSpriteRef; }
        SpriteStub *getSpriteStub() const { return mpStub; }
        SpriteMovement *getSpriteMovement() const { return mpMovement; }
        AlterSprite *getAlterSprite() const { return mpAlterSprite; }
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual bool handleElement(eElement element, Element * pElement);
        virtual void loadFinished();

        std::string mName;
        SpriteRef *mpSpriteRef;
        SpriteStub *mpStub;
        SpriteMovement *mpMovement;
        AlterSprite *mpAlterSprite;
    };

    class SpriteMovement : public Element
    {
    public:
        SpriteMovement();
        virtual ~SpriteMovement();
        virtual eElement whichElement() const{ return ESPRITEMOVEMENT; }

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

        Focus getInitialFocus() const;
        bool hasEndFocus() const;
        Focus getEndFocus() const;
        
        eMovementDirection getMovementDirection() const;
        eMovementStyle getMovementStyle() const;
        CL_DomElement createDomElement(CL_DomDocument &doc) const;

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        //virtual bool handleElement(eElement element, Element * pElement);
        //virtual void handleText(const std::string &text);

    private:
        static eFocusX focusXFromString(const std::string &str);
        static eFocusY focusYFromString(const std::string &str);
        static eFocusZ focusZFromString(const std::string &str);
        static eFocus focusTypeFromString ( const std::string &str );
        static eMovementDirection movementDirectionFromString ( const std::string &str );
        static eMovementStyle   movementStyleFromString( const std::string &str );

        bool mbEndFocus;
        Focus mInitialFocus;
        Focus mEndFocus;
        eMovementDirection meMovementDirection;
        eMovementStyle meMovementStyle;
    };

    class Phase : public Element
    {
    public:
        Phase();
        virtual ~Phase();
        virtual eElement whichElement() const{ return EPHASE; }   
        CL_DomElement createDomElement(CL_DomDocument &) const;

        bool inParallel()const;
        uint getDurationMs() const;

        void execute();

        std::list<SpriteAnimation*>::const_iterator getSpriteAnimationsBegin() const ;
        std::list<SpriteAnimation*>::const_iterator getSpriteAnimationsEnd() const;

    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        bool mbParallel;
        uint mnDuration;
        ScriptElement *mpScript;
        std::list<SpriteAnimation*> mSpriteAnimations;
    };


    class Animation : public Element
    {
    public:
        Animation();
        virtual ~Animation();
        virtual eElement whichElement() const{ return EANIMATION; } 

        CL_DomElement createDomElement(CL_DomDocument &) const;
        std::string getName() const;
        enum eType { BATTLE, WORLD };
        eType getType() const;

        std::list<Phase*>::const_iterator getPhasesBegin() const;
        std::list<Phase*>::const_iterator getPhasesEnd() const;

    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        std::list<Phase*> mPhases;
        eType meType;
        std::string mName;
    };





};

#endif





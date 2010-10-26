#include <ClanLib/core.h>
#include <list>
#include <algorithm>
#include <cassert>
#include "Animation.h"
#include "LevelFactory.h"
#include "IApplication.h"
#include "Level.h"

using namespace StoneRing;



eWho who_from_string ( const std::string &str )
{
    if (str == "none") return NONE;
    else if (str == "caster") return CASTER;
    else if (str == "caster_group") return CASTER_GROUP;
    else if (str == "target") return TARGET;
    else if (str == "target_group") return TARGET_GROUP;
    else if (str == "all") return ALL;
    else throw CL_Exception("Bad Who = " + str );

}


SpriteStub::SpriteStub()
{
}

SpriteStub::~SpriteStub()
{
}


SpriteStub::eWhich SpriteStub::Which() const
{
    return m_eWhich;
}


void SpriteStub::load_attributes(CL_DomNamedNodeMap attributes)
{
  
    std::string which = get_required_string("which",attributes);
    if(which == "main")
	m_eWhich = MAIN;
    else m_eWhich = OFF;
}
  

BattleSprite::BattleSprite()
{
}

BattleSprite::~BattleSprite()
{
}

eWho BattleSprite::GetWho() const
{
    return m_eWho;
}

eBattleSprite BattleSprite::GetWhich() const
{
    return m_eWhich;
}


void BattleSprite::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_eWho = who_from_string( get_required_string("who",attributes) );

    std::string which = get_required_string("which",attributes);

    if (which == "idle") m_eWhich = IDLE;
    else if (which == "attack") m_eWhich = ATTACK;
    else if (which == "use") m_eWhich = USE;
    else if (which == "recoil") m_eWhich = RECOIL;
    else if (which == "weak") m_eWhich = WEAK;
    else if (which == "dead") m_eWhich = DEAD;
    else throw CL_Exception("Bad which on battleSprite");
}



/*
            HIDE, SMALLER_SIZE, LARGER_SIZE, HALF_SIZE, DOUBLE_SIZE, NEGATIVE,
            X_FLIP, Y_FLIP, GRAYSCALE, GREENSCALE, REDSCALE, BLUESCALE
*/
AlterSprite::eAlter AlterSprite::alter_from_string(const std::string &str)
{
    if (str == "hide") return HIDE;
    else if (str == "smaller_size") return SMALLER_SIZE;
    else if (str == "larger_size") return LARGER_SIZE;
    else if (str == "half_size") return HALF_SIZE;
    else if (str == "double_size") return DOUBLE_SIZE;
    else if (str == "negative") return NEGATIVE;
    else if (str == "xflip") return X_FLIP;
    else if (str == "yflip") return Y_FLIP;
    else if (str == "grayscale") return GRAYSCALE;
    else if (str == "greenscale") return GREENSCALE;
    else if (str == "redscale") return REDSCALE;
    else if (str == "bluescale") return BLUESCALE;
    else throw CL_Exception("Bad alter: " + str);
}


SpriteAnimation::SpriteAnimation()
        :
        m_bSkip(false),
        m_pSpriteRef(NULL),
        m_pBattleSprite(NULL),
        m_pStub(NULL),
        m_pMovement(NULL),
        m_pAlterSprite(NULL),
        m_sprite(BattleState::UNDEFINED_SPRITE_TICKET)
{
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
}

bool SpriteAnimation::handle_element(eElement element, Element * pElement)
{
    switch (element)
    {
    case ESPRITESTUB:
        m_pStub = dynamic_cast<SpriteStub*>(pElement);
        break;
    case ESPRITEREF:
        m_pSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        break;
    case ESPRITEMOVEMENT:
        m_pMovement = dynamic_cast<SpriteMovement*>(pElement);
        break;
    case EALTERSPRITE:
        m_pAlterSprite = dynamic_cast<AlterSprite*>(pElement);
        break;
    case EBATTLESPRITE:
        m_pBattleSprite = dynamic_cast<BattleSprite*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

void SpriteAnimation::load_finished()
{
    if (!m_pStub && !m_pSpriteRef && !m_pBattleSprite)
        throw CL_Exception("Missing sprite stub or sprite ref or battle sprite on spriteAnimation");
}

BattleState::SpriteTicket SpriteAnimation::GetSpriteTicket() const
{
    return m_sprite;
}

void SpriteAnimation::SetSpriteTicket(BattleState::SpriteTicket ticket)
{
    m_sprite = ticket;
}


SpriteMovement::SpriteMovement()
        :m_bEndFocus(false),m_bForEach(false),m_periods(6.0f),m_amplitude(30.0f),m_nDistance(64)
{
}


void SpriteMovement::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_initial_focus.meFocusType = focusTypeFromString(get_required_string("initialFocus",attributes));

    if (has_attribute("initialFocusX",attributes))
        m_initial_focus.meFocusX = focusXFromString( get_string("initialFocusX",attributes));
    else m_initial_focus.meFocusX = X_CENTER;

    if (has_attribute("initialFocusY",attributes))
        m_initial_focus.meFocusY = focusYFromString( get_string("initialFocusY",attributes));
    else m_initial_focus.meFocusY = Y_CENTER;

    if (has_attribute("initialFocusZ",attributes))
        m_initial_focus.meFocusZ = focusZFromString( get_string("initialFocusZ",attributes));
    else m_initial_focus.meFocusZ = FRONT;

    if (has_attribute("endFocus",attributes))
    {
        m_bEndFocus = true;
        m_end_focus.meFocusType = focusTypeFromString(get_required_string("endFocus",attributes));

        if (has_attribute("endFocusX",attributes))
            m_end_focus.meFocusX = focusXFromString( get_string("endFocusX",attributes));
        else m_end_focus.meFocusX = X_CENTER;

        if (has_attribute("endFocusY",attributes))
            m_end_focus.meFocusY = focusYFromString( get_string("endFocusY",attributes));
        else m_end_focus.meFocusY = Y_CENTER;

        if (has_attribute("endFocusZ",attributes))
            m_end_focus.meFocusZ = focusZFromString( get_string("endFocusZ",attributes));
        else m_end_focus.meFocusZ = FRONT;
    }

    if (has_attribute("movementDirection",attributes))
        m_eMovementDirection = movementDirectionFromString( get_string("movementDirection",attributes));
    else m_eMovementDirection = STILL;

    if (has_attribute("movementStyle",attributes))
        m_eMovementStyle = movementStyleFromString ( get_string("movementStyle",attributes ));
    else m_eMovementStyle = STRAIGHT;

    m_amplitude = get_implied_float("amplitude",attributes,30.0f);
    m_rotation = get_implied_float("rotation",attributes,0.0f);
    m_periods = get_implied_float("periods",attributes,6.0f);
    m_nDistance = get_implied_int("distance",attributes,64);
    m_fCompletion = get_implied_float("movementCompletion",attributes,1.0f);
    m_bInvert = get_implied_bool("invert",attributes,false);
    m_fCircleAngle = get_implied_float("circleAngle",attributes,0.0f);
    m_fCircleDegrees = get_implied_float("circleDegrees",attributes,0.0f);
    m_fCircleRadius = get_implied_float("circleRadius",attributes,0.0f);

    get_implied_bool("forEach",attributes,false);
}

bool SpriteMovement::HasEndFocus() const
{
    return m_bEndFocus;
}

CL_DomElement
SpriteMovement::CreateDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "SpriteMovement");
}

SpriteMovement::eFocus
SpriteMovement::focusTypeFromString(const std::string &str)
{
    if (str == "screen") return SCREEN;
    else if (str == "caster") return CASTER;
    else if (str == "target") return TARGET;
    else if (str == "caster_group") return CASTER_GROUP;
    else if (str == "target_group") return TARGET_GROUP;
    else if (str == "locus") return CASTER_LOCUS;
    else throw CL_Exception("Bad focus: " + str );
}

SpriteMovement::eFocusX
SpriteMovement::focusXFromString ( const std::string &str )
{
    if (str == "center") return X_CENTER;
    else if (str == "towards") return TOWARDS;
    else if (str == "away") return AWAY;
    else if (str == "left")  return LEFT;
    else if (str == "right") return RIGHT;
    else throw CL_Exception("Bad focus x type: " + str );
}

SpriteMovement::eFocusY
SpriteMovement::focusYFromString ( const std::string &str )
{
    if (str == "center") return Y_CENTER;
    else if (str == "top") return TOP;
    else if (str == "bottom") return BOTTOM;
    else throw CL_Exception("Bad focus y type: " + str );
}

SpriteMovement::eFocusZ
SpriteMovement::focusZFromString ( const std::string &str )
{
    if (str == "front") return FRONT;
    else if (str == "back") return BACK;
    else throw CL_Exception("Bad focus z type: " + str );
}


SpriteMovement::eMovementDirection
SpriteMovement::movementDirectionFromString ( const std::string &str )
{
    if (str == "still") return STILL;
    else if (str == "n") return N;
    else if (str == "s") return S;
    else if (str == "e") return E;
    else if (str == "w") return W;
    else if (str == "ne") return NE;
    else if (str == "nw") return NW;
    else if (str == "se") return SE;
    else if (str == "sw") return SW;
    else if (str == "away") return MOVE_AWAY;
    else if (str == "towards") return MOVE_TOWARDS;
    else if (str == "endFocus") return END_FOCUS;

    throw CL_Exception("Bad movement Direction: " + str);
    return STILL;
}

SpriteMovement::eMovementStyle
SpriteMovement::movementStyleFromString( const std::string &str )
{
    if (str == "straight") return STRAIGHT;
    else if (str == "arc_over") return ARC_OVER;
    else if (str == "arc_under") return ARC_UNDER;
    else if (str == "sine") return SINE;
    else if (str == "xonly") return XONLY;
    else if (str == "yonly") return YONLY;
    else if (str == "circle") return CIRCLE;
    else throw CL_Exception("Bad movementStyle " + str );
}

SpriteMovement::~SpriteMovement()
{
}


SpriteMovement::Focus SpriteMovement::GetInitialFocus() const
{
    return m_initial_focus;
}

SpriteMovement::Focus SpriteMovement::GetEndFocus() const
{
    return m_end_focus;
}

bool SpriteMovement::ForEachTarget() const
{
    return m_bForEach;
}

SpriteMovement::eMovementDirection
SpriteMovement::GetMovementDirection() const
{
    return m_eMovementDirection;
}

SpriteMovement::eMovementStyle SpriteMovement::GetMovementStyle() const
{
    return m_eMovementStyle;
}

float SpriteMovement::Amplitude() const
{
    return m_amplitude;
}

float SpriteMovement::Periods() const
{
    return m_periods;
}

int SpriteMovement::Distance() const
{
    return m_nDistance;
}

float SpriteMovement::Completion() const
{
    return m_fCompletion;
}

bool SpriteMovement::Invert() const
{
    return m_bInvert;
}

float SpriteMovement::circleDegrees()const
{
    return m_fCircleDegrees;
}

float SpriteMovement::circleStartAngle() const
{
    return m_fCircleAngle;
}
float SpriteMovement::circleRadius() const // in pixels
{
    return m_fCircleRadius;
}

SpriteMovement::eMovementCircleDir SpriteMovement::circleDirection() const
{
}



bool Phase::handle_element(eElement element, Element * pElement)
{
    switch (element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESPRITEANIMATION:
        m_sprite_animations.push_back( dynamic_cast<SpriteAnimation*>(pElement));
        break;

    default:
        return false;
    }

    return true;
}

void Phase::load_attributes(CL_DomNamedNodeMap  attributes)
{

    m_nDuration = get_required_int("duration", attributes);
    m_bParallel = get_required_bool("parallel",attributes);

}

Phase::Phase():m_pScript(NULL)
{
}

Phase::~Phase()
{
    delete m_pScript;

    std::for_each(m_sprite_animations.begin(),m_sprite_animations.end(),del_fun<SpriteAnimation>());
}


CL_DomElement
Phase::CreateDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "Phase");
}

uint Phase::GetDurationMs() const
{
    return m_nDuration;
}

void Phase::Execute()
{
    if (m_pScript)
        m_pScript->ExecuteScript();
}

std::list<SpriteAnimation*>::const_iterator
Phase::GetSpriteAnimationsBegin() const
{
    return m_sprite_animations.begin();
}

std::list<SpriteAnimation*>::const_iterator
Phase::GetSpriteAnimationsEnd() const
{
    return m_sprite_animations.end();
}





Animation::Animation()
{
}


void Animation::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);

    std::string type = get_required_string("type",attributes);

    if (type == "battle") m_eType = BATTLE;
    else if (type == "world") m_eType = WORLD;
    else throw CL_Exception("Bogus animation type: " + type );

}

bool Animation::handle_element(Element::eElement element, Element * pElement)
{
    if (element == EPHASE )
    {
        m_phases.push_back ( dynamic_cast<Phase*>(pElement) );
        return true;
    }
    else return false;
}


Animation::~Animation()
{
    std::for_each(m_phases.begin(),m_phases.end(),del_fun<Phase>());
}

CL_DomElement
Animation::CreateDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "animation");
}

std::string
Animation::GetName() const
{
    return m_name;
}



Animation::eType
Animation::GetType() const
{
    return m_eType;
}


std::list<Phase*>::const_iterator Animation::GetPhasesBegin() const
{
    return m_phases.begin();
}

std::list<Phase*>::const_iterator Animation::GetPhasesEnd() const
{
    return m_phases.end();
}














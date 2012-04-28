
#ifndef MAPPABLE_OBJECT_H
#define MAPPABLE_OBJECT_H

#include "sr_defines.h"
#include "Graphic.h"
#include "Event.h"
#include "Direction.h"
#include "Navigator.h"
#include <stack>

using std::tr1::shared_ptr;

namespace StoneRing {

    class Movement;
    class Level;
    class Director;
    
    

    class MappableObject : public SteelType::IHandle
    {
    public:
        enum eMappableObjectType { 
            NPC, SQUARE, CONTAINER, DOOR, WARP, PLAYER 
        };
        enum eMovementType { 
            MOVEMENT_NONE, MOVEMENT_WANDER, MOVEMENT_PACE_NS, MOVEMENT_PACE_EW, MOVEMENT_SCRIPT
        };
        enum eMovementSpeed {
            SLOW, MEDIUM, FAST            
        };        
        enum eSize { 
            MO_SMALL, MO_MEDIUM, MO_LARGE, MO_TALL, MO_WIDE, MO_CUSTOM             
        };        
        


        MappableObject();
        virtual ~MappableObject();
        
        
        /* Level API */
        virtual CL_Rect GetTileRect() const; 
        virtual void    Draw(CL_GraphicContext& GC, const CL_Point& offset);
        virtual void    Move(Level& level);
        virtual bool    IsSolid() const;
        virtual std::string GetName() const;
        virtual bool    IsSprite() const;
        virtual bool    IsTile() const;
        virtual int     GetDirectionBlock() const;        
        // Update sprite, etc
        virtual void    Update();
        // Returns if any events were provoked
        virtual bool    ProvokeEvents ( Event::eTriggerType trigger );
        virtual bool    DoesStep() const { return false; }
        virtual void    OnStep();
        virtual void    Stop() { m_nStepLoop = 0; Set_Frame_For_Direction(); }
        virtual bool    RespectsHotness() const{ return true; }
        virtual CL_Rect GetSpriteRect() const ;
        virtual bool    IsFlying() const;
        bool            IsAligned() const; // Is aligned on cells (not moving between them)
        CL_Point        GetPosition() const { return m_pos / 32; }  // In tiles     
        void            SetAlpha(float f) { m_alpha = f; }
        CL_Size         GetSize() const { return m_size; }
        
        virtual bool    EvaluateCondition() const;
        virtual void    Prod();
        
        /* Navigator API */
        void            PushNavigator(Navigator* pNav) { m_navStack.push( pNav ); }
        Navigator*      PopNavigator();
        void            SetPixelPosition(const CL_Point& pixel_pos);
        CL_Point        GetPixelPosition() const { return m_pos; }
        virtual eMovementType GetMovementType() const { return MOVEMENT_NONE; }
        virtual eMovementSpeed GetMovementSpeed() const { return SLOW; }

        // In pixels
        void            CalculateEdgePoints(const CL_Point &topleft, Direction dir, std::list<CL_Point> *pList);        
        static int      ConvertDirectionToDirectionBlock(Direction dir);     
    
    protected:
        enum eFlags 
        { 
            SPRITE = 1, TILEMAP = 2, SOLID = 4, FLYING = 8  
        };

        // In cells
        static CL_Size  DimensionsFromSizeType(eSize size);
        
        virtual void    Moved_One_Cell();
        virtual CL_Size Calc_Tile_Dimensions()const;
        virtual void    Set_Frame_For_Direction();
        virtual Direction Get_Default_Facing()const;
        void            Set_Size(eSize size);
        
        std::string     m_name;
        CL_Sprite       m_sprite;
        SpriteRefOrTilemap m_graphic;

        CL_Size         m_size;
        //Direction m_eFacingDirection;
        Direction       m_direction;
        
        uint            m_nStep;
        uint            m_nStepLoop; // step frame alternator
        ushort          m_StartX;
        ushort          m_StartY;
        CL_Point        m_pos;
        float           m_alpha;

        eMappableObjectType m_eType;
        char            m_cFlags;
        ushort          m_nTilesMoved;
        std::stack<Navigator*>  m_navStack;
    };

    class MappableObjectElement : public MappableObject, public Element {
    public:
        MappableObjectElement();
        virtual ~MappableObjectElement();
        /* Element API */
        virtual eElement WhichElement() const{ return EMAPPABLEOBJECT; }
        virtual bool    EvaluateCondition() const;   
        virtual bool    ProvokeEvents ( Event::eTriggerType trigger );     
        virtual eMovementType  GetMovementType() const { return m_move_type; }
        virtual eMovementSpeed GetMovementSpeed() const { return m_speed; }      
#if SR2_EDITOR
        CL_DomElement   CreateDomElement(CL_DomDocument&)const;
#endif            
    protected:
        virtual bool    handle_element(eElement element, Element * pElement );
        virtual void    load_attributes(CL_DomNamedNodeMap attributes);
        virtual void    load_finished();
    private:        
        ScriptElement*  m_pCondition;    
        std::list<Event*> m_events;  
        eMovementSpeed m_speed;
        eMovementType m_move_type;
        NPCNavigator m_navigator;
#if SR2_EDITOR
        eSize m_eSize;
#endif
    };
    
    class MappableObjectDynamic: public MappableObject {
    public:
        MappableObjectDynamic(eMappableObjectType type, eMovementType move_type, eMovementSpeed speed);
        virtual ~MappableObjectDynamic();

        virtual void    Draw(CL_GraphicContext& GC, const CL_Point& offset);
        void            SetSprite(CL_Sprite sprite, eSize size);
        void            SetSolid(bool solid);        
        virtual eMovementType  GetMovementType() const { return m_move_type; }
        virtual eMovementSpeed GetMovementSpeed() const { return m_move_speed; }    
        void SetDefaultFacing(const Direction& dir) { m_default_facing = dir; }
    protected:
        virtual void Set_Frame_For_Direction();
        virtual Direction Get_Default_Facing()const;
    private:     
        eMovementType   m_move_type;
        eMovementSpeed  m_move_speed;
        Direction       m_default_facing;
    };


    class MappablePlayer : public MappableObject
    {
    public:
        MappablePlayer(uint startX, uint startY);
        virtual ~MappablePlayer();
        virtual bool IsSolid() const { return true; }
        virtual bool IsSprite() const { return true; }
        CL_Point     GetPointInFront() const;
        virtual bool IsTile() const { return false; }
 	virtual void StopMovement();
        virtual bool DoesStep() const { return true; }
       // virtual void Moved_One_Cell();
        void         SetSprite(CL_Sprite sprite) { m_sprite = sprite; }
        virtual bool RespectsHotness()const{ return false; }
        virtual void ResetLevelX(uint x) { m_pos.x = x * 32;}
        virtual void ResetLevelY(uint y) { m_pos.y = y * 32;}
        virtual bool Step() const { return true; }
        uint         GetLevelX() const { return m_pos.x; }
        uint         GetLevelY() const { return m_pos.y; }
        virtual eMovementType
                    GetMovementType() const { return MOVEMENT_WANDER; }        
        Direction    GetDirection() const { return m_direction; }
        ControlNavigator& GetNavigator()  { return m_navigator; }
        void         SetDirection(const Direction& dir);
        void         SerializeState(std::ostream& out);
        void         DeserializeState(std::istream& in);
    private:
        virtual bool delete_sprite() const { return false; }
        virtual void Set_Frame_For_Direction();
        Direction        m_direction;
        ControlNavigator m_navigator;
    };
}

#endif




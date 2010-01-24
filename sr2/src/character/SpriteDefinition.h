#ifndef CHARACTER_DEFINITION_H
#define CHARACTER_DEFINITION_H

#include "Element.h"
#include "Character.h" // For the enums
#include "SpriteRef.h"


namespace StoneRing
{
    class SpriteDefinition;

    class SpriteDefinition : public Element
    {
    public:
        SpriteDefinition();
        virtual ~SpriteDefinition();

        virtual eElement WhichElement() const { return ESPRITEDEFINITION; }
        std::string GetName() const { return m_name; }
        bool HasBindPoints() const;
        int GetBindPoint1() const { return m_nBindPoint1; }
        int GetBindPoint2() const { return m_nBindPoint2; }
        SpriteRef * GetSpriteRef() const { return m_pSpriteRef; }

    private:
        virtual bool handle_element(eElement, Element * );
        virtual void load_attributes(CL_DomNamedNodeMap);
        std::string m_name;
        SpriteRef * m_pSpriteRef;
        bool m_bHasBindPoints;
        int m_nBindPoint1;
        int m_nBindPoint2;
    };


}

#endif





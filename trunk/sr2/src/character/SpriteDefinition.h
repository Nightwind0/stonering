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

        virtual eElement whichElement() const { return ESPRITEDEFINITION; }
        std::string getName() const { return mName; }
        bool hasBindPoints() const;
        int getBindPoint1() const { return mnBindPoint1; }
        int getBindPoint2() const { return mnBindPoint2; }
        SpriteRef * getSpriteRef() const { return mpSpriteRef; }

    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        std::string mName;
        SpriteRef * mpSpriteRef;
        bool mbHasBindPoints;
        int mnBindPoint1;
        int mnBindPoint2;
    };


};

#endif





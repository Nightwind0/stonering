#ifndef CHARACTER_DEFINITION_H
#define CHARACTER_DEFINITION_H

#include "Element.h"
#include "Character.h" // For the enums
#include "SpriteRef.h"


namespace StoneRing
{
    class SpriteDefinition;

    eCharacterAttribute CharAttributeFromString(const std::string &str); 
    eCommonAttribute CommonAttributeFromString(const std::string &str);

    // CA encompasses both common and character attributes
    uint CAFromString(const std::string &str);

    std::string CAToString(uint);

    class CharacterDefinition :     public Element
    {
    public:
        CharacterDefinition(void);
        virtual ~CharacterDefinition(void);

        virtual eElement whichElement() const { return ECHARACTER; }

    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        std::string mSpriteRef;
        std::string mName;
        // Should these both be maps? maybe later...
        std::list<SpriteDefinition*> mSpriteDefinitions;
        CharacterClass * mpClass;

    };

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





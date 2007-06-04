#ifndef SR_CHARACTER_CLASS
#define SR_CHARACTER_CLASS

#include "Element.h"
#include <ClanLib/core.h>
#include "Character.h"

namespace StoneRing
{

    class WeaponTypeRef;
    class ArmorTypeRef;
    class StatScript;
    class SkillRef;
    class BattleMenu;
    class ScriptElement;

    class CharacterClass : public Element
    {
    public:
        CharacterClass();
        virtual ~CharacterClass();
        virtual eElement whichElement() const{ return ECHARACTERCLASS; }

        std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsBegin() const;
        std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsEnd() const;

        std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsBegin() const;
        std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsEnd() const;

        double getStat(eCharacterAttribute attr, int level);

        std::list<SkillRef*>::const_iterator getSkillRefsBegin() const;
        std::list<SkillRef*>::const_iterator getSkillRefsEnd() const;

        BattleMenu * getBattleMenu() const;
        std::string getName() const;
        ICharacter::eGender getGender() const;
    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        std::string mName;
        ICharacter::eGender meGender;
        std::list<WeaponTypeRef*> mWeaponTypes;
        std::list<ArmorTypeRef*> mArmorTypes;
        typedef std::map<eCharacterAttribute,StatScript*> StatMap;
        StatMap mStatScripts;
        std::list<SkillRef*> mSkillRefs;
        BattleMenu *mpMenu;
    };

    class StatScript : public Element
    {
    public:
        StatScript();
        ~StatScript();
        virtual eElement whichElement() const{ return ESTATSCRIPT; }
        eCharacterAttribute getCharacterStat() const;
        double getStat(int level);
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual bool handleElement(Element::eElement, Element * pElement);
        virtual void loadFinished();
        ScriptElement *mpScript;
        eCharacterAttribute meStat;
    };
};

#endif





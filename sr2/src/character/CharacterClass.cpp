#include "CharacterClass.h"
#include "Skill.h"
#include "Item.h"
#include "CharacterDefinition.h"
#include "WeaponTypeRef.h"
#include "ArmorTypeRef.h"
#include "BattleMenu.h"

using namespace StoneRing;


SkillRef::SkillRef()
{
}

SkillRef::~SkillRef()
{
}

std::string SkillRef::getRef() const
{
    return mRef;
}

uint SkillRef::getSPCost() const
{
    return mnSp;
}

uint SkillRef::getBPCost() const
{
    return mnBp;
}

uint SkillRef::getMinLevel() const
{
    return mnMinLevel;
}


void SkillRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mRef = getRequiredString("skillName", pAttributes);
    mnSp = getImpliedInt("overrideSp", pAttributes,0);
    mnBp = getImpliedInt("overrideBp", pAttributes,0);
    mnMinLevel = getImpliedInt("overrideMinLevel",pAttributes,0);
}

void CharacterClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);

#ifndef NDEBUG
    std::cout << "Loading class: " << mName << std::endl;
#endif


    std::string gender = getImpliedString("gender",pAttributes, "either");

    if(gender == "male") meGender = ICharacter::MALE;       
    else if (gender == "female") meGender = ICharacter::FEMALE; 
    else if (gender == "either") meGender = ICharacter::NEUTER;
}

bool CharacterClass::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EWEAPONTYPEREF:
        mWeaponTypes.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
        break;
    case EARMORTYPEREF:
        mArmorTypes.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
        break;
    case ESTATSCRIPT:
        {
            StatScript *pScript = dynamic_cast<StatScript*>(pElement);
            mStatScripts[pScript->getCharacterStat()] = pScript;
            break;
        }
    case ESKILLREF:
        mSkillRefs.push_back( dynamic_cast<SkillRef*>(pElement));
        break;
    case EBATTLEMENU:
        mpMenu = dynamic_cast<BattleMenu*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}


double CharacterClass::getStat(eCharacterAttribute attr, int level)
{
    StatMap::iterator it = mStatScripts.find(attr);
    if(it == mStatScripts.end())
        throw CL_Error("Missing stat on character class.");

    StatScript *pScript = it->second;
    return pScript->getStat(level);
}

CharacterClass::CharacterClass()
:mpMenu(NULL)
{
    std::for_each(mWeaponTypes.begin(),mWeaponTypes.end(),del_fun<WeaponTypeRef>());
    std::for_each(mArmorTypes.begin(),mArmorTypes.end(),del_fun<ArmorTypeRef>());
    std::for_each(mSkillRefs.begin(),mSkillRefs.end(),del_fun<SkillRef>());

    for(std::map<eCharacterAttribute,StatScript*>::iterator it = mStatScripts.begin();
        it != mStatScripts.end();
        it++)
    {
        delete it->second;
    }
}

CharacterClass::~CharacterClass()
{
    delete mpMenu;
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::getWeaponTypeRefsBegin() const
{
    return mWeaponTypes.begin();
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::getWeaponTypeRefsEnd() const
{
    return mWeaponTypes.end();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::getArmorTypeRefsBegin() const
{
    return mArmorTypes.begin();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::getArmorTypeRefsEnd() const
{
    return mArmorTypes.end();
}


std::list<SkillRef*>::const_iterator CharacterClass::getSkillRefsBegin() const
{
    return mSkillRefs.begin();
}

std::list<SkillRef*>::const_iterator CharacterClass::getSkillRefsEnd() const
{
    return mSkillRefs.end();
}

std::string CharacterClass::getName() const
{
    return mName;
}


        
ICharacter::eGender 
CharacterClass::getGender() const
{
    return meGender;
}
        


void StatScript::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    std::string stat = getRequiredString("stat",pAttributes);
    meStat = CharAttributeFromString ( stat );
}

bool StatScript::handleElement(Element::eElement element, StoneRing::Element *pElement)
{
    if(element == ESCRIPT)
    {
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }

    return false;
}

void StatScript::loadFinished()
{
    if(mpScript == NULL) throw CL_Error("No script defined for scriptElement");
}

StatScript::StatScript( )
{

}

StatScript::~StatScript()
{
}

eCharacterAttribute 
StatScript::getCharacterStat() const
{
    return meStat;
}

double StatScript::getStat(int level)
{
    // Magic conversion to double
    ParameterList params;
    params.push_back ( ParameterListItem("$_CL",level) );

    return mpScript->executeScript(params);
}







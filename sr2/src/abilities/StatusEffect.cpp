#include "StatusEffect.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"


void StoneRing::StatusEffect::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name", pAttributes );
    std::string last = getRequiredString("last",pAttributes );

    if(last == "battle") meLast = BATTLE;
    else if (last == "round_count") meLast = ROUND_COUNT;
    else if (last == "permanent") meLast = PERMANENT;

    if( meLast == BATTLE )
    {
        mfLengthMultiplier = getImpliedFloat("lengthMultiplier",pAttributes,1);
    }
    else if (meLast == ROUND_COUNT )
    {
        mnRoundCount = getRequiredInt("roundCount",pAttributes);
    }

}

bool StoneRing::StatusEffect::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EONINVOKE:
        mpOnInvoke = dynamic_cast<OnInvoke*>(pElement);
        break;
    case EONROUND:
        mpOnRound = dynamic_cast<OnRound*>(pElement);
        break;
    case EONCOUNTDOWN:
        mpOnCountdown = dynamic_cast<OnCountdown*>(pElement);
        break;
    case EONREMOVE:
        mpOnRemove = dynamic_cast<OnRemove*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}

StoneRing::StatusEffect::StatusEffect():mpOnInvoke(NULL),mpOnRound(NULL),
                                        mpOnCountdown(NULL), mpOnRemove(NULL)
{
}

StoneRing::StatusEffect::~StatusEffect()
{
    delete mpOnInvoke;
    delete mpOnRound;
    delete mpOnCountdown;
    delete mpOnRemove;
}


CL_DomElement StoneRing::StatusEffect::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"statusEffect");
}

StoneRing::OnInvoke * StoneRing::StatusEffect::getOnInvoke() const
{
    return mpOnInvoke;
}

StoneRing::OnRound * StoneRing::StatusEffect::getOnRound() const
{
    return mpOnRound;
}

StoneRing::OnCountdown * StoneRing::StatusEffect::getOnCountdown() const
{
    return mpOnCountdown;
}

StoneRing::OnRemove * StoneRing::StatusEffect::getOnRemove() const
{
    return mpOnRemove;
}

std::string StoneRing::StatusEffect::getName() const
{
    return mName;
}

StoneRing::StatusEffect::eLast 
StoneRing::StatusEffect::getLast() const
{
    return meLast;
}

uint StoneRing::StatusEffect::getRoundCount() const
{
    return mnRoundCount;
}


// Multiply the magic power of the user by this using an algorithm to get length..
float StoneRing::StatusEffect::getLengthMultiplier() const
{
    return mfLengthMultiplier;
}
                




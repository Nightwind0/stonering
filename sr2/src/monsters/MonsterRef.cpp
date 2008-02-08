#include "MonsterRef.h"

using StoneRing::MonsterRef;

MonsterRef::MonsterRef():mnCount(1)
{
}

MonsterRef::~MonsterRef()
{
}


std::string MonsterRef::getName() const
{
    return mName;
}

int MonsterRef::getCount() const
{
    return mnCount;
}


void MonsterRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnCount = getImpliedInt("count",pAttributes,1);
}

void MonsterRef::loadFinished()
{
}
        
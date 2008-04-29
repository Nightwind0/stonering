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
    mnCellX = getImpliedInt("cellX",pAttributes,0);
    mnCellY = getImpliedInt("cellY",pAttributes,0);
    mnColumns = getImpliedInt("cols",pAttributes,1);
    mnRows = getImpliedInt("rows",pAttributes,1);

#ifndef NDEBUG
    std::cout << "Count = " << mnCount << " Cols = " << mnColumns << " Rows = " << mnRows << std::endl;
#endif

    if(mnColumns * mnRows < mnCount)
        throw CL_Error("MonsterRef: " + mName + " has too few rows and columns for count.");
}

void MonsterRef::loadFinished()
{
}
        
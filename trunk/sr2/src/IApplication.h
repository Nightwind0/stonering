#ifndef IAPPLICATION_H
#define IAPPLICATION_H


#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <IParty.h>

namespace StoneRing
{

class IApplication 
{
 public:


      virtual CL_ResourceManager * getResources()const=0;
      virtual IParty * getParty() const=0;
      static IApplication * getInstance();

      virtual int getScreenWidth()const=0;
      virtual int getScreenHeight()const=0;


      virtual CL_Rect getLevelRect() const=0;
      virtual CL_Rect getDisplayRect() const=0;


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot)=0;


};

};
#endif

#ifndef IAPPLICATION_H
#define IAPPLICATION_H


#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "IParty.h"
#include "LevelFactory.h"

namespace StoneRing
{

class IApplication 
{
 public:


      virtual CL_ResourceManager * getResources()const=0;
      virtual IParty * getParty() const=0;
      virtual LevelFactory *getLevelFactory() const =0;
      static IApplication * getInstance();

      virtual int getScreenWidth()const=0;
      virtual int getScreenHeight()const=0;


      virtual CL_Rect getLevelRect() const=0;
      virtual CL_Rect getDisplayRect() const=0;

      virtual void playAnimation(const std::string &animation)const=0;
      virtual void playSound(const std::string &sound)const=0;
      virtual void loadLevel(const std::string &level, uint startX, uint startY)=0;
      virtual void startBattle(const std::string &monster, uint count, bool isBoss)=0;
      virtual void say(const std::string &speaker, const std::string &text)=0;
      virtual void pause(uint time)=0;
      virtual void invokeShop(const std::string &shoptype)=0;

      


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer)=0;


};

};
#endif

//editor.h


#ifndef EDITOR_H
#define EDITOR_H

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/gui.h>
#include <ClanLib/guistylesilver.h>
#include <stdlib.h>

#include "EditableLevel.h"
#include "TileSelector.h"
#include "MapGrid.h"
#include "GridPoint.h"
#include "IApplication.h"
#include "IParty.h"
#include "Item.h"



class StoneRing::ItemRef; // Forward decl

class EditorParty : public StoneRing::IParty
{
public:
    virtual bool getGold() const { return true;}
    virtual bool hasItem(StoneRing::Item::eItemType type, const std::string &item) const{ return true; }
    virtual bool hasItem(StoneRing::ItemRef *pItemRef )const{return true;}
    virtual bool didEvent(const std::string &event) const{return true;}
    virtual uint getLevelX() const{return 0;}
    virtual uint getLevelY() const{return 0; }
    virtual uint getWidth() const { return 64; }
    virtual uint getHeight() const { return 64; }
    virtual void doEvent(const std::string &name){}
    virtual void giveItem(ItemRef * pItemRef){}
    virtual void takeItem(ItemRef * pItemRef){}
    virtual void giveGold(int amount){}
    virtual void modifyAttribute(const std::string &attribute, int add, const std::string &target){}
};


class EditorMain : public CL_ClanApplication, public StoneRing::IApplication
{
public:
	EditorMain();
	~EditorMain();

	static EditorMain *instance;


	// IApplication Interface
	
	  virtual CL_ResourceManager * getResources()const;
	  virtual StoneRing::IParty * getParty() const;
	  virtual LevelFactory * getLevelFactory() const;
      
		
      virtual int getScreenWidth()const;
      virtual int getScreenHeight()const;


      virtual CL_Rect getLevelRect() const;
      virtual CL_Rect getDisplayRect() const;


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer);


      virtual void playAnimation(const std::string &animation)const{}
      virtual void playSound(const std::string &sound)const{}
      virtual void loadLevel(const std::string &level, uint startX, uint startY){}
      virtual void startBattle(const std::string &monster, uint count, bool isBoss){}
      virtual void say(const std::string &speaker, const std::string &text){}
      virtual void pause(uint time){}
      virtual void invokeShop(const std::string &shoptype){}



private:
	int main(int argc, char **argv);
	
	CL_GUIManager *gui_manager;
	CL_ComponentManager *component_manager;

	void on_quit();
	void on_save();
	void on_load();
	void on_new();

	void on_add_row();
	void on_add_column();
	void on_show_hot();
	void on_show_blocks();
	
	void on_paint();
	void on_tileset_change(string userdata);

	bool quit;
	string menuitem;

	EditorParty * mpParty;
	EditableLevelFactory * mpLevelFactory;
	CL_ResourceManager *mpResources;

	CL_SlotContainer slots;

	TileSelector* tiles;
	MapGrid* map;

	CL_GraphicContext *gc;

	EditableLevel *mpLevel;

};



#endif


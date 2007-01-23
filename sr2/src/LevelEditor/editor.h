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
#include "ItemFactory.h"
#include "ItemManager.h"
#include "EditableLevel.h"
#include "TileSelector.h"
#include "MapGrid.h"
#include "GridPoint.h"
#include "Infobar.h"
#include "IApplication.h"
#include "IParty.h"
#include "Item.h"



class StoneRing::ItemRef; // Forward decl
class StoneRing::CharacterFactory;

class EditorParty : public StoneRing::IParty
{
public:
    virtual bool getGold() const { return true;}
    virtual bool hasItem(StoneRing::ItemRef *pItemRef, uint count )const{return true;}
    virtual bool didEvent(const std::string &event) const{return true;}

    virtual uint getLevelX() const{return 0;}
    virtual uint getLevelY() const{return 0; }
    virtual uint getWidth() const { return 64; }
    virtual uint getHeight() const { return 64; }
    virtual void doEvent(const std::string &name, bool bRemember){}
    virtual void giveItem(ItemRef * pItemRef, uint){}
    virtual void takeItem(ItemRef * pItemRef, uint){}
    virtual void giveGold(int amount){}
    virtual uint getCharacterCount() const{ return 0;}
    virtual uint getSelectedCharacterIndex() const { return 0;}
    virtual ICharacter* getCharacter(uint) const { return NULL;}
    virtual ICharacter* getSelectedCharacter() const {return NULL; }
    virtual uint getCasterCharacterIndex() const { return 0; }
    virtual ICharacter * getCasterCharacter() const{ return NULL; }
};


class EditorItemManager : public ItemManager
{
public:
    EditorItemManager(){}
    virtual ~EditorItemManager(){}

    virtual Item * getItem ( const ItemRef & ref) const { return NULL; }
private:
};

class EditorMain : public CL_ClanApplication, public StoneRing::IApplication
{
public:
	EditorMain();
	virtual ~EditorMain();

	// IApplication Interface
	
	virtual CL_ResourceManager * getResources()const;
	virtual StoneRing::IParty * getParty() const;
	virtual LevelFactory * getLevelFactory() const;
	
	ICharacterGroup *getSelectedCharacterGroup() const { return NULL; }
	CharacterFactory * getCharacterFactory() const { return NULL; }
	ItemFactory * getItemFactory() const { static ItemFactory itemFactory; return &itemFactory; }
	const ItemManager * getItemManager() const { static EditorItemManager itemManager; return &itemManager; }
	virtual AbilityFactory * getAbilityFactory() const { static AbilityFactory abilityFactory; return &abilityFactory; }
	virtual const AbilityManager * getAbilityManager() const { static AbilityManager abilityManager; return &abilityManager; }
	
	virtual int getScreenWidth()const;
	virtual int getScreenHeight()const;


      virtual CL_Rect getLevelRect() const;
      virtual CL_Rect getDisplayRect() const;


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer);

	  virtual void requestRedraw(const State *pState){}
      virtual void playScene(const std::string &animation){}
      virtual void playSound(const std::string &sound){}
      virtual void loadLevel(const std::string &level, uint startX, uint startY){}
      virtual void startBattle(const std::string &monster, uint count, bool isBoss){}
      virtual void say(const std::string &speaker, const std::string &text){}
      virtual void pause(uint time){}
      virtual void invokeShop(const std::string &shoptype){}
      virtual void choice(const std::string &, const std::vector<std::string> &, Choice*){}
	  virtual void pop(bool){}
      Infobar * getInfo() const { return mInfo; }

private:
	int main(int argc, char **argv);
	
	CL_GUIManager *mGui_manager;
	CL_ComponentManager *mComponent_manager;
	CL_FileDialog * mpDialog;

	void on_quit();
	void on_save();
	void on_load();
	void on_new();

	void on_add_row();
	void on_add_column();
	void on_show_hot();
	void on_show_blocks();
	void on_change_tool(string newtool);
	
	void on_paint();
	void on_tileset_change(string userdata);

	bool mbQuit;
	string mMenuitem;

	EditorParty * mpParty;
	EditableLevelFactory * mpLevelFactory;
	CL_ResourceManager *mpResources;

	CL_SlotContainer mSlots;

	TileSelector* mTiles;
	MapGrid* mMap;

	//CL_InputBox* info;
	Infobar* mInfo;

	CL_GraphicContext *mGc;

	EditableLevel *mpLevel;

};



#endif




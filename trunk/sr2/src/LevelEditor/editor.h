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
#include "EditorElementFactory.h"
#include "TileSelector.h"
#include "MapGrid.h"
#include "GridPoint.h"
#include "Infobar.h"
#include "IApplication.h"
#include "IParty.h"
#include "Item.h"
#include "AppUtils.h"
#include "SteelInterpreter.h"
#include "EditorElements.h"


class StoneRing::ItemRef; // Forward decl
class StoneRing::CharacterFactory;
class StoneRing::ICharacter;

namespace Editor{

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
    virtual void giveItem(StoneRing::ItemRef * pItemRef, uint){}
    virtual void takeItem(StoneRing::ItemRef * pItemRef, uint){}
    virtual void giveGold(int amount){}
    virtual uint getCharacterCount() const{ return 0;}
    virtual uint getSelectedCharacterIndex() const { return 0;}
    virtual StoneRing::ICharacter* getCharacter(uint) const { return NULL;}
    virtual StoneRing::ICharacter* getSelectedCharacter() const {return NULL; }
    virtual uint getCasterCharacterIndex() const { return 0; }
    virtual StoneRing::ICharacter * getCasterCharacter() const{ return NULL; }
};


class EditorItemManager : public StoneRing::ItemManager
{
public:
    EditorItemManager(){}
    virtual ~EditorItemManager(){}

    virtual StoneRing::Item * getItem ( const ItemRef & ref) const { return NULL; }
private:
};

class EditorMain : public CL_ClanApplication, public StoneRing::IApplication
{
public:
    EditorMain();
    virtual ~EditorMain();

    // IApplication Interface
    virtual void requestRedraw(const StoneRing::State *){on_paint();}
    virtual CL_ResourceManager * getResources()const;
    virtual StoneRing::IParty * getParty() const;
    
    StoneRing::ICharacterGroup *getSelectedCharacterGroup() const { return NULL; }
    StoneRing::CharacterFactory * getCharacterFactory() const { return NULL; }
 
    virtual StoneRing::ItemManager * getItemManager()  { static EditorItemManager itemManager; return &itemManager; }
    virtual StoneRing::AbilityManager * getAbilityManager()  { static StoneRing::AbilityManager abilityManager; return &abilityManager; }
    virtual StoneRing::IFactory *getElementFactory() { return mpLevelFactory; }
    virtual int getScreenWidth()const;
    virtual int getScreenHeight()const;

    virtual CL_Rect getLevelRect() const;
    virtual CL_Rect getDisplayRect() const;
    virtual void pop(bool popAll){}

    virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer);
    virtual AstScript * loadScript(const std::string &name, const std::string &script);
    virtual SteelType runScript(AstScript * pScript);
    virtual SteelType runScript(AstScript *pScript, const ParameterList &params); 
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
    EditorElementFactory * mpLevelFactory;
    CL_ResourceManager *mpResources;

    CL_SlotContainer mSlots;

    TileSelector* mTiles;
    MapGrid* mMap;

    //CL_InputBox* info;
    Infobar* mInfo;

    CL_GraphicContext *mGc;

    Editor::Level *mpLevel;
    StoneRing::AppUtils mAppUtils;
    SteelInterpreter mInterpreter;
};

};

#endif






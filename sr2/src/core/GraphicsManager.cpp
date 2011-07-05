#include "IApplication.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include "GraphicsManager.h"


using namespace StoneRing;
using StoneRing::Font;




GraphicsManager * GraphicsManager::m_pInstance=NULL;


void GraphicsManager::initialize()
{
    if(!m_pInstance){
	m_pInstance = new GraphicsManager();
    }
}

CL_Gradient GraphicsManager::GetMenuGradient()
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String gradientname = CL_String("Game/MenuGradient");
    
    CL_Resource resource = resources.get_resource(gradientname);
    
    if(resource.get_type() != "gradient")
        throw CL_Exception("Gradient resource element was not 'gradient' type");
    
    CL_Colorf top_left(resource.get_element().get_attribute("top_left"));
    CL_Colorf top_right(resource.get_element().get_attribute("top_right"));
    CL_Colorf bottom_left(resource.get_element().get_attribute("bottom_left"));
    CL_Colorf bottom_right(resource.get_element().get_attribute("bottom_right"));
    
    return CL_Gradient(top_left,top_right,bottom_left,bottom_right);
}

CL_Pointf GraphicsManager::GetMenuInset()
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Resource resource = resources.get_resource("Game/MenuInset");
    
    if(resource.get_type() != "point")
        throw CL_Exception("Point resource element was not 'point' type");
    
    
    float x = atof(resource.get_element().get_attribute("x").c_str());
    float y = atof(resource.get_element().get_attribute("y").c_str());
    
    return CL_Pointf(x,y);        
}


CL_Sprite GraphicsManager::GetPortraits ( const std::string& character)
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/Portraits/" +  character, &resources);
    
    CL_Sprite clone(GET_MAIN_GC());
    clone.clone(sprite); 
    
    sprite.set_alignment(origin_center);
    return clone;
}

CL_Image GraphicsManager::CreateImage ( const std::string& name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
    
    CL_Image image(GET_MAIN_GC(),name,&resources);
    
    return image;
}


CL_Sprite GraphicsManager::CreateSprite ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/" +  name, &resources);
    
    CL_Sprite clone(GET_MAIN_GC());
    clone.clone(sprite); 
    
    sprite.set_alignment(origin_center);

    return clone;
}

std::string GraphicsManager::LookUpMapWithSprite (CL_Sprite surface)
{
    for ( std::map<std::string,CL_Sprite>::iterator i = m_pInstance->m_tile_map.begin();
            i != m_pInstance->m_tile_map.end();
            i++)
    {
        if ( i->second == surface)
        {
            return i->first;
        }
    }

    throw CL_Exception ( "BAD! TILEMAP NOT FOUND IN lookupMapWithSurface" );

    return "die";

}

CL_Sprite GraphicsManager::GetTileMap ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite surface;


    if (m_pInstance->m_tile_map.find( name ) == m_pInstance->m_tile_map.end())
    {
#ifndef NDEBUG
        std::cout << "TileMap now loading: " << name << std::endl;
#endif
        surface = CL_Sprite(GET_MAIN_GC(),"Tilemaps/" + name, &resources);

        m_pInstance->m_tile_map[ name ] = surface;
        return surface;
    }

    surface = m_pInstance->m_tile_map[name];

    return surface;
}

std::string GraphicsManager::NameOfOverlay(Overlay overlay)
{
    switch (overlay)
    {
    case BATTLE_STATUS:
        return "BattleStatus";
    case BATTLE_MENU:
        return "BattleMenu";
    case BATTLE_POPUP_MENU:
        return "BattlePopup";
    case CHOICE:
        return "Choice";
    case SAY:
        return "Say";
    case EXPERIENCE:
	return "Experience";
    case MAIN_MENU:
	return "MainMenu";
    case ITEMS:
	return "ItemSelect";
    case DYNAMIC_MENU:
        return "DynamicMenu";
    case SKILL_TREE:
        return "SkillTree";
    case EQUIP:
        return "Equip";
    default:
        assert(0);
    }

    assert(0);
    return "";
}

std::string GraphicsManager::NameOfDisplayFont(DisplayFont font)
{
    switch (font)
    {
    case DISPLAY_HP_POSITIVE:
        return "Font_hp_plus";
    case DISPLAY_HP_NEGATIVE:
        return "Font_hp_minus";
    case DISPLAY_MP_POSITIVE:
        return "Font_mp_plus";
    case DISPLAY_MP_NEGATIVE:
        return "Font_mp_minus";
    case DISPLAY_MISS:
        return "Font_miss";
    case DISPLAY_FONT_SHADOW:
	return "Font_display_shadow";
    default:
        assert(0);
        return "";
    }

}

CL_Sprite  GraphicsManager::CreateMonsterSprite (const std::string &monster, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/Monsters/"+monster+'/'+sprite_id,&resources);


    sprite.set_alignment(origin_center);
  
    return sprite;
}

CL_Sprite GraphicsManager::CreateCharacterSprite ( const std::string &player, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/BattleSprites/"+player+'/'+sprite_id,&resources);


    sprite.set_alignment(origin_center);
    return sprite;
}

CL_Sprite  GraphicsManager::CreateEquipmentSprite ( EquipmentSpriteType type, const std::string& sprite_name)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    std::string item_type;
    switch(type)
    {
	case EQUIPMENT_SPRITE_WEAPON:
	    item_type = "Weapon";
	    break;
	case EQUIPMENT_SPRITE_ARMOR:
	    item_type = "Armor";
	    break;
    }
    
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/Equipment/" + item_type + '/' + sprite_name, &resources);
    
    
    sprite.set_alignment(origin_center);
    return sprite;
}


CL_Image  GraphicsManager::GetBackdrop(const std::string &name)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Image surface = CL_Image(GET_MAIN_GC(),"Backdrops/" + name, &resources);

    return surface;
}

CL_Image GraphicsManager::GetOverlay(GraphicsManager::Overlay overlay)
{
    std::map<Overlay,CL_Image>::iterator foundIt = m_pInstance->m_overlay_map.find(overlay);

    if (foundIt != m_pInstance->m_overlay_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

        CL_Image surface( GET_MAIN_GC(),std::string("Overlays/") + NameOfOverlay(overlay) + "/overlay", &resources );

        m_pInstance->m_overlay_map [ overlay ] = surface;

        return surface;
    }
}


CL_Image GraphicsManager::GetIcon(const std::string& icon)
{
    std::map<std::string,CL_Image>::iterator foundIt = m_pInstance->m_icon_map.find(icon);

    if (foundIt != m_pInstance->m_icon_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
        CL_Image surface;
        try {
            surface =  CL_Image( GET_MAIN_GC(), std::string("Icons/") + icon, &resources );
        }
        catch(CL_Exception e){
            surface = CL_Image( GET_MAIN_GC(), std::string("Icons/no_icon"), &resources );
        }
        m_pInstance->m_icon_map [ icon ] = surface;

        return surface;
    }
}

StoneRing::Font  GraphicsManager::GetFont(const std::string &name)
{
    std::map<std::string,Font>::iterator foundIt = m_pInstance->m_font_map.find( name );

    if (foundIt != m_pInstance->m_font_map.end())
    {
        return foundIt->second;
    }
    else
    {
	return m_pInstance->LoadFont(name); // also puts it into map
    }
}

StoneRing::Font GraphicsManager::LoadFont(const std::string& name)
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
    const std::string fontname = "Fonts/" + name;
    CL_Resource font_resource = resources.get_resource(fontname);
    
    CL_Font font;
    CL_Pointf shadow_offset;
    CL_Colorf color = CL_Colorf::white;
    if(font_resource.get_type() == "font")
    {
	 CL_Font_Sprite spritefont( GET_MAIN_GC(), fontname, &resources );
	 font = spritefont;
    }
    else if(font_resource.get_type() == "freetype")
    {
        CL_String filename = font_resource.get_element().get_attribute("file");
        CL_String fontname = font_resource.get_element().get_attribute("font_name");
        CL_String size_str = font_resource.get_element().get_attribute("size");
        CL_String color_str =  font_resource.get_element().get_attribute("color");
	float shadowx, shadowy;
	if(font_resource.get_element().has_attribute("shadowx"))
	    shadowx = atof(font_resource.get_element().get_attribute("shadowx").c_str());
	if(font_resource.get_element().has_attribute("shadowy"))
	    shadowy = atof(font_resource.get_element().get_attribute("shadowy").c_str());	
	
	shadow_offset = CL_Pointf(shadowx,shadowy);
	CL_Colorf thecolor(color_str);
        int size = atoi(size_str.c_str());

        CL_IODevice file = resources.get_directory(font_resource).open_file_read(filename);
        CL_Font_Freetype thefont(GET_MAIN_GC(),
                                 filename,
                                 size,
                                 file);
	font = thefont;
	color = thecolor;
    }
    else if(font_resource.get_type() == "systemfont")
    {
	CL_String fontname = font_resource.get_element().get_attribute("font_name");
	CL_String size_str = font_resource.get_element().get_attribute("size");
	CL_String color_str = font_resource.get_element().get_attribute("color");
	CL_Colorf thecolor(color_str);
	
	CL_Font_System thefont(GET_MAIN_GC(), fontname, atoi(size_str.c_str()));
	
	font = thefont;
	color = thecolor;
    }
    
    Font thefont;
    thefont.m_font = font;
    thefont.m_color = color;
    thefont.m_shadow_offset = shadow_offset;
    m_pInstance->m_font_map[name] = thefont;;
    
    return thefont;
}

std::string  GraphicsManager::GetFontName ( Overlay overlay, const std::string& type )
{
    std::map<Overlay,std::map<std::string,std::string> >::iterator mapIt = m_pInstance->m_overlay_font_map.find( overlay  );
    CL_ResourceManager & resources  = IApplication::GetInstance()->GetResources();
    std::string fontname;

    if (mapIt != m_pInstance->m_overlay_font_map.end())
    {
        std::map<std::string,std::string>::iterator foundIt = mapIt->second.find(type);
        if (foundIt != mapIt->second.end())
        {
            fontname = foundIt->second;
        }
        else
        {
            // This overlay has an entry, but not this font yet
            fontname =  CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay)
                                                    + "/fonts/" + type ), resources );
            mapIt->second[type] = fontname;
        }
    }
    else
    {
        // Overlay doesnt have an entry yet
        fontname = CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay)
                                               + "/fonts/" + type ), resources );

        m_pInstance->m_overlay_font_map[overlay][type] = fontname;
    }


    return fontname;
}

CL_Colorf GraphicsManager::GetColor ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String colorStr = CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay) + "/colors/" + name),resources);
    
    return CL_Colorf(colorStr);
}


std::string  GraphicsManager::GetFontName( DisplayFont font )
{
    return NameOfDisplayFont(font);

}


StoneRing::Font GraphicsManager::GetFont( Overlay overlay, const std::string& type )
{
    return GetFont( GetFontName ( overlay, type ) );
}


CL_Pointf GraphicsManager::GetPoint ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String pointname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/points/" + name);
    
    CL_Resource resource = resources.get_resource(pointname);
    
    if(resource.get_type() != "point")
        throw CL_Exception("Point resource element was not 'point' type");
    
    
    float x = atof(resource.get_element().get_attribute("x").c_str());
    float y = atof(resource.get_element().get_attribute("y").c_str());
    
    return CL_Pointf(x,y);    
}

CL_Image GraphicsManager::GetImage ( GraphicsManager::Overlay overlay, const std::string& i_name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String name = CL_String("Overlays/" + NameOfOverlay(overlay) + '/' + i_name);
    
    CL_Image image(GET_MAIN_GC(),name,&resources);

    return image;
}


CL_Rectf GraphicsManager::GetRect ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String pointname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/rects/" + name);
    
    CL_Resource resource = resources.get_resource(pointname);
    
    if(resource.get_type() != "rect")
        throw CL_Exception("Rect resource element was not 'rect' type");
    
    
    float top = atof(resource.get_element().get_attribute("top").c_str());
    float left = atof(resource.get_element().get_attribute("left").c_str());
    float right = atof(resource.get_element().get_attribute("right").c_str());
    float bottom = atof(resource.get_element().get_attribute("bottom").c_str());    
    return CL_Rectf(left,top,right,bottom);
}

CL_Gradient GraphicsManager::GetGradient ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String gradientname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/gradients/" + name);
    
    CL_Resource resource = resources.get_resource(gradientname);
    
    if(resource.get_type() != "gradient")
        throw CL_Exception("Gradient resource element was not 'gradient' type");
    
    CL_Colorf top_left(resource.get_element().get_attribute("top_left"));
    CL_Colorf top_right(resource.get_element().get_attribute("top_right"));
    CL_Colorf bottom_left(resource.get_element().get_attribute("bottom_left"));
    CL_Colorf bottom_right(resource.get_element().get_attribute("bottom_right"));

    return CL_Gradient(top_left,top_right,bottom_left,bottom_right);
}

CL_Sprite GraphicsManager::GetSprite ( GraphicsManager::Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Overlays/"+NameOfOverlay(overlay)+"/sprites/"+name,&resources);


    sprite.set_alignment(origin_center);
    return sprite;
}

CL_Sprite GraphicsManager::GetSpriteWithImage ( const CL_Image image )
{
    // TODO: Is there a better way to do this? Such as loading the spriate frame directly from the resource manager?
    CL_Texture texture(GET_MAIN_GC(), image.get_width(), image.get_height(), cl_rgba8);
    CL_FrameBuffer framebuffer(GET_MAIN_GC());  
    framebuffer.attach_color_buffer(0, texture);
    GET_MAIN_GC().set_frame_buffer(framebuffer);
    image.draw(GET_MAIN_GC(),0,0);
    GET_MAIN_GC().reset_frame_buffer();
    
    // Okay. Now the text should have the image on it..
    CL_SpriteDescription desc;
    desc.add_frame(texture);
    
    CL_Sprite sprite(GET_MAIN_GC(),desc);
    sprite.set_alignment(origin_center);
    
    return sprite;
}


GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{


}






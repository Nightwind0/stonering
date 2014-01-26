#include "IApplication.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include <cmath>
#include "GraphicsManager.h"


using namespace StoneRing;
using StoneRing::Font;

#ifndef M_PI
#define M_PI 3.1415926
#endif

GraphicsManager * GraphicsManager::m_pInstance = NULL;


void GraphicsManager::initialize() {
	if( !m_pInstance ) {
		m_pInstance = new GraphicsManager();
	}
}

clan::Gradient GraphicsManager::GetMenuGradient() {
	return m_pInstance->m_theme.m_menu_gradient;
}

clan::Pointf GraphicsManager::GetMenuInset() {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	clan::XMLResourceNode node = clan::XMLResourceManager::get_doc(resources).get_resource( "Game/MenuInset" );

	if( node.get_type() != "point" )
		throw clan::Exception( "Point resource element was not 'point' type" );


	float x = atof( node.get_element().get_attribute( "x" ).c_str() );
	float y = atof( node.get_element().get_attribute( "y" ).c_str() );

	return clan::Pointf( x, y );
}


clan::Sprite GraphicsManager::GetPortraits( const std::string& character ) {
	clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();
	clan::Sprite clone;
	std::function<void()> func = [&] {
		clan::Canvas canvas( GET_MAIN_CANVAS() );
		clan::Sprite  sprite =	clan::Sprite::resource( canvas, "Sprites/Portraits/" +  character, resources );	
		clone = sprite.clone();
	};
	
	
	IApplication::GetInstance()->RunOnMainThread(func);

	return clone;
}

clan::Image GraphicsManager::CreateImage( const std::string& name ) {
	clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();

	
	clan::Image image;
	
	std::function<void()> func = [&]{
		clan::Canvas canvas(GET_MAIN_CANVAS());
		image = clan::Image::resource( canvas, name, resources );
	};
	
	IApplication::GetInstance()->RunOnMainThread(func);

	return image;
}


clan::Sprite GraphicsManager::CreateSprite( const std::string & name, bool add_category ) {
	clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();

	
	
	clan::Sprite clone;
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());		
		clan::Sprite  sprite =	clan::Sprite::resource( canvas, ( add_category ? "Sprites/" : "" ) +  name, resources );
		clone = sprite.clone();
	};

	IApplication::GetInstance()->RunOnMainThread(func);

	//clone.set_alignment( clan::origin_center );

	return clone;
}

std::string GraphicsManager::LookUpMapWithImage( clan::Texture surface ) {
	for( std::map<std::string, clan::Texture2D>::iterator i = m_pInstance->m_tile_map.begin();
						i != m_pInstance->m_tile_map.end();
						i++ ) {
		if( i->second == surface ) {
			return i->first;
		}
	}

	throw clan::Exception( "BAD! TILEMAP NOT FOUND IN lookupMapWithSurface" );

	return "die";

}

clan::Texture2D GraphicsManager::GetTileMap( const std::string & name ) {
	clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();

	clan::Texture2D surface;


	if( m_pInstance->m_tile_map.find( name ) == m_pInstance->m_tile_map.end() ) {
#ifndef NDEBUG
		std::cout << "TileMap now loading: " << name << std::endl;
#endif
	std::function<void()> func = [&] {
			clan::Canvas canvas(GET_MAIN_CANVAS());
			surface = clan::Texture2D::resource( canvas, "Tilemaps/" + name, resources).get().to_texture_2d();		
		};	
		IApplication::GetInstance()->RunOnMainThread(func);
	

		m_pInstance->m_tile_map[ name ] = surface;
		return surface;
	}

	surface = m_pInstance->m_tile_map[name];

	return surface;
}

std::string GraphicsManager::NameOfOverlay( Overlay overlay ) {
	switch( overlay ) {
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
		case SHOP:
			return "Shop";
		case SAVE_LOAD:
			return "SaveLoad";
		case STARTUP:
			return "Startup";
		case STATUS:
			return "Status";
		case BANNER:
			return "Banner";
		case GAMEOVER:
			return "Gameover";
		case ITEM_GET:
			return "ItemGet";
		case GOLD_GET:
			return "GoldGet";
		case SKILL_GET:
			return "SkillGet";
		case ITEM_GET_SINGLE:
			return "ItemGetSingle";
		default:
			assert( 0 );
	}

	assert( 0 );
	return "";
}

std::string GraphicsManager::NameOfDisplayFont( DisplayFont font ) {
	switch( font ) {
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
			assert( 0 );
			return "";
	}

}

clan::Sprite  GraphicsManager::CreateMonsterSprite( const std::string &monster, const std::string &sprite_id ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	clan::Sprite sprite; 	
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		sprite = clan::Sprite::resource( canvas, "Sprites/Monsters/" + monster + '/' + sprite_id, resources );
	};

	IApplication::GetInstance()->RunOnMainThread(func);
	
	sprite.set_alignment( clan::origin_center );

	return sprite;
}

clan::Sprite GraphicsManager::CreateCharacterSprite( const std::string &player, const std::string &sprite_id ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();

	clan::Sprite sprite; 	
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		sprite = clan::Sprite::resource( canvas, "Sprites/BattleSprites/" + player + '/' + sprite_id, resources );
	};

	IApplication::GetInstance()->RunOnMainThread(func);

	sprite.set_alignment( clan::origin_center );
	return sprite;
}

clan::Sprite  GraphicsManager::CreateEquipmentSprite( EquipmentSpriteType type, const std::string& sprite_name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string item_type;
	switch( type ) {
		case EQUIPMENT_SPRITE_WEAPON:
			item_type = "Weapon";
			break;
		case EQUIPMENT_SPRITE_ARMOR:
			item_type = "Armor";
			break;
	}
	clan::Sprite sprite;
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		sprite = clan::Sprite::resource( canvas, "Sprites/Equipment/" + item_type + '/' + sprite_name, resources );
	};
	
	IApplication::GetInstance()->RunOnMainThread(func);
	

	sprite.set_alignment( clan::origin_center );
	return sprite;
}


clan::Image  GraphicsManager::GetBackdrop( const std::string &name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	clan::Image surface;
	std::function<void()> func = [&] { 
		clan::Canvas canvas(GET_MAIN_CANVAS());
		surface = clan::Image::resource( canvas, "Backdrops/" + name, resources );
	};
	
	IApplication::GetInstance()->RunOnMainThread(func);
	
	return surface;
}

clan::Image GraphicsManager::GetOverlay( GraphicsManager::Overlay overlay ) {
	std::map<Overlay, clan::Image>::iterator foundIt = m_pInstance->m_overlay_map.find( overlay );

	if( foundIt != m_pInstance->m_overlay_map.end() ) {
		return foundIt->second;
	} else {
		clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();
		clan::Image surface;
		std::function<void()> func = [&] {
			clan::Canvas canvas(GET_MAIN_CANVAS());
			surface = clan::Image::resource( canvas, std::string( "Overlays/" ) + NameOfOverlay( overlay ) + "/overlay", resources );
		};
		
		IApplication::GetInstance()->RunOnMainThread(func);
		
		m_pInstance->m_overlay_map [ overlay ] = surface;

		return surface;
	}
}


clan::Image GraphicsManager::GetIcon( const std::string& icon ) {

	std::map<std::string, clan::Image>::iterator foundIt = m_pInstance->m_icon_map.find( icon );

	if( foundIt != m_pInstance->m_icon_map.end() ) {
		return foundIt->second;
	} else {
		clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();
		clan::Image surface;
		std::function<void()> func = [&] {
			clan::Canvas canvas(GET_MAIN_CANVAS());			
			try {
				surface =  clan::Image::resource( canvas, std::string( "Icons/" ) + icon, resources );
			} catch( clan::Exception e ) {
				surface = clan::Image::resource( canvas, std::string( "Icons/no_icon" ), resources );
			}
		};
		
		IApplication::GetInstance()->RunOnMainThread(func);
		
		m_pInstance->m_icon_map [ icon ] = surface;

		return surface;
	}
}

StoneRing::Font  GraphicsManager::GetFont( const std::string &name ) {
	std::map<std::string, Font>::iterator foundIt = m_pInstance->m_font_map.find( name );

	if( foundIt != m_pInstance->m_font_map.end() ) {
		return foundIt->second;
	} else {
		return m_pInstance->LoadFont( name ); // also puts it into map
	}
}

GraphicsManager::Theme::ColorRef GraphicsManager::GetColorRef( const std::string& color_name ) {
	if( color_name == "color_hp" )
		return Theme::COLOR_HP;
	else if( color_name == "color_mp" )
		return Theme::COLOR_MP;
	else if( color_name == "color_sp" )
		return Theme::COLOR_SP;
	else if( color_name == "color_main" )
		return Theme::COLOR_MAIN;
	else if( color_name == "color_accent" )
		return Theme::COLOR_ACCENT;
	else return Theme::COLOR_COUNT;
}


StoneRing::Font GraphicsManager::LoadFont( const std::string& name ) {
	clan::ResourceManager& resources  = IApplication::GetInstance()->GetResources();
	const std::string fontname =  name;
	clan::XMLResourceNode font_resource = clan::XMLResourceManager::get_doc(resources).get_resource( fontname );
	
	clan::FontDescription desc;
	desc.set_typeface_name(name);
	desc.set_height(24);

	clan::Font font;
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		font = clan::Font::resource(canvas,desc,resources);
	};
	IApplication::GetInstance()->RunOnMainThread(func);
	clan::Pointf shadow_offset;
	clan::Colorf color = clan::Colorf::white;


	 std::string color_str =  font_resource.get_element().get_attribute( "color" );
     Theme::ColorRef color_ref = GetColorRef( color_str );
     if( color_ref != Theme::COLOR_COUNT ) {
		color = m_theme.m_colors[color_ref];
     } else {
        color = clan::Colorf( color_str );
     }
	float shadowx, shadowy;
	if( font_resource.get_element().has_attribute( "shadowx" ) )
		shadowx = atof( font_resource.get_element().get_attribute( "shadowx" ).c_str() );
	if( font_resource.get_element().has_attribute( "shadowy" ) )
		shadowy = atof( font_resource.get_element().get_attribute( "shadowy" ).c_str() );

	shadow_offset = clan::Pointf( shadowx, shadowy );


	Font thefont;
	thefont.m_font = font;
	thefont.m_color = color;
	thefont.m_shadow_offset = shadow_offset;
	thefont.m_shadow_color = m_theme.m_colors[Theme::COLOR_SHADOW];
	m_pInstance->m_font_map[name] = thefont;;

	return thefont;
}

std::string  GraphicsManager::GetFontName( Overlay overlay, const std::string& type ) {
	std::map<Overlay, std::map<std::string, std::string> >::iterator mapIt = m_pInstance->m_overlay_font_map.find( overlay );
	clan::ResourceManager & resources  = IApplication::GetInstance()->GetResources();
	std::string fontname;

	if( mapIt != m_pInstance->m_overlay_font_map.end() ) {
		std::map<std::string, std::string>::iterator foundIt = mapIt->second.find( type );
		if( foundIt != mapIt->second.end() ) {
			fontname = foundIt->second;
		} else {
			// This overlay has an entry, but not this font yet
			fontname =  String_load( std::string( "Overlays/" + NameOfOverlay( overlay )
																																												+ "/fonts/" + type ), resources );
			mapIt->second[type] = fontname;
		}
	} else {
		// Overlay doesnt have an entry yet
		fontname = String_load( std::string( "Overlays/" + NameOfOverlay( overlay )
																																										+ "/fonts/" + type ), resources );

		m_pInstance->m_overlay_font_map[overlay][type] = fontname;
	}


	return fontname;
}

clan::Colorf GraphicsManager::GetColor( Overlay overlay, const std::string& name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string colorStr = String_load( std::string( "Overlays/" + NameOfOverlay( overlay ) + "/colors/" + name ), resources );

	return clan::Colorf( colorStr );
}


std::string  GraphicsManager::GetFontName( DisplayFont font ) {
	return NameOfDisplayFont( font );

}


StoneRing::Font GraphicsManager::GetFont( Overlay overlay, const std::string& type ) {
	return GetFont( GetFontName( overlay, type ) );
}


clan::Pointf GraphicsManager::GetPoint( Overlay overlay, const std::string& name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string pointname = std::string( "Overlays/" + NameOfOverlay( overlay ) + "/points/" + name );

	clan::XMLResourceNode resource = clan::XMLResourceManager::get_doc(resources).get_resource( pointname );

	if( resource.get_type() != "point" )
		throw clan::Exception( "Point resource element was not 'point' type " +  NameOfOverlay(overlay) + name.c_str());


	float x = atof( resource.get_element().get_attribute( "x" ).c_str() );
	float y = atof( resource.get_element().get_attribute( "y" ).c_str() );

	return clan::Pointf( x, y );
}

clan::Image GraphicsManager::GetImage( GraphicsManager::Overlay overlay, const std::string& i_name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string name = std::string( "Overlays/" + NameOfOverlay( overlay ) + '/' + i_name );
	clan::Image image;
	
	std::function<void()> func = [&] {
	clan::Canvas canvas ( GET_MAIN_CANVAS() );
		image = clan::Image::resource( canvas, name, resources );
	};
	IApplication::GetInstance()->RunOnMainThread(func);
	return image;
}


clan::Rectf GraphicsManager::GetRect( Overlay overlay, const std::string& name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string pointname = std::string( "Overlays/" + NameOfOverlay( overlay ) + "/rects/" + name );

	clan::XMLResourceNode resource = clan::XMLResourceManager::get_doc(resources).get_resource( pointname );

	if( resource.get_type() != "rect" )
		throw clan::Exception( "Rect resource element was not 'rect' type" );


	float top = atof( resource.get_element().get_attribute( "top" ).c_str() );
	float left = atof( resource.get_element().get_attribute( "left" ).c_str() );
	float right = atof( resource.get_element().get_attribute( "right" ).c_str() );
	float bottom = atof( resource.get_element().get_attribute( "bottom" ).c_str() );
	return clan::Rectf( left, top, right, bottom );
}

clan::Gradient GraphicsManager::GetGradient( Overlay overlay, const std::string& name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	std::string gradientname = std::string( "Overlays/" + NameOfOverlay( overlay ) + "/gradients/" + name );

	clan::XMLResourceNode resource = clan::XMLResourceManager::get_doc(resources).get_resource( gradientname );

	if( resource.get_type() != "gradient" )
		throw clan::Exception( "Gradient resource element was not 'gradient' type" );

	clan::Colorf top_left( resource.get_element().get_attribute( "top_left" ) );
	clan::Colorf top_right( resource.get_element().get_attribute( "top_right" ) );
	clan::Colorf bottom_left( resource.get_element().get_attribute( "bottom_left" ) );
	clan::Colorf bottom_right( resource.get_element().get_attribute( "bottom_right" ) );

	return clan::Gradient( top_left, top_right, bottom_left, bottom_right );
}

clan::Sprite GraphicsManager::GetSprite( GraphicsManager::Overlay overlay, const std::string& name ) {
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
	clan::Sprite sprite;
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		sprite = clan::Sprite::resource( canvas, "Overlays/" + NameOfOverlay( overlay ) + "/sprites/" + name, resources );
	};
	IApplication::GetInstance()->RunOnMainThread(func);

	sprite.set_alignment( clan::origin_center );
	return sprite;
}

clan::Sprite GraphicsManager::GetSpriteWithImage( const clan::Image image ) {
	// TODO: Is there a better way to do this? Such as loading the spriate frame directly from the resource manager?

	clan::Sprite sprite;
	std::function<void()> func = [&] {
		clan::Canvas canvas(GET_MAIN_CANVAS());
		// Okay. Now the text should have the image on it..

		sprite =  clan::Sprite( canvas );
		sprite.add_frame( image.get_texture().get_texture().to_texture_2d() );	
		sprite.set_alignment( clan::origin_center );
	};
	IApplication::GetInstance()->RunOnMainThread(func);
	return sprite;
}

clan::Colorf GraphicsManager::LoadColor( clan::ResourceManager& resources, const std::string& path ) {
	std::string color_str = String_load( path, resources );
	return clan::Colorf( color_str );
}


GraphicsManager::Theme GraphicsManager::LoadTheme( const std::string& name ) {
	Theme theme;
	clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();

	std::string theme_path = std::string( "Themes/" + name + "/" );
	std::string gradientname = std::string( theme_path + "MenuGradient" );

	clan::XMLResourceNode resource = clan::XMLResourceManager::get_doc(resources).get_resource( gradientname );

	if( resource.get_type() != "gradient" )
		throw clan::Exception( "Gradient resource element was not 'gradient' type" );

	clan::Colorf top_left( resource.get_element().get_attribute( "top_left" ) );
	clan::Colorf top_right( resource.get_element().get_attribute( "top_right" ) );
	clan::Colorf bottom_left( resource.get_element().get_attribute( "bottom_left" ) );
	clan::Colorf bottom_right( resource.get_element().get_attribute( "bottom_right" ) );

	theme.m_menu_gradient = clan::Gradient( top_left, top_right, bottom_left, bottom_right );


	theme.m_colors[Theme::COLOR_MAIN] = LoadColor( resources, theme_path + "color_main" );
	theme.m_colors[Theme::COLOR_HP] = LoadColor( resources, theme_path + "color_hp" );
	theme.m_colors[Theme::COLOR_MP] = LoadColor( resources, theme_path + "color_mp" );
	theme.m_colors[Theme::COLOR_SP] = LoadColor( resources, theme_path + "color_sp" );
	theme.m_colors[Theme::COLOR_ACCENT] = LoadColor( resources, theme_path + "color_accent" );
	theme.m_colors[Theme::COLOR_SHADOW] = LoadColor( resources, theme_path + "color_shadow" );
	return theme;
}


void GraphicsManager::SetTheme( const std::string& theme ) {
	if( theme != m_pInstance->m_theme_name ) {
		const std::map<std::string, Theme>::const_iterator it = m_pInstance->m_theme_map.find( theme );
		if( it == m_pInstance->m_theme_map.end() ) {
			m_pInstance->m_theme_map[theme] = m_pInstance->LoadTheme( theme );
		}
		m_pInstance->m_theme = m_pInstance->m_theme_map[theme];
		m_pInstance->m_font_map.clear(); // Dump font cache
		m_pInstance->m_theme_name = theme;
	}
}

std::string GraphicsManager::GetThemeName() {
	return m_pInstance->m_theme_name;
}


void GraphicsManager::GetAvailableThemes( std::list< std::string >& o_themes ) {
	clan::ResourceManager & resources = IApplication::GetInstance()->GetResources();
	std::vector<std::string> sections = clan::XMLResourceManager::get_doc(resources).get_resource_names( "Themes" );
	for( std::vector<std::string>::const_iterator it = sections.begin(); it != sections.end(); it++ ) {
		o_themes.push_back( *it );
	}
}



clan::Colorf GraphicsManager::HSVToRGB( const StoneRing::GraphicsManager::HSVColor& color ) {
	clan::Colorf rgb_color;
	if( color.s == 0.0f ) {                      //HSV from 0 to 1
		rgb_color.set_red( color.v );
		rgb_color.set_green( color.v );
		rgb_color.set_blue( color.v );
	} else {
		float var_h = color.h * 6.0f;
		if( var_h == 6.0f )
			var_h = 0.0f;      //H must be < 1
		float var_i = int( var_h );
		float var_1 = color.h * ( 1.0f - color.s );
		float var_2 = color.h * ( 1.0f - color.s * ( var_h - var_i ) );
		float var_3 = color.v * ( 1.0f - color.s * ( 1.0 - ( var_h - var_i ) ) );

		float var_r, var_g, var_b;
		if( var_i == 0 ) {
			var_r = color.v     ;
			var_g = var_3 ;
			var_b = var_1;
		} else if( var_i == 1 ) {
			var_r = var_2 ;
			var_g = color.v     ;
			var_b = var_1;
		} else if( var_i == 2 ) {
			var_r = var_1 ;
			var_g = color.v     ;
			var_b = var_3;
		} else if( var_i == 3 ) {
			var_r = var_1 ;
			var_g = var_2 ;
			var_b = color.v;
		} else if( var_i == 4 ) {
			var_r = var_3 ;
			var_g = var_1 ;
			var_b = color.v;
		} else                   {
			var_r = color.v     ;
			var_g = var_1 ;
			var_b = var_2;
		}

		rgb_color.set_red( var_r );
		rgb_color.set_green( var_g );
		rgb_color.set_blue( var_b );
	}

	return rgb_color;
}

GraphicsManager::HSVColor GraphicsManager::RGBToHSV( const clan::Colorf& color ) {
	HSVColor hsv_color;
	float var_Min = std::min( color.get_red(), std::min( color.get_green(), color.get_blue() ) );    //Min. value of RGB
	float var_Max = std::max( color.get_red(), std::max( color.get_green(), color.get_blue() ) );   //Max. value of RGB
	float del_Max = var_Max - var_Min;             //Delta RGB value

	hsv_color.v = var_Max;

	if( del_Max == 0.0 ) {                    //This is a gray, no chroma...
		hsv_color.h = 0.0;                                //HSV results from 0 to 1
		hsv_color.s = 0.0;
	} else {                                //Chromatic data...
		hsv_color.s = del_Max / var_Max;

		float del_R = ( ( ( var_Max - color.get_red() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
		float del_G = ( ( ( var_Max - color.get_green() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
		float del_B = ( ( ( var_Max - color.get_blue() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;

		if( color.get_red() == var_Max ) hsv_color.h = del_B - del_G;
		else if( color.get_green() == var_Max ) hsv_color.h = ( 1.0 / 3.0 ) + del_R - del_B;
		else if( color.get_blue() == var_Max ) hsv_color.h = ( 2.0 / 3.0 ) + del_G - del_R;

		if( hsv_color.h < 0.0f ) hsv_color.h += 1.0f;
		if( hsv_color.h > 1.0f ) hsv_color.h -= 1.0f;
	}
	return hsv_color;
}

float GraphicsManager::RotateHue( const float hue, const float angle_rads ) {
	float radians = hue * M_PI * 2.0;
	return radians + angle_rads;
}

clan::Colorf GraphicsManager::GetAnalog( const clan::Colorf& color, bool left ) {
	float degs = left ? 30.0 : -30.0;
	return GetComplement( color, degs );
}

clan::Colorf GraphicsManager::GetOppositeColor( const clan::Colorf& color ) {
	return GetComplement( color, 180.0f );
}

clan::Colorf GraphicsManager::GetSplit( const clan::Colorf& color, bool left ) {
	return GetComplement( color, left ? 150.0f : -150.0f );
}

clan::Colorf GraphicsManager::GetTriadic( const clan::Colorf& color, bool left ) {
	return GetComplement( color, left ? 120.0f : -120.f );
}


clan::Colorf GraphicsManager::GetComplement( const clan::Colorf& color, float degs ) {
	const float deg_per_rad = 57.29577951f;
	const float radians = degs * deg_per_rad;
	HSVColor hsv = RGBToHSV( color );
	hsv.v = RotateHue( hsv.v, radians );
	return HSVToRGB( hsv );
}

void GraphicsManager::GetComplementaryColors( const clan::Colorf& color, clan::Colorf& one, clan::Colorf& two ) {
	// For now, use triadic
	one = GetTriadic( color, true );
	two = GetTriadic( color, false );
}





GraphicsManager::GraphicsManager() {
}

GraphicsManager::~GraphicsManager() {


}






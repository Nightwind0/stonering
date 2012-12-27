/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef FONT_H
#define FONT_H
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#ifndef uint
typedef unsigned int uint;
#endif
namespace StoneRing {
class GraphicsManager;
class Font {
public:
	enum Alignment {
		DEFAULT,
		CENTER,
		BOTTOM_LEFT,
		TOP_LEFT,
		ABOVE
	};
	Font();
	~Font();
	void draw_text( CL_GraphicContext &  	gc,
		const CL_Pointf &  	position,
		const CL_StringRef &  	text, Alignment = DEFAULT, float multiply = 1.0f );
	void draw_text( CL_GraphicContext & gc,
		float x, float y,
		const CL_StringRef & text, Alignment = DEFAULT, float multiply = 1.0f );
	bool is_null()  {
		return m_font.is_null();
	}
	void set_alpha( float alpha ) {
		m_alpha = alpha;
	}
	float get_alpha() const {
		return m_alpha;
	}

	CL_FontMetrics get_font_metrics( CL_GraphicContext& gc ) {
		return m_font.get_font_metrics();
	}
	CL_Size get_text_size( CL_GraphicContext& gc, const CL_StringRef& text ) {
		return m_font.get_text_size( gc, text );
	}
	void set_color(CL_Colorf color){m_color=color;}
private:
	friend class GraphicsManager;
	friend int draw_text( CL_GraphicContext&, const Font &, CL_Rectf, CL_StringRef, uint );
	float calc_offset( CL_GraphicContext& );
	CL_Colorf m_color;
	CL_Colorf m_shadow_color;
	CL_Font   m_font;
	CL_Pointf m_shadow_offset;
	float m_alpha;
};

}
#endif // FONT_H

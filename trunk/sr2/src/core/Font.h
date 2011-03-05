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
namespace StoneRing{
    class GraphicsManager;
    class Font
    {
    public:
	enum Alignment {
	    DEFAULT,
	    CENTER,
	    TOP_LEFT,
	    ABOVE
	};
	Font();
	~Font();
	void draw_text(CL_GraphicContext &  	gc,
		const CL_Pointf &  	position,
		const CL_StringRef &  	text, Alignment = DEFAULT);
	void draw_text(CL_GraphicContext & gc,
		       float x, float y,
		       const CL_StringRef & text, Alignment = DEFAULT);
	bool is_null()  { return m_font.is_null(); }
	void set_alpha(float alpha) { m_color.set_alpha(alpha); }
	float get_alpha() const { return m_color.get_alpha(); }
	CL_FontMetrics get_font_metrics(CL_GraphicContext& gc)
	{
	    return m_font.get_font_metrics(gc);
	}
	CL_Size get_text_size(CL_GraphicContext& gc, const CL_StringRef& text)
	{
	    return m_font.get_text_size(gc,text);
	}
    private:
	friend class GraphicsManager;
	friend int draw_text(CL_GraphicContext&, const Font &, CL_Rectf, CL_StringRef, uint);
	float calc_offset(CL_GraphicContext&);
	CL_Colorf m_color;
	CL_Font   m_font;
    };

}
#endif // FONT_H

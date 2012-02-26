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

#include "Font.h"

using StoneRing::Font;

Font::Font()
{
}

Font::~Font()
{
}

void Font::draw_text(CL_GraphicContext &  	gc,
		const CL_Pointf &  	position,
		const CL_StringRef &  	text,
		Alignment alignment,
                float mult
		    )
{
    CL_Pointf offset(0.0f,0.0f);
    switch(alignment)
    {
	case TOP_LEFT:
	    offset = CL_Pointf(0.0f,m_font.get_font_metrics().get_ascent() - m_font.get_font_metrics().get_internal_leading());
	    break;
	case CENTER:
	    offset = CL_Pointf(0.0f,m_font.get_font_metrics().get_ascent() / 2.0f);
	    break;
        case DEFAULT:
	case ABOVE:
	    offset = CL_Pointf(0.0f,calc_offset(gc));
	    break;
	case BOTTOM_LEFT:
	    offset = CL_Pointf(0.0f, - m_font.get_font_metrics().get_ascent() + m_font.get_font_metrics().get_internal_leading() - m_font.get_font_metrics().get_external_leading());
	    break;
	default:
	    break;
    }
    if(m_shadow_offset != CL_Pointf(0.0f,0.0f))
    {
	m_font.draw_text(gc,position + offset + m_shadow_offset, text, m_shadow_color);
    }
    
    CL_Colorf color = m_color;
    color.r *= mult;
    color.b *= mult;
    color.g *= mult;

    m_font.draw_text(gc,position + offset,text,color);
}

void Font::draw_text(CL_GraphicContext & gc,
		       float x, float y,
		       const CL_StringRef & text, Alignment alignment, float mult)
{
    draw_text(gc, CL_Pointf(x,y),text, alignment,mult);
}

float Font::calc_offset(CL_GraphicContext& gc) 
{
    	return  0.0f - ((m_font.get_font_metrics().get_height() - 
				    m_font.get_font_metrics().get_descent() -  
				    m_font.get_font_metrics().get_internal_leading())/ 2.0f);
}





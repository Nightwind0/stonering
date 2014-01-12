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

Font::Font(): m_alpha( 1.0f ) {
}

Font::~Font() {
}

void Font::draw_text( clan::Canvas &  	gc,
																						const clan::Pointf &  	position,
																						const std::string &  	text,
																						Alignment alignment,
																						float mult
																				) {
	clan::Pointf offset( 0.0f, 0.0f );
	switch( alignment ) {
		case TOP_LEFT:
			offset = clan::Pointf( 0.0f, m_font.get_font_metrics().get_ascent() - m_font.get_font_metrics().get_internal_leading() );
			break;
		case CENTER:
			offset = clan::Pointf( 0.0f, m_font.get_font_metrics().get_ascent() / 2.0f );
			break;
		case DEFAULT:
		case ABOVE:
			offset = clan::Pointf( 0.0f, calc_offset( gc ) );
			break;
		case BOTTOM_LEFT:
			offset = clan::Pointf( 0.0f, - m_font.get_font_metrics().get_ascent() + m_font.get_font_metrics().get_internal_leading() - m_font.get_font_metrics().get_external_leading() );
			break;
		default:
			break;
	}
	if( m_shadow_offset != clan::Pointf( 0.0f, 0.0f ) ) {
		clan::Colorf shadow_color = m_shadow_color;
		shadow_color.a *= m_alpha;
		m_font.draw_text( gc, position + offset + m_shadow_offset, text, shadow_color );
	}

	clan::Colorf color = m_color;
	color.r *= mult;
	color.b *= mult;
	color.g *= mult;
	color.a = m_alpha;

	m_font.draw_text( gc, position + offset, text, color );
}

void Font::draw_text( clan::Canvas & gc,
																						float x, float y,
																						const std::string & text, Alignment alignment, float mult ) {
	draw_text( gc, clan::Pointf( x, y ), text, alignment, mult );
}

float Font::calc_offset( clan::Canvas& gc ) {
	return  0.0f - ( ( m_font.get_font_metrics().get_height() -
																				m_font.get_font_metrics().get_descent() -
																				m_font.get_font_metrics().get_internal_leading() ) / 2.0f );
}





#include "sr_defines.h"


std::string IntToString(const int &i)
{
    std::ostringstream os;

    os << i;


    return os.str();

}

std::string FloatToString(const float &f)
{
    std::ostringstream os;

    os << f;

    return os.str();
}

std::string CL_String_load(const std::string& id, CL_ResourceManager& resources)
{
    CL_Resource res = resources.get_resource(id);
    if (res.get_type() != "string")
        throw CL_Exception("Resource is not string type!");

    return res.get_element().get_attribute("value");
    //return res.get_element().get_text();
}
CL_Size get_text_size(CL_Font font, CL_GraphicContext &gc, const CL_StringRef &text)
{
	CL_Size total_size;


	{
		CL_FontMetrics fm = font.get_font_metrics(gc);
		int line_spacing = fm.get_external_leading();
		std::vector<CL_String> lines = CL_StringHelp::split_text(text, "\n", false);
		for (std::vector<CL_String>::size_type i=0; i<lines.size(); i++)
		{
			CL_Size line_size = font.get_provider()->get_text_size(gc, lines[i]);

			if ((i+1) != lines.size())	// Do not add the line spacing on the last line
				line_size.height += line_spacing;

			if (total_size.width < line_size.width)	// Find the widest line
				total_size.width = line_size.width;

			total_size.height += line_size.height;
		}
	}

	return total_size;
}



int draw_text(CL_GraphicContext& gc, CL_Font font, CL_Colorf color, CL_Rectf rect, CL_StringRef string, uint string_pos)
{
    CL_StringRef str = string.substr(string_pos);
    CL_FontMetrics metrics = font.get_font_metrics(gc);

    //CL_Size testSize = font.get_text_size(gc,"Hello Hello");

    uint max_characters = 0;//rect.get_width() / metrics.get_max_character_width() - 1;



    CL_Size size;

    bool done = false;

    while(!done){
        ++max_characters;
        //size = font.get_text_size(gc,str.substr(0,max_characters));
        size = get_text_size(font,gc,str.substr(0,max_characters)); // WORKAROUND: This works around the CL bug

        if(size.width > rect.get_width()){
            // We went over width.
            // Maybe there's a word break we can find, then we can replace it with a \n
            // which clanlib will automatically handle
            size_t space = str.find_last_of(" ",max_characters);
            // TODO: See if there is a line break before the space,
            // because if there is a giant word that you can't break up, this
            // algorithm will just needlessly add \ns
            if(space != CL_StringRef::npos){
                str[space] = '\n';
            }else{
                // no space... gonna have to cut it off here by inserting a newline
                str = str.substr(0,max_characters) + "\n" + str.substr(max_characters);
            }
            --max_characters;
            continue;
        }

        if(size.height > rect.get_height()){
            // Whoops. Too  much...
            --max_characters;
            break;
        }

        if(max_characters == str.size()) break;

    }


    font.draw_text(gc,rect.left,rect.top + metrics.get_height(),string.substr(string_pos,max_characters),color);

    return max_characters;

}

template<class T>
CL_Vec2<T> operator+(const CL_Vec2<T> &a, const CL_Vec2<T> &b)
{
    CL_Vec2<T> val = a;
    a += b;

    return val;
}

template<class T>
CL_Vec2<T> operator*(const CL_Vec2<T> &a, const CL_Vec2<T> &b)
{
    CL_Vec2<T> val = a;
    a *= b;

    return val;
}


template<class T>
CL_Vec2<T> operator*(const CL_Vec2<T> &a, const T& t)
{
    CL_Vec2<T> val = a;
    a *= t;

    return val;
}



template<class T>
CL_Vec2<T> operator*(const T& t, const CL_Vec2<T> &v)
{
    return operator*(v,t);
}

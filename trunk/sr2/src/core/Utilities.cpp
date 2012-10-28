#include "sr_defines.h"
#include "GraphicsManager.h"
#include <iomanip>
#include <stdlib.h>

void WriteString(std::ostream& stream, const std::string& str)
{
    int size = str.size();
    stream.write((char*)&size,sizeof(int));
    stream.write(str.c_str(),size);
}

std::string ReadString(std::istream& stream)
{
    int size;
    stream.read((char*)&size,sizeof(int));
    char *buffer = new char[size+1];
    memset(buffer,0,size+1*sizeof(char));
    stream.read(buffer,size * sizeof(char));
   
    return std::string(buffer);
}


int StringToInt(const std::string& str)
{
	return atoi(str.c_str());
}

std::string IntToString(const int &i, int width)
{
    std::ostringstream os;

    os << std::setw(width) << i;


    return os.str();

}

std::string FloatToString(const float &f, int width, int precision)
{
    std::ostringstream os;

    os << std::setprecision(precision) << std::setw(width) << f;

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
CL_Size get_text_size(const StoneRing::Font& font, CL_GraphicContext &gc, const CL_StringRef &text)
{
	CL_Size total_size;


	{
		CL_FontMetrics fm = const_cast<StoneRing::Font&>(font).get_font_metrics(gc);
		int line_spacing = fm.get_external_leading();
		std::vector<CL_String> lines = CL_StringHelp::split_text(text, "\n", false);
		for (std::vector<CL_String>::size_type i=0; i<lines.size(); i++)
		{
			CL_Size line_size = const_cast<StoneRing::Font&>(font).get_text_size(gc, lines[i]);

			if ((i+1) != lines.size())	// Do not add the line spacing on the last line
				line_size.height += line_spacing;

			if (total_size.width < line_size.width)	// Find the widest line
				total_size.width = line_size.width;

			total_size.height += line_size.height;
		}
	}

	return total_size;
}



int draw_text(CL_GraphicContext& gc,StoneRing::Font &font, CL_Rectf rect, CL_StringRef string, uint string_pos)
{
    CL_StringRef str = string.substr(string_pos);
    if(!str.size()) return 0;
    CL_FontMetrics metrics = const_cast<StoneRing::Font&>(font).get_font_metrics(gc);

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
                str = str.substr(0,max_characters-1) + '\n' + str.substr(max_characters-1);
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

    font.draw_text(gc,rect.left,rect.top + metrics.get_height(),
                   string.substr(string_pos,max_characters));

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

CL_Colorf operator*(CL_Colorf a, CL_Colorf b)
{
    CL_Colorf color(a.r*b.r,a.g*b.g,a.b*b.b);
    return color;
}




#ifndef SR_DEFINES_H
#define SR_DEFINES_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include <sstream>
#include <string>
#include <functional>
#include <cmath>
#include <stdlib.h>
typedef unsigned int uint;
typedef unsigned short ushort;
#include "Font.h"

#ifdef WIN32
#pragma warning (disable : 4250 )
#endif

template<class T>
struct del_fun_t
{
    typedef del_fun_t result_type;
    const del_fun_t& operator()(T* p) const {
        delete p;
        return *this;
    }
};

template<class T>
del_fun_t<T> del_fun()
{
    return del_fun_t<T>();
}

template<class T>
struct get_second_t
{
    typedef T argument_type;
    typename T::second_type operator()(const T& p) const
    {
        return p.second;
    }
};

template<class Pair>
get_second_t<Pair> get_second()
{
    return get_second_t<Pair>();
}


  /* class for the compose_f_gx adapter
   */
template <class OP1, class OP2>
class compose_f_gx_t :
    public std::unary_function<typename OP2::argument_type, typename OP1::result_type>
  {
    private:
      OP1 op1;     //process: op1(op2(x))
      OP2 op2;
    public:
      //constructor
      compose_f_gx_t(const OP1& o1, const OP2& o2)
       : op1(o1), op2(o2) {
      }


      //function call
      typename OP1::result_type
          operator() (const typename OP2::argument_type& x) const {
              return op1 (op2(x));
      }
};


/*convenience functions for the compose _f_gx adapter
*/
template<class OP1,class OP2>
inline compose_f_gx_t<OP1,OP2>
compose_f_gx (const OP1& o1, const OP2& o2) {
    return compose_f_gx_t<OP1,OP2>(o1,o2);
}


std::string IntToString(const int &i, int width=1);
std::string FloatToString(const float &f, int width=3,int precision=2);
std::string CL_String_load(const std::string& id, CL_ResourceManager& resources);


inline double ranf()
{
  return (double)rand() / ((double)RAND_MAX + 1.0);
}

/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.

*/
/* normal random variate generator */
/* mean m, standard deviation s */
inline double normal_random(double m, double sigma)
{
	double x1, x2, w, y1;
	static double y2;
	static bool use_last = false;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = false;
	}
	else
	{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = true;
	}

	return( m + y1 * sigma );
}



///////////////////////////////////////////////////////////////////////
//
//       Enums
//
enum eWho {NONE, CASTER, CASTER_GROUP, TARGET, TARGET_GROUP, ALL };

enum eBattleSprite  {
            IDLE,
            ATTACK,
            USE,
            RECOIL,
            WEAK,
            DEAD
        };

#ifndef NDEBUG
extern bool gbDebugStop;
#endif


int draw_text(CL_GraphicContext& gc,  StoneRing::Font &font, CL_Rectf rect, CL_StringRef string, uint string_pos=0);


template<class T>
CL_Vec2<T> operator+(const CL_Vec2<T> &a, const CL_Vec2<T> &b);
template<class T>
CL_Vec2<T> operator*(const CL_Vec2<T> &a, const CL_Vec2<T> &b);

template<class T>
CL_Vec2<T> operator*(const CL_Vec2<T> &a, const T& t);

template<class T>
CL_Vec2<T> operator*(const T& t, const CL_Vec2<T> &v);


template<class T>
class Visitor
{
public:
    Visitor(){}
    virtual ~Visitor(){}
    
    virtual void Visit(T)=0;    
};

const CL_Pointf kEmptyPoint(0.0f,0.0f);

CL_Colorf operator*(CL_Colorf a, CL_Colorf);

void WriteString(std::ostream& stream, const std::string&);
std::string ReadString(std::istream& stream);

// Clanlib's rect::contains is broken
template <class T>
bool RectContains(const CL_Rectx<T>& rect, const CL_Vec2<T>& p){
     return ((p.x >= rect.left && p.x < rect.right) || (p.x < rect.left && p.x >= rect.right))
                    && ((p.y >= rect.top && p.y < rect.bottom) || (p.y < rect.top && p.y >= rect.bottom));
}

#endif





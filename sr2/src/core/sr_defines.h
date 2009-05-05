
#ifndef SR_DEFINES_H
#define SR_DEFINES_H

#include <sstream>
#include <string>
#include <functional>
#include <cmath>
#include <stdlib.h>
typedef unsigned int uint;
typedef unsigned short ushort;

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


std::string IntToString(const int &i);
std::string FloatToString(const float &f);

inline double ranf()
{
  return (double)rand() / (double)RAND_MAX;
}

// random dist centered around 0
inline double gaussian_random (const double sigma=1.0)
{
  double x, y, r2;

  do
    {
      /* choose x,y in uniform square (-1,-1) to (+1,+1) */

      x = -1 + 2 * ranf();
      y = -1 + 2 * ranf();

      /* see if it is in the unit circle */
      r2 = x * x + y * y;
    }
  while (r2 > 1.0 || r2 == 0);

  /* Box-Muller transform */
  return sigma * y * sqrt (-2.0 * log (r2) / r2);
}

inline double random_distribution(double center, double std_dev_ratio)
{
  return center + gaussian_random(std_dev_ratio * center);
}



///////////////////////////////////////////////////////////////////////
//
//       Enums
//
enum eWho {NONE, CASTER, CASTER_GROUP, TARGET, TARGET_GROUP, ALL };

#ifndef NDEBUG
extern bool gbDebugStop;
#endif


#endif





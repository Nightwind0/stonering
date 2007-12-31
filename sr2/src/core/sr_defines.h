
#ifndef SR_DEFINES_H
#define SR_DEFINES_H

#include <sstream>
#include <string>
#include <functional>
typedef unsigned int uint;
typedef unsigned short ushort;


#pragma warning (disable : 4250 )


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
get_second_t<typename Pair> get_second()
{
    return get_second_t<typename Pair>();
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


///////////////////////////////////////////////////////////////////////
//
//       Enums
//
enum eWho {NONE, CASTER, CASTER_GROUP, TARGET, TARGET_GROUP, ALL };

#ifndef NDEBUG
extern bool gbDebugStop;
#endif


#endif





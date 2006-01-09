#ifndef SR_DEFINES_H
#define SR_DEFINES_H

#include <sstream>
#include <string>

typedef unsigned int uint;
typedef unsigned short ushort;


#pragma warning (disable : 4250 )


template<class T>
struct del_fun_t
{
   del_fun_t& operator()(T* p) {
     delete p;
     return *this;
   }
};

template<class T>
del_fun_t<T> del_fun()
{
   return del_fun_t<T>();
}


std::string IntToString(const int &i);

#ifndef NDEBUG
extern bool gbDebugStop;
#endif


#endif

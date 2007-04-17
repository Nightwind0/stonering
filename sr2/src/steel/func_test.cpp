#include <iostream>
#include <string>
#include <map>
#include "SteelFunctor.h"


class Foo
{
public:
    Foo(){}
    virtual    ~Foo(){}
    
    SteelType foo() { return SteelType(); }
    SteelType one(int i) 
    {
        std::cout << "int:" << i << std::endl;
        return SteelType();
    }
    SteelType two(const int &i,const double &d)
    {
        std::cout << "int:" << i << " double:" << d << std::endl;
        return SteelType();
    }

private:
};


int main()
{
    std::map<std::string,SteelFunctor*> functors;
    
    Foo fooboy;
    

    functors["foo"] =  new SteelFunctorNoArgs<Foo>(&fooboy,&Foo::foo);
    functors["one"] =  new SteelFunctor1Arg<Foo,int>(&fooboy,&Foo::one);
    functors["two"] = new SteelFunctor2Arg<Foo,const int&,const double&>(&fooboy,&Foo::two);
}



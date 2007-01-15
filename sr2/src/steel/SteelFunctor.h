
#ifndef STEEL_FUNCTOR_H
#define STEEL_FUNCTOR_H

#include <string>
#include <queue>
#include "SteelType.h"


class SteelFunctor
{
public:
    SteelFunctor();
    virtual ~SteelFunctor();

    virtual SteelType Call(ParamList &params)=0;

private:
};

template<class ObjType>
class SteelFunctorNoArgs : public SteelFunctor
{
public:
    typedef SteelType (ObjType::*FuncPointer)();
    SteelFunctorNoArgs(ObjType *pObj, FuncPointer p):
	m_pFunc(p),m_pObj(pObj){}
	virtual ~SteelFunctorNoArgs(){}
    virtual SteelType Call(ParamList &params)
    {
	(m_pObj->*m_pFunc)();
    }
private:
    FuncPointer m_pFunc;
    ObjType *m_pObj;
};

template<class ObjType, class Arg1>
    class SteelFunctor1Arg : public SteelFunctor
{
public:
    typedef SteelType (ObjType::*FuncPointer)(Arg1);
    SteelFunctor1Arg(ObjType *pObj, FuncPointer p)
	:m_pFunc(p),m_pObj(pObj){}
    virtual ~SteelFunctor1Arg(){}
    virtual SteelType Call(ParamList &params)
    {
	(m_pObj->*m_pFunc)(params.next());
    }

       
private:
    FuncPointer m_pFunc;
    ObjType *m_pObj;
};

template<class ObjType, class Arg1, class Arg2>
    class SteelFunctor2Arg : public SteelFunctor
{
public:
    typedef SteelType (ObjType::*FuncPointer)(Arg1,Arg2);
    SteelFunctor2Arg(ObjType *pObj, FuncPointer p)
	:m_pFunc(p),m_pObj(pObj){}
	virtual ~SteelFunctor2Arg(){}
	virtual SteelType Call(ParamList &params)
	{
	    (m_pObj->*m_pFunc)(params.next(),params.next());
	}

private:
    FuncPointer  m_pFunc;
    ObjType * m_pObj;
};

template<class ObjType, class Arg1, class Arg2, class Arg3>
    class SteelFunctor3Arg : public SteelFunctor
{
public:
    typedef SteelType (ObjType::*FuncPointer)(Arg1,Arg2,Arg3);
    SteelFunctor3Arg(ObjType *pObj, FuncPointer p)
	:m_pFunc(p),m_pObj(pObj){}
	virtual ~SteelFunctor3Arg(){}
	virtual SteelType Call(ParamList &params)
	{
	    (m_pObj->*m_pFunc)(params.next(),
			       params.next(),
			       params.next());
	}
private:
	FuncPointer m_pFunc;
	ObjType *m_pObj;
};

template<class ObjType, class Arg1, class Arg2, class Arg3, class Arg4>
    class SteelFunctor4Arg : public SteelFunctor
{
public:
    typedef SteelType (ObjType::*FuncPointer)(Arg1,Arg2,Arg3,Arg4);
    SteelFunctor4Arg(ObjType *pObj, FuncPointer p):
	m_pFunc(p),m_pObj(pObj){}
	virtual ~SteelFunctor4Arg(){}
	virtual SteelType Call(ParamList &params)
	{
	    (m_pObj->*m_pFunc)(params.next(),
			       params.next(),
			       params.next(),
			       params.next());
	}
private:
	FuncPointer m_pFunc;
	ObjType *m_pObj;
};

/*

template <class Class>
class Thunk
{
public:
    typedef SteelType (Class::*MemberFunction)() ;
    Thunk(ParamList *pParams,Class *pObject, SteelFunctor f);
    virtual ~Thunk();

private:
};

*/
#endif

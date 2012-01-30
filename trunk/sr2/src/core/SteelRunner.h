/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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


#ifndef STEELRUNNER_H
#define STEELRUNNER_H

#include <ClanLib/core.h>
#include "sr_defines.h"
#ifdef WIN32
#include "SteelInterpreter.h"
#else
#include "steel/SteelInterpreter.h"
#endif
namespace StoneRing {
    

template <class T>
class SteelRunner : public CL_Runnable
{

public:
    SteelRunner(SteelInterpreter* pInterpreter,T* callee,void (T::*MemFunPtr)(void));
    virtual ~SteelRunner();
    virtual void run();
    void setScript(AstScript* pScript, const ParameterList& params);
    void setFunctor(SteelType::Functor pFunctor);
    SteelType getResult() const;
protected:
    SteelInterpreter * m_pInterpreter;
    T*m_callee;
    void (T::*m_callback)(void);
    AstScript* m_pScript;
    SteelType::Functor m_pFunctor;
    ParameterList m_params;
    SteelType m_result;
};


template <class T>
SteelRunner<T>::SteelRunner(SteelInterpreter* pInterpreter,T* callee,void (T::*functor)(void))
:m_pInterpreter(pInterpreter),m_callee(callee),m_callback(functor),m_pScript(NULL){

}

template <class T>
SteelRunner<T>::~SteelRunner()
{

}

template <class T>
void SteelRunner<T>::run()
{
    if(m_pScript)
        m_result = m_pInterpreter->runAst(m_pScript,m_params);
    if(m_pFunctor)
        m_result = m_pFunctor->Call(m_pInterpreter,SteelType::Container());
    (m_callee->*m_callback)();
}
template <class T>
void SteelRunner<T>::setScript(AstScript* pScript, const ParameterList& params)
{
    m_pScript = pScript;
    m_params = params;
}

template <class T>
void SteelRunner<T>::setFunctor ( SteelType::Functor pFunctor )
{
    m_pFunctor = pFunctor;
}

template <class T>
SteelType SteelRunner<T>::getResult() const
{
    return m_result;
}


}
#endif // STEELRUNNER_H

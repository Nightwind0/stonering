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
#include "SteelException.h"
#else
#include "steel/SteelInterpreter.h"
#include "steel/SteelException.h"
#endif
namespace StoneRing {
    


template <class T>
class SteelRunner : public clan::Runnable
{

public:
    SteelRunner(Steel::SteelInterpreter* pInterpreter,T* callee,void (T::*MemFunPtr)(void));
    virtual ~SteelRunner();
    virtual void run();
    void setScript(Steel::AstScript* pScript, const Steel::ParameterList& params);
    void setFunctor(Steel::SteelType::Functor pFunctor);
    Steel::SteelType getResult() const;
protected:
    Steel::SteelInterpreter * m_pInterpreter;
    T*m_callee;
    void (T::*m_callback)(void);
    Steel::AstScript* m_pScript;
    Steel::SteelType::Functor m_pFunctor;
    Steel::ParameterList m_params;
    Steel::SteelType m_result;
};


template <class T>
SteelRunner<T>::SteelRunner(Steel::SteelInterpreter* pInterpreter,T* callee,void (T::*functor)(void))
:m_pInterpreter(pInterpreter),m_callee(callee),m_callback(functor),m_pScript(NULL){

}

template <class T>
SteelRunner<T>::~SteelRunner()
{

}

template <class T>
void SteelRunner<T>::run()
{
	try{
    if(m_pScript)
        m_result = m_pInterpreter->runAst(m_pScript,m_params);
    if(m_pFunctor)
        m_result = m_pFunctor->Call(m_pInterpreter,Steel::SteelType::Container());
	}catch(Steel::SteelException e){
		std::cerr << e.getMessage() << " in " << e.getLine() << " of " << e.getScript() << std::endl;
	}catch(...){
		std::cerr << "Some kind of exception occured." << std::endl;
	}
    (m_callee->*m_callback)();
}
template <class T>
void SteelRunner<T>::setScript(Steel::AstScript* pScript, const Steel::ParameterList& params)
{
    m_pScript = pScript;
    m_params = params;
}

template <class T>
void SteelRunner<T>::setFunctor ( Steel::SteelType::Functor pFunctor )
{
    m_pFunctor = pFunctor;
}

template <class T>
Steel::SteelType SteelRunner<T>::getResult() const
{
    return m_result;
}


}
#endif // STEELRUNNER_H

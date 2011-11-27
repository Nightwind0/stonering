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


#include "SteelRunner.h"

namespace StoneRing{

SteelRunner::SteelRunner(SteelInterpreter* pInterpreter):m_pInterpreter(pInterpreter)
{

}

SteelRunner::~SteelRunner()
{

}

void SteelRunner::run()
{
    m_result = m_pInterpreter->runAst(m_pScript,m_params);
}

void SteelRunner::setScript(AstScript* pScript, const ParameterList& params)
{
    m_pScript = pScript;
    m_params = params;
}

SteelType SteelRunner::getResult() const
{
    return m_result;
}

}
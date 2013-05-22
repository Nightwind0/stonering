/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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


#ifndef COUNTDISPLAY_H
#define COUNTDISPLAY_H

#include "sr_defines.h"
#include <ClanLib/core.h>

namespace StoneRing { 
template <class T>
class TimedInterpolator
{
public:
    TimedInterpolator();
    virtual ~TimedInterpolator();
	
	void SetTime(double time);
	void Start();
	void SetStaticValue(T value);
	T GetValue()const;
	void SetRange(T start, T end);

private:
	T m_start;
	T m_end;
	double m_time;
	long m_start_time;

};


	
template<class T>
TimedInterpolator<T>::TimedInterpolator(  )
	:m_start(0),m_end(0),m_time(0.5) {

}

template<class T>
TimedInterpolator<T>::~TimedInterpolator() {

}



template<class T>
void TimedInterpolator<T>::Start() {
	m_start_time = CL_System::get_time();
}


template<class T>
T TimedInterpolator<T>::GetValue() const {
	double p = (CL_System::get_time() - m_start_time) / (m_time*1000.0);
	if(std::abs(p) < 1.0)
		return m_start + (m_end-m_start) * p;
	else return m_end;
}

template<class T>
void TimedInterpolator<T>::SetTime(double time)
{
	m_time = time;
}

template <class T>
void TimedInterpolator<T>::SetRange( T start, T end ) {
	m_start = start;
	m_end = end;
	Start();
}

template <class T>
void TimedInterpolator<T>::SetStaticValue( T value ) {
	m_start = m_end = value;
}


}

#endif // COUNTDISPLAY_H

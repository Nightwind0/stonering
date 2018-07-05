/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

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


#ifndef CALLBACK_H
#define CALLBACK_H

namespace StoneRing { 

    template <class R>
    class Callback
    {
    public:
        Callback();
        virtual ~Callback();
        virtual R invoke()=0;
    };

    template <class T, class R>
    class Callback_member_func : public Callback<R> {
    public:
        typedef R (T::*MemberFunc)(void);
        Callback_member_func(T* ptr, MemberFunc func):
            m_ptr(ptr),m_member_func(func){
        }
        virtual R invoke(){
            return m_ptr->*(m_member_func)();
        }
        virtual ~Callback_member_func();
    private:
        T* m_ptr;
        MemberFunc m_member_func;
    };


}
#endif // CALLBACK_H

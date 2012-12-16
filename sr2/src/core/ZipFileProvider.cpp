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


#include "ZipFileProvider.h"

using namespace Steel;

namespace StoneRing { 
	
ZipFile::ZipFile( CL_VirtualDirectory vd ): m_vd(vd)
{

}

ZipFile::~ZipFile()
{

}


	
bool ZipFile::open( const char* path )
{
	m_io = m_vd.open_file_read(path);
	m_eof = false;
	return !m_io.is_null();
}

bool ZipFile::eof() const
{
	return m_eof;
}

void ZipFile::close()
{

}

int ZipFile::read( char* buffer, int bytes )
{
	int r = m_io.read(buffer,bytes);
	if (r < bytes)
		m_eof = true;
	return r;
}

	
ZipFileProvider::ZipFileProvider() {

}

ZipFileProvider::~ZipFileProvider() {

}

void ZipFileProvider::SetVirtualDirectory( CL_VirtualDirectory vd )
{
	m_vd = vd;
}


IFile* ZipFileProvider::create()
{
	return new ZipFile(m_vd);
}


}

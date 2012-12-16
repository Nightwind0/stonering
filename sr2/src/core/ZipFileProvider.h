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


#ifndef ZIPFILEPROVIDER_H
#define ZIPFILEPROVIDER_H

#include <ClanLib/core.h>

#ifdef _WINDOWS_
#include "File.h"
#else
#include <steel/File.h>
#endif

namespace StoneRing { 
	
class ZipFile: public Steel::IFile {
public:
	ZipFile(CL_VirtualDirectory vd);
	virtual ~ZipFile();
	virtual bool eof()const;
	virtual int  read(char *buffer, int bytes);
	virtual void close();
	virtual bool open(const char* path);
private:
	CL_VirtualDirectory m_vd;
	CL_IODevice m_io;
	bool m_eof;
};

class ZipFileProvider : public Steel::IFileProvider{
public:
	ZipFileProvider();
	virtual ~ZipFileProvider();
	void SetVirtualDirectory(CL_VirtualDirectory vd);
	Steel::IFile* create();
private:
	CL_VirtualDirectory m_vd;
};


}
#endif // ZIPFILEPROVIDER_H


/* This class modified from the ClanLib Game Programming Library for specific
** use within Stone Ring. 
**
** Changes Copyright (c) Daniel Palm 2005
**
** The original notice follows:
**/


/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/


#include <ClanLib/core.h>
#include <ClanLib/gui.h>
#include "filedialog_generic.h"
#include "filedialog.h"

/////////////////////////////////////////////////////////////////////////////
// Construction:

SR_FileDialog::SR_FileDialog(
	CL_Component *parent,
	CL_StyleManager *style)
: CL_Window(parent, style), impl(NULL)
{
	impl = new SR_FileDialog_Generic(this, "", "", "*");
}

SR_FileDialog::SR_FileDialog(
	const std::string &title,
	const std::string &file,
	const std::string &filter,
	CL_Component *parent,
	CL_StyleManager *style)
: CL_Window(parent, style), impl(NULL)
{
	impl = new SR_FileDialog_Generic(this, title, file, filter);
}

SR_FileDialog::~SR_FileDialog()
{
	delete impl;
}

const std::string SR_FileDialog::open(
	CL_Component *parent)
{
	SR_FileDialog filedialog("Open File", "", "*.*", parent);
	filedialog.run();

	return filedialog.get_file();
}

const std::string SR_FileDialog::open(
	const std::string &file,
	const std::string &filter,
	CL_Component *parent)
{
	SR_FileDialog filedialog("Open File", file, filter, parent);
	filedialog.run();

	return filedialog.get_file();
}

const std::string SR_FileDialog::save(
	CL_Component *parent)
{
	SR_FileDialog filedialog("Save File", "", "*.*", parent);
	filedialog.run();

	return filedialog.get_file();
}

const std::string SR_FileDialog::save(
	const std::string &file,
	const std::string &filter,
	CL_Component *parent)
{
	SR_FileDialog filedialog("Save File", file, filter, parent);
	filedialog.run();

	return filedialog.get_file();
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

const std::string &SR_FileDialog::get_file() const
{
	return impl->get_file();
}

const std::string &SR_FileDialog::get_filter() const
{
	return impl->get_filter();
}

const std::string &SR_FileDialog::get_dir() const
{
	return impl->get_dir();
}

const std::string &SR_FileDialog::get_path() const
{
	return impl->get_path();
}

bool SR_FileDialog::is_hidden_files_visible() const
{
	return impl->is_hidden_files_visible();
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

void SR_FileDialog::set_file(const std::string &filename)
{
	impl->set_file(filename);
}

void SR_FileDialog::set_dir(const std::string &dir)
{
	impl->set_dir(dir);
}

void SR_FileDialog::set_filter(const std::string &filter)
{
	impl->set_filter(filter);
}

void SR_FileDialog::show_hidden_files(bool enable)
{
	impl->show_hidden_files(enable);
}

void SR_FileDialog::refresh()
{
	impl->refresh();
}

/////////////////////////////////////////////////////////////////////////////
// Signals:

CL_Signal_v1<const std::string &> &SR_FileDialog::sig_file_highlighted()
{
	return impl->sig_file_highlighted;
}

CL_Signal_v1<const std::string &> &SR_FileDialog::sig_file_selected()
{
	return impl->sig_file_selected;
}

CL_Signal_v1<const std::string &> &SR_FileDialog::sig_dir_entered()
{
	return impl->sig_dir_entered;
}

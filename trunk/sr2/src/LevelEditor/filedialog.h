
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

//! clanGUI="Controls"
//! header=gui.h

#ifndef _srheader_file_dialog
#define _srheader_file_dialog


#if _MSC_VER > 1000
#pragma once
#endif

#include <ClanLib/gui.h>

class SR_FileDialog_Generic;

//: File dialog component
//- !group=GUI/Controls!
//- !header=gui.h!
class SR_FileDialog : public CL_Window
{
//! Construction:
public:
	//: File dialog constructor
	SR_FileDialog(
		CL_Component *parent,
		CL_StyleManager *style = NULL);

	//: File dialog constructor
	SR_FileDialog(
		const std::string &title,
		const std::string &file,
		const std::string &filter,
		CL_Component *parent,
		CL_StyleManager *style = NULL);

	//: File dialog destructor
	virtual ~SR_FileDialog();

	//: Open
	static const std::string open(
		CL_Component *parent);

	//: Open
	static const std::string open(
		const std::string &file,
		const std::string &filter,
		CL_Component *parent);

	//: Save
	static const std::string save(
		CL_Component *parent);

	//: Save
	static const std::string save(
		const std::string &file,
		const std::string &filter,
		CL_Component *parent);

//! Attributes:
public:
	//: Returns the selected file name. 
	const std::string &get_file() const;

	//: Returns the filter which the user has chosen in the file dialog.
	const std::string &get_filter() const;

	//: Returns the active directory in the file dialog.
	const std::string &get_dir() const;

	//: Returns the active directory path in the file dialog. 
	const std::string &get_path() const;

	//: Returns true if hidden files are shown in the filedialog, else false.
	bool is_hidden_files_visible() const;

//! Operations:
public:
	//: Sets the default selection to filename.
	//- <p>If filename is absolute, set_dir() is also called.</p>
	void set_file(const std::string &filename);

	//: Sets a directory path for the file dialog. 
	void set_dir(const std::string &dir);

	//: Sets the filter to filter.
	void set_filter(const std::string &filter);

	//: If enable is true, hidden files are shown in the filedialog, else no hidden files are shown. 
	void show_hidden_files(bool enable = true);

	//: Re-reads the active directory in the file dialog. 
	//- <p>It is seldom necessary to call this function.
	//- It is provided in case the directory contents change and you want to refresh the directory listbox.</p>
	void refresh();

//! Signals:
public:
	//: This signal is emitted when the user highlights a file. 
	CL_Signal_v1<const std::string &> &sig_file_highlighted();

	//: This signal is emitted when the user selects a file. 
	CL_Signal_v1<const std::string &> &sig_file_selected();

	//: This signal is emitted when the user has selected a new directory. 
	CL_Signal_v1<const std::string &> &sig_dir_entered();

//! Implementation:
private:
// TODO: Fix this copy constructor
//	CL_FileDialog(const CL_FileDialog &copy) : CL_Component(NULL, NULL) { return; } // disallow copy construction.
	SR_FileDialog_Generic *impl;
};

#endif

/* This class modified from the ClanLib Game Programming Library for specific
** use within Stone Ring. 
**
** Changes Copyright (c) Daniel Palm 2005
**
** The original notice follows:
**/


/*  $Id$
**
**  ClanLib Game SDK
**  Copyright (C) 2003  The ClanLib Team
**  For a total list of contributers see the file CREDITS.
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
**
*/

//! clanGUI="Controls"
//! header=gui.h

#ifndef srheader_treeitem
#define srheader_treeitem

#if _MSC_VER > 1000
#pragma once
#endif

#include <ClanLib/gui.h>

class SR_TreeItem_Generic;
class SR_TreeNode;
class CL_Surface;
class CL_CheckBox;

//: TreeView_Item component
//- !group=GUI/Controls!
//- !header=gui.h!
class SR_TreeItem : public CL_Component
{
//! Construction:
public:
	//: TreeView Item Constructor
	SR_TreeItem(
		SR_TreeNode *node,
		CL_Component *parent,
		CL_StyleManager *style = NULL);

	//: TreeView Item Destructor
	virtual ~SR_TreeItem();

//! Attributes:
public:
	//: Returns the item text for a specified column.
	//return: The item text.
	const std::string &get_text(int column = 0) const;

	//: Returns the item component for a specified column.
	//return: The item component.
	CL_Component *get_component(int column = 0) const;

	//: Returns the TreeView node this component is used in.
	SR_TreeNode *get_node() const;

	//: Return the icon.
	CL_Surface *get_icon() const;

	//: Get the margin between the start of the item and start of the text.
	int get_text_margin(void) const;

	//: Return the height.
	int get_custom_height() const;

//! Operations:
public:
	//: Sets the text for a specified column.
	//param text: The column text.
	void set_text(int column, const std::string &text);

	//: Adds a checkbox to a specified column.
	CL_CheckBox *set_checkbox(int column, const std::string &text = std::string());

	//: Adds a component for a specified column.
	void set_component(int column, CL_Component *component);

	//: Sets the items icon.
	//- <p>If 'delete_surface' is true, the surface will be deleted when the component is deleted.</p>
	void set_icon(CL_Surface *surface, bool delete_surface = false);

	//: Set the margin between the start of the item and start of the text.
	//: Use this to set a margin if you have no icon on this item, but on others.
	void set_text_margin(int margin);

	//: Sets the height.
	void set_custom_height(int height);

//! Signals:
public:
	//: This signal is emitted when the item is clicked on.
	CL_Signal_v0 &sig_clicked();

//! Implementation:
private:
	SR_TreeItem(const SR_TreeItem &copy) : CL_Component(NULL, NULL) { return; } // disallow copy construction.
	SR_TreeItem_Generic *impl;
};

#endif

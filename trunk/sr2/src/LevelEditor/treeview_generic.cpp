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


#include "treeview_generic.h"
#include "treenode_generic.h"
#include <ClanLib/display.h>
#include <ClanLib/gui.h>

const static std::string blank;

/////////////////////////////////////////////////////////////////////////////
// Construction:

SR_TreeView_Generic::SR_TreeView_Generic(SR_TreeView *self)
: show_root_decoration(true),
  show_header(true),
  treeview(self)
{
	// Create client area (which is the main treeview)
	CL_Component *client_area = new CL_Component(treeview);
	treeview->set_client_area(client_area);
	
	slot_paint_children = client_area->sig_paint_children().connect_virtual(
		this, &SR_TreeView_Generic::on_paint_children);
}

SR_TreeView_Generic::~SR_TreeView_Generic()
{
	delete treeview->get_client_area();
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

int SR_TreeView_Generic::get_column_count() const
{
	return columns.size();
}

int SR_TreeView_Generic::get_column_width(int index) const
{
	if(columns.size() == 0)
		return treeview->get_client_area()->get_width();

	if((unsigned int)index > columns.size())
		return 0;

	return columns[index].width;
}

const std::string &SR_TreeView_Generic::get_column_name(int index) const
{
	if((unsigned int)index > columns.size())
		return blank;

	return columns[index].name;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

int SR_TreeView_Generic::add_column(const std::string &name, int width)
{
	Column column;
	column.name = name;
	column.width = width;
	columns.push_back(column);
	return get_column_count() - 1;
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks:

void SR_TreeView_Generic::on_paint_children(CL_SlotParent_v0 &super)
{
  	CL_Point point(0, 0);
  	treeview->SR_TreeNode::impl->draw_nodes(point);
}

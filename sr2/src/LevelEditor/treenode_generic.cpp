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


#include "treenode.h"
#include "treeview.h"
#include <ClanLib/display.h>
#include "treenode_generic.h"

/////////////////////////////////////////////////////////////////////////////
// Construction:

SR_TreeNode_Generic::SR_TreeNode_Generic(SR_TreeNode *self, SR_TreeNode *parent, SR_TreeView *root)
{
	treenode = self;
	this->root = root;
	this->parent = parent;
	component = NULL;
	collapsed = false;
	selected = false;
	selectable = 2;
	collapsable = 2;
	userdata = NULL;
	root_node = false;
}

SR_TreeNode_Generic::~SR_TreeNode_Generic()
{
	clear(false);
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

void SR_TreeNode_Generic::set_collapsed(bool new_collapsed)
{
	if(collapsed == new_collapsed || children.empty())
		return;
	collapsed = new_collapsed;

	if(collapsed)
	{
		show_nodes(false);
		root->sig_item_collapsed()(*treenode);
	}
	else
	{
		show_nodes(true);
		root->sig_item_expanded()(*treenode);
	}
}

void SR_TreeNode_Generic::show_nodes(bool show)
{
	std::list<SR_TreeNode *>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
	{
		(*it)->impl->component->show(show);
		(*it)->impl->show_nodes(show);
	}
}

void SR_TreeNode_Generic::draw_nodes(CL_Point &point)
{
	std::list<SR_TreeNode *>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
	{
		int x = point.x;

		root->sig_paint_node()(*it, point);
		if((*it)->is_collapsed() == false)
			(*it)->impl->draw_nodes(point);

		point.x = x;
	}
}

void SR_TreeNode_Generic::set_component(CL_Component *new_component)
{
	if(component)
	{
		component->sig_mouse_up().disconnect(slot_mouse_up);
		delete component;
	}

	component = new_component;
	slot_mouse_up = component->sig_mouse_up().connect(
		this, &SR_TreeNode_Generic::on_child_click);
}

void SR_TreeNode_Generic::clear(bool do_signal)
{
	if(component)
	{
		component->sig_mouse_up().disconnect(slot_mouse_up);
		delete component;
		component = NULL;
	}

	std::list<SR_TreeNode *>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
		delete (*it);
	children.clear();

	if(do_signal)
		root->sig_clear()();
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks:

void SR_TreeNode_Generic::on_child_click(const CL_InputEvent &key)
{
	if(key.id == CL_MOUSE_LEFT)
	{
		if(treenode->is_collapsable())
			treenode->set_collapsed(!collapsed);
		if(treenode->is_selectable())
			treenode->set_selected();
//		else
//			root->sig_item_clicked()(*treenode);
	}
}

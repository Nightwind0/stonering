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



#include "treeview.h"
#include "treeitem.h"
#include "treenode.h"
#include "treenode_generic.h"

/////////////////////////////////////////////////////////////////////////////
// Construction:

SR_TreeNode::SR_TreeNode(SR_TreeNode *parent, SR_TreeView *root)
{
	impl = new SR_TreeNode_Generic(this, parent, root);
}

SR_TreeNode::~SR_TreeNode()
{
//	delete impl;
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

void *SR_TreeNode::get_userdata() const
{
	return impl->userdata;
}

CL_Component *SR_TreeNode::get_component() const
{
	return impl->component;
}

bool SR_TreeNode::is_selectable() const
{
	switch(impl->selectable)
	{
	case 0:
		return false;
		break;
	case 1:
		return true;
		break;
	default:
		if(impl->parent)
            return impl->parent->is_selectable();
		else
			return true;
		break;
	}
}

bool SR_TreeNode::is_collapsable() const
{
	switch(impl->collapsable)
	{
	case 0:
		return false;
		break;
	case 1:
		return true;
		break;
	default:
		if(impl->parent)
            return impl->parent->is_collapsable();
		else
			return true;
		break;
	}
}

bool SR_TreeNode::is_selected() const
{
	return impl->selected;
}

bool SR_TreeNode::is_collapsed() const
{
	return impl->collapsed;
}

bool SR_TreeNode::has_children() const
{
	return (impl->children.empty() == false);
}

bool SR_TreeNode::is_root() const
{
	return impl->root_node;
}

SR_TreeNode *SR_TreeNode::get_current_item() const
{
	// Do a depth-first search for current selected item.
	std::list<SR_TreeNode *>::const_iterator it;
	for (it = impl->children.begin(); it != impl->children.end(); ++it)
	{
		if((*it)->impl->selected)
			return (*it);
		else 
		{
			SR_TreeNode *node = (*it)->get_current_item();
			if(node)
				return node;
		}
	}
	return NULL;
}

SR_TreeNode *SR_TreeNode::get_parent() const
{
	return impl->parent;
}

int SR_TreeNode::get_depth() const
{
	if(impl->parent == 0)
		return 0;
	else
		return impl->parent->get_depth() + 1;
}

SR_TreeView *SR_TreeNode::get_treeview() const
{
	return impl->root;
}

int SR_TreeNode::get_placement_offset() const
{
	return impl->placement_offset;
}

int SR_TreeNode::get_items_height() const
{
	int height = 0;
	if(impl->component && impl->component->is_visible(false))
		height = impl->component->get_height();

	std::list<SR_TreeNode *>::iterator it;
	for (it = impl->children.begin(); it != impl->children.end(); ++it)
		height += (*it)->get_items_height();

	return height;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

SR_TreeItem *SR_TreeNode::insert_item(
	const std::string &label1, 
	const std::string &label2, 
	const std::string &label3, 
	const std::string &label4, 
	const std::string &label5, 
	const std::string &label6, 
	const std::string &label7, 
	const std::string &label8) 
{
	SR_TreeNode *node = new SR_TreeNode(this, impl->root);
	impl->children.push_back(node);

	if(impl->root == this)
		node->impl->root_node = true;

	SR_TreeItem *treeitem = new SR_TreeItem(node, impl->root->get_client_area());
	treeitem->set_text(0, label1);
	treeitem->set_text(1, label2);
	treeitem->set_text(2, label3);
	treeitem->set_text(3, label4);
	treeitem->set_text(4, label5);
	treeitem->set_text(5, label6);
	treeitem->set_text(6, label7);
	treeitem->set_text(7, label8);
	treeitem->find_preferred_size();

	node->set_component(treeitem);

	impl->root->sig_item_added()(*node);

	return treeitem;
}

SR_TreeNode *SR_TreeNode::insert_item(
	CL_Component *component1, 
	CL_Component *component2, 
	CL_Component *component3, 
	CL_Component *component4, 
	CL_Component *component5, 
	CL_Component *component6, 
	CL_Component *component7, 
	CL_Component *component8)
{
	SR_TreeNode *node = new SR_TreeNode(this, impl->root);
	impl->children.push_back(node);

	if(impl->root == this)
		node->impl->root_node = true;

	impl->root->get_client_area()->add_child(component1);

	node->set_component(component1);

	impl->root->sig_item_added()(*node);

	return node;
}

bool SR_TreeNode::remove_item(SR_TreeNode *node)
{
	std::list<SR_TreeNode *>::iterator it;

	// First check all children
	for(it = impl->children.begin(); it != impl->children.end(); ++it)
	{
		if((*it) == node)
		{
			impl->root->sig_item_removed()(*node);
			delete (*it);
			impl->children.erase(it);
			return true;
		}
	}

	// Then search all children
	for(it = impl->children.begin(); it != impl->children.end(); ++it)
		if((*it)->remove_item(node))
			return true;

	return false;
}

void SR_TreeNode::set_component(CL_Component *component)
{
	impl->set_component(component);
}

void SR_TreeNode::set_collapsed(bool collapsed)
{
	impl->set_collapsed(collapsed);
}
	
void SR_TreeNode::set_selected(bool select)
{
	impl->root->clear_selection();
	impl->selected = select;
	impl->root->sig_selection_changed()(*this);
//	impl->root->sig_item_clicked()(*this);
}

void SR_TreeNode::set_selectable(bool enable)
{
	impl->selectable = enable;
}

void SR_TreeNode::set_collapsable(bool enable)
{
	impl->collapsable = enable;
}

void SR_TreeNode::set_selected(SR_TreeNode *node, bool select)
{
	node->set_selected(select);
}

void SR_TreeNode::clear_selection()
{
	impl->selected = false;

	std::list<SR_TreeNode *>::iterator it;
	for (it = impl->children.begin(); it != impl->children.end(); ++it)
		(*it)->clear_selection();
}

void SR_TreeNode::clear()
{
	impl->clear(true);
}

void SR_TreeNode::set_userdata(void *data)
{
	impl->userdata = data;
}

void SR_TreeNode::set_placement_offset(int offset)
{
	impl->placement_offset = offset;
}

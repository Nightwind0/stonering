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

#ifndef srheader_treenode_generic
#define srheader_treenode_generic

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include <ClanLib/signals.h>
#include <ClanLib/display.h>


class SR_TreeNode;

//: TreeView node
class SR_TreeNode_Generic
{
public:
	SR_TreeNode_Generic(SR_TreeNode *self, SR_TreeNode *parent, SR_TreeView *root);
	~SR_TreeNode_Generic();
	
	void set_collapsed(bool new_collapsed);
	void show_nodes(bool show);
	void draw_nodes(CL_Point &point);
	void set_component(CL_Component *new_component);
	void clear(bool do_signal);

	SR_TreeView *root;
	SR_TreeNode *parent;
	SR_TreeNode *treenode;

	CL_Component *component;

	std::list<SR_TreeNode *> children;

	CL_SlotContainer slots;
	CL_Slot slot_mouse_up;

	void *userdata;

	bool collapsed;
	bool selected;
	int selectable;	 // 0 - not selectable, 1 - selectable, 2 - use parent setting
	int collapsable; // 0 - not collapsable, 1 - collapsable, 2 - use parent setting
	bool root_node;

	int placement_offset;

	void on_child_click(const CL_InputEvent &key);

	CL_Signal_v1<CL_Point &> sig_paint_node;
};

#endif

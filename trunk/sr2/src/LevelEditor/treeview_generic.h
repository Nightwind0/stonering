

/* This class modified from the ClanLib Game Programming Library for specific
** use within Stone Ring. 
**
** Changes Copyright (c) Daniel Palm 2005
**
** The original notice follows:
**/




/*  $Id$
	
	ClanGUI, copyrights by various people. Have a look in the CREDITS file.
	
	This sourcecode is distributed using the Library GNU Public Licence,
	version 2 or (at your option) any later version. Please read LICENSE
	for details.
*/

#ifndef srheader_treeview_generic
#define srheader_treeview_generic

#if _MSC_VER > 1000
#pragma once
#endif

#include "treeview.h"

class SR_TreeView_Generic
{
//! Construction:
public:
	SR_TreeView_Generic(SR_TreeView *self);
	~SR_TreeView_Generic();

//! Attributes:
public:
	int get_column_count() const;
	int get_column_width(int index) const;
	const std::string &get_column_name(int index) const;

	bool show_root_decoration;
	bool show_header;

//! Operations:
public:
	int add_column(const std::string &name, int width);
		
//! Signals:
public:
	CL_Signal_v2<SR_TreeNode *, CL_Point &> sig_paint_node;
	CL_Signal_v1<const SR_TreeNode &> sig_selection_changed;
	CL_Signal_v1<const SR_TreeNode &> sig_item_clicked;
	CL_Signal_v1<const SR_TreeNode &> sig_item_added;
	CL_Signal_v1<const SR_TreeNode &> sig_item_removed;
	CL_Signal_v1<const SR_TreeNode &> sig_item_expanded;
	CL_Signal_v1<const SR_TreeNode &> sig_item_collapsed;
	CL_Signal_v1<int> sig_column_added;
	CL_Signal_v1<int> sig_column_removed;
	CL_Signal_v0 sig_clear;

//! Callbacks:
private:
	void on_paint_children(CL_SlotParent_v0 &super);

//! Implementation:
private:
	struct Column
	{
		std::string name;
		int original_width;
		int width;
	};
	std::vector<Column> columns;

	CL_Slot slot_paint_children;

	SR_TreeView *treeview;
};

#endif

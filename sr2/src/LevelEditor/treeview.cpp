
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



#ifdef _MSC_VER
#pragma warning (disable:4355)
#endif
#include <ClanLib/gui.h>

#include "treeview_generic.h"

/////////////////////////////////////////////////////////////////////////////
// Construction:

SR_TreeView::SR_TreeView(
	CL_Component *parent,
	CL_StyleManager *style)
: CL_Component(parent, style), SR_TreeNode(NULL, this), impl(NULL)
{
	impl = new SR_TreeView_Generic(this);
	get_style_manager()->connect_styles("treeview", this);
	find_preferred_size();
}

SR_TreeView::SR_TreeView(
	const CL_Rect &pos,
	CL_Component *parent,
	CL_StyleManager *style)
: CL_Component(pos, parent, style), SR_TreeNode(NULL, this), impl(NULL)
{
	impl = new SR_TreeView_Generic(this);
	get_style_manager()->connect_styles("treeview", this);
}

SR_TreeView::~SR_TreeView()
{
	clear();
	delete impl;
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

int SR_TreeView::get_column_count() const
{
	return impl->get_column_count();
}

int SR_TreeView::get_column_width(int index) const
{
	return impl->get_column_width(index);
}

const std::string &SR_TreeView::get_column_name(int index) const
{
	return impl->get_column_name(index);
}

bool SR_TreeView::is_root_decoration_visible() const
{
	return impl->show_root_decoration;
}

bool SR_TreeView::is_header_visible() const
{
	return impl->show_header;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

int SR_TreeView::add_column(const std::string &name, int width)
{
	int index = impl->add_column(name, width);
	impl->sig_column_added(index);
	return index;
}

void SR_TreeView::show_root_decoration(bool enable)
{
	impl->show_root_decoration = enable;
}

void SR_TreeView::show_header(bool enable)
{
	impl->show_header = enable;
	sig_resize()(get_width(), get_height());
}

/////////////////////////////////////////////////////////////////////////////
// Signals:

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_selection_changed()
{
	return impl->sig_selection_changed;
}

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_item_clicked()
{
	return impl->sig_item_clicked;
}

CL_Signal_v2<SR_TreeNode *, CL_Point &> &SR_TreeView::sig_paint_node()
{
	return impl->sig_paint_node;
}

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_item_added()
{
	return impl->sig_item_added;
}

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_item_removed()
{
	return impl->sig_item_removed;
}

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_item_expanded()
{
	return impl->sig_item_expanded;
}

CL_Signal_v1<const SR_TreeNode &> &SR_TreeView::sig_item_collapsed()
{
	return impl->sig_item_collapsed;
}

CL_Signal_v1<int> &SR_TreeView::sig_column_added()
{
	return impl->sig_column_added;
}

CL_Signal_v1<int> &SR_TreeView::sig_column_removed()
{
	return impl->sig_column_removed;
}

CL_Signal_v0 &SR_TreeView::sig_clear()
{
	return impl->sig_clear;
}

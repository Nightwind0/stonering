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

#ifndef srheader_treeitem_generic
#define srheader_treeitem_generic

#if _MSC_VER > 1000
#pragma once
#endif

#include "treeitem.h"

class SR_TreeNode;

class SR_TreeItem_Generic
{
//! Construction:
public:
	SR_TreeItem_Generic(SR_TreeItem *self, SR_TreeNode *node);
	~SR_TreeItem_Generic();

//! Attributes:
public:
	struct TreeItemStruct
	{
		TreeItemStruct() { component = NULL; }

		CL_Component *component;
		std::string text;
	};
	std::vector<TreeItemStruct> items;

	SR_TreeNode *node;

	CL_Surface *sur_icon;
	bool delete_sur_icon;
	int custom_height;
	int text_margin;

//! Signals:
public:
	CL_Signal_v0 sig_clicked;

//! Callbacks:
private:
	void on_mouse_down(const CL_InputEvent &key);

//! Implementation:
private:
	SR_TreeItem *item;

	CL_Slot slot_mouse_down;
};

#endif

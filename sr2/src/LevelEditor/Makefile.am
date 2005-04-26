
bin_PROGRAMS = editor

editor_SOURCES = editor.cpp\
		 GridPoint.cpp\
		 MapGrid.cpp \
		 TileSelector.cpp \
		 EditableLevel.cpp \
		treeview.cpp \
		treeview_generic.cpp \
		treenode.cpp \
		treenode_generic.cpp \
		treeitem.cpp \
		treeitem_generic.cpp \
		filedialog.cpp \
		filedialog_generic.cpp \
		Infobar.cpp

editor_LDADD = $(CLANLIB_LIBS) \
		$(top_srcdir)/src/libstonering.la

INCLUDES = -I$(top_srcdir)/src


editor_CPPFLAGS = $(CLANLIB_CFLAGS)

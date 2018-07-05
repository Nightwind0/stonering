
bin_PROGRAMS = editor

editor_SOURCES = editor.cpp\
		 GridPoint.cpp\
		 MapGrid.cpp \
		 TileSelector.cpp \
		Infobar.cpp \
		EditorElements.cpp \
		EditorElementFactory.cpp


editor_LDADD = $(CLANLIB_LIBS) \
		$(top_srcdir)/src/libstonering.la


INCLUDES =  	-I@top_srcdir@/src/ \
		-I@top_srcdir@/src/states/ \
		-I@top_srcdir@/src/level/  \
		-I@top_srcdir@/src/party/ \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/items/weapons \
		-I@top_srcdir@/src/items/armor \
		-I@top_srcdir@/src/character \
		-I@top_srcdir@/src/abilities \
		-I@top_srcdir@/src/monsters \
		-I@top_srcdir@/src/core \
		-I@top_srcdir@/src/steel



editor_CPPFLAGS = $(CLANLIB_CFLAGS)

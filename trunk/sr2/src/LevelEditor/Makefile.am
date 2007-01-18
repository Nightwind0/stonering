
bin_PROGRAMS = editor

editor_SOURCES = editor.cpp\
		 GridPoint.cpp\
		 MapGrid.cpp \
		 TileSelector.cpp \
		 EditableLevel.cpp \
		Infobar.cpp \
		$(top_srcdir)/src/Element.cpp \
		$(top_srcdir)/src/GraphicsManager.cpp \
		$(top_srcdir)/src/Utilities.cpp \
		$(top_srcdir)/src/Animation.cpp \
		$(top_srcdir)/src/factories/LevelFactory.cpp \
		$(top_srcdir)/src/factories/ItemFactory.cpp \
		$(top_srcdir)/src/factories/AbilityFactory.cpp

editor_LDADD = $(CLANLIB_LIBS) \
		$(top_srcdir)/src/states/libstates.la \
		$(top_srcdir)/src/level/liblevel.la \
		$(top_srcdir)/src/party/libparty.la \
		$(top_srcdir)/src/items/libitems.la \
		$(top_srcdir)/src/character/libcharacter.la \
		$(top_srcdir)/src/abilities/libabilities.la \
		$(top_srcdir)/src/steel/libsteel.la


INCLUDES =  	-I@top_srcdir@/src/ \
		-I@top_srcdir@/src/states/ \
		-I@top_srcdir@/src/level/  \
		-I@top_srcdir@/src/level/actions  \
		-I@top_srcdir@/src/level/checks  \
		-I@top_srcdir@/src/party/ \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/items/weapons \
		-I@top_srcdir@/src/items/armor \
		-I@top_srcdir@/src/character \
		-I@top_srcdir@/src/abilities \
		-I@top_srcdir@/src/steel



editor_CPPFLAGS = $(CLANLIB_CFLAGS)

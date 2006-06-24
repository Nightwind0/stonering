SUBDIRS = abilities character factories items level party states

bin_PROGRAMS = stonering

stonering_SOURCES =	Application.cpp \
			Animation.cpp \
			Element.cpp \
			GraphicsManager.cpp \
			Utilities.cpp


stonering_LDADD = $(CLANLIB_LIBS) \
			@top_srcdir@/src/abilities/libabilities.la \
			@top_srcdir@/src/character/libcharacter.la \
			@top_srcdir@/src/factories/libfactories.la \
			@top_srcdir@/src/items/libitems.la \
			@top_srcdir@/src/level/liblevel.la \
			@top_srcdir@/src/party/libparty.la \
			@top_srcdir@/src/states/libstates.la	

stonering_CPPFLAGS = $(CLANLIB_CFLAGS) \
			$(CPPFLAGS)
stonering_CFLAGS = $(CLANLIB_CFLAGS) 

INCLUDES =  -I@top_srcdir@/src/states/ \
		-I@top_srcdir@/src/level/  \
		-I@top_srcdir@/src/party/ \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/character \
		-I@top_srcdir@/src/abilities

SUBDIRS = core abilities character factories items level party states steel

noinst_LTLIBRARIES = libstonering.la

bin_PROGRAMS = stonering

stonering_LDADD = libstonering.la

stonering_SOURCES =	Application.cpp 

stonering_CPPFLAGS = $(CLANLIB_CFLAGS) \
			$(CPPFLAGS)
stonering_CFLAGS = $(CLANLIB_CFLAGS) 

libstonering_la_SOURCES = 

libstonering_la_LIBADD = $(CLANLIB_LIBS) \
			@top_srcdir@/src/abilities/libabilities.la \
			@top_srcdir@/src/character/libcharacter.la \
			@top_srcdir@/src/factories/libfactories.la \
			@top_srcdir@/src/items/libitems.la \
			@top_srcdir@/src/level/liblevel.la \
			@top_srcdir@/src/party/libparty.la \
			@top_srcdir@/src/states/libstates.la \
			@top_srcdir@/src/core/libcore.la \
			@top_srcdir@/src/steel/libsteel.la	


INCLUDES = \
		-I@top_srcdir@/src/states/ \
		-I@top_srcdir@/src/core/ \
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

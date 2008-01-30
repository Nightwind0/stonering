SUBDIRS = core abilities character factories items level party states monsters

noinst_LTLIBRARIES = libstonering.la

bin_PROGRAMS = stonering

stonering_LDADD = libstonering.la \
				 states/libstates.la \
				-lsteel

stonering_SOURCES =	Application.cpp 

stonering_CPPFLAGS = $(CLANLIB_CFLAGS) \
			$(CPPFLAGS)
stonering_CFLAGS = $(CLANLIB_CFLAGS) 

libstonering_la_SOURCES = 

libstonering_la_LIBADD = $(CLANLIB_LIBS) \
			abilities/libabilities.la \
			character/libcharacter.la \
			factories/libfactories.la \
			monsters/libmonsters.la \
			items/libitems.la \
			level/liblevel.la \
			party/libparty.la \
			core/libcore.la 



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
		-I@top_srcdir@/src/monsters \
		-I@top_srcdir@/src/abilities


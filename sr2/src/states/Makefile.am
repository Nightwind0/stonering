noinst_LTLIBRARIES = libstates.la

libstates_la_CPPFLAGS = $(CLANLIB_CFLAGS) $(CPPFLAGS)

libstates_la_SOURCES =  \
		ChoiceState.cpp \
		ChoiceState.h \
		MapState.cpp \
		MapState.h \
		SayState.cpp \
		SayState.h \
		State.h 

INCLUDES = 	-I@top_srcdir@/src/ \
		-I@top_srcdir@/src/core/ \
		-I@top_srcdir@/src/level \
		-I@top_srcdir@/src/level/actions \
		-I@top_srcdir@/src/level/checks \
		-I@top_srcdir@/src/abilities \
		-I@top_srcdir@/src/party \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/monsters \
		-I@top_srcdir@/src/items/weapons \
		-I@top_srcdir@/src/items/armor \
		-I@top_srcdir@/src/character

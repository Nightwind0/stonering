
SUBDIRS = LevelEditor

bin_PROGRAMS = stonering

stonering_SOURCES =	 Application.cpp \
			 Party.cpp 


noinst_LTLIBRARIES = libstonering.la

libstonering_la_SOURCES = Level.cpp \
			GraphicsManager.cpp

libstonering_la_CPPFLAGS = $(CLANLIB_CFLAGS)

stonering_CPPFLAGS = -O0

stonering_LDADD = $(CLANLIB_LIBS) \
			 libstonering.la	

stonering_CPPFLAGS = $(CLANLIB_CFLAGS)

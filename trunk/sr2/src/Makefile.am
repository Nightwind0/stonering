

bin_PROGRAMS = stonering

stonering_SOURCES =	 Application.cpp \
			 Party.cpp 


noinst_LTLIBRARIES = libstonering.la

libstonering_la_SOURCES = Level.cpp \
			GraphicsManager.cpp \
			LevelFactory.cpp 


libstonering_la_CPPFLAGS = $(CLANLIB_CFLAGS) \
				$(CPPFLAGS)


stonering_LDADD = $(CLANLIB_LIBS) \
			 libstonering.la	

stonering_CPPFLAGS = $(CLANLIB_CFLAGS) \
			$(CPPFLAGS)
stonering_CFLAGS = $(CLANLIB_CFLAGS) 



bin_PROGRAMS = stonering

stonering_SOURCES =	Application.cpp \
			Party.cpp \
			MapState.cpp	


noinst_LTLIBRARIES = libstonering.la

libstonering_la_SOURCES = \
			AbilityFactory.cpp \
			AbilityManager.cpp \
			Animation.cpp \
			CharacterClass.cpp \
			Element.cpp \
			GraphicsManager.cpp \
			Item.cpp \
			ItemFactory.cpp \
			ItemManager.cpp \
			Level.cpp \
			LevelElements.cpp \
			LevelFactory.cpp \
			MappableObject.cpp \
			Skill.cpp \
			Spell.cpp \
			StatusEffect.cpp \
			Utilities.cpp


libstonering_la_CPPFLAGS = $(CLANLIB_CFLAGS) \
				$(CPPFLAGS)


stonering_LDADD = $(CLANLIB_LIBS) \
			 libstonering.la	

stonering_CPPFLAGS = $(CLANLIB_CFLAGS) \
			$(CPPFLAGS)
stonering_CFLAGS = $(CLANLIB_CFLAGS) 

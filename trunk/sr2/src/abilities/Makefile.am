noinst_LTLIBRARIES = libabilities.la

libabilities_la_CPPFLAGS = $(CLANLIB_CFLAGS) $(CPPFLAGS)

libabilities_la_SOURCES = AbilityManager.cpp \
			AbilityManager.h \
			Effect.h \
			Skill.h \
			Skill.cpp \
			Spell.cpp \
			SpellRef.h \
			SpellRef.cpp \
			Spell.h \
			StatusEffect.h \
			StatusEffect.cpp

INCLUDES = 	-I@top_srcdir@/src/ \
		-I@top_srcdir@/src/character \
		-I@top_srcdir@/src/party \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/level \
		-I@top_srcdir@/src/level/actions \
		-I@top_srcdir@/src/level/checks \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/core


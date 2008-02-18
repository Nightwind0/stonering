lib_LTLIBRARIES = libsteel.la
bin_PROGRAMS = steel

libsteel_la_SOURCES = SteelParser.cpp \
			SteelScanner.cpp \
			SteelFunctor.cpp \
			SteelInterpreter.cpp \
			SteelException.cpp \
			Ast.cpp \
			SteelType.cpp \
			steel.trison \
			steel.reflex

libsteel_ladir = $(pkgincludedir)
libsteel_la_HEADERS =   Ast.h \
						SteelFunctor.h \
						SteelException.h \
						SteelType.h \
						SteelParser.h \
						SteelScanner.h \
						SteelInterpreter.h

SteelParser.cpp : steel.trison
	trison -S steel.states -o SteelParser -Hh steel.trison

SteelScanner.cpp : steel.reflex
	reflex  steel.reflex

func_test : SteelFunctor.h func_test.cpp SteelFunctor.cpp
	g++ -g func_test.cpp SteelFunctor.cpp -o func_test


steel_SOURCES = steel.cpp
steel_LDADD = libsteel.la

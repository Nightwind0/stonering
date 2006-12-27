noinst_LTLIBRARIES = libsteel.la

libsteel_la_SOURCES = SteelParser.cpp \
			SteelParser.h \
			SteelScanner.cpp \
			SteelScanner.h \
			steel.trison \
			steel.reflex

SteelParser.cpp : steel.trison
	trison -S steel.states -o SteelParser steel.trison

SteelScanner.cpp : steel.reflex
	reflex -o SteelScanner steel.reflex


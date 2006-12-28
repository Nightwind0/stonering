noinst_LTLIBRARIES = libsteel.la

libsteel_la_SOURCES = SteelParser.cpp \
			SteelParser.h \
			SteelScanner.cpp \
			SteelScanner.h \
			Ast.cpp \
			Ast.h \
			steel.trison \
			steel.reflex

SteelParser.cpp : steel.trison
	trison -S steel.states -o SteelParser steel.trison

SteelScanner.cpp : steel.reflex SteelParser.cpp
	reflex -o SteelScanner steel.reflex

test_parser : test_parser.cpp SteelParser.o SteelScanner.o Ast.o
	g++ -g test_parser.cpp SteelParser.o SteelScanner.o Ast.o -o test_parser

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(stonering, 0.0.1.1)
AC_CONFIG_SRCDIR(src/Application.cpp)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

#AC_CONFIG_HEADER([config.h])
AM_CONFIG_HEADER( config.h )
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
		


# AM_MAINTAINER_MODE
# Checks for libraries.
AC_CHECK_LIB(steel,main)


#PKG_CHECK_MODULES(CLANLIB, clanCore-2.1 clanApp-2.1 clanDisplay-2.1 clanGL-2.1 clanSound-2.1 clanGUI-2.1)
PKG_CHECK_MODULES(CLANLIB, clanCore-3.0 clanApp-3.0 clanDisplay-3.0 clanGL-3.0 clanSound-3.0 clanGUI-3.0)
#PKG_CHECK_MODULES(CLANLIB, clanCore-2.4 clanApp-2.4 clanDisplay-2.4 clanGL-2.4 clanSound-2.4 clanGUI-2.4 clanVorbis-2.4)

AC_SUBST([CLANLIB_LIBS]) 
AC_SUBST([CLANLIB_CFLAGS])


# Check for debug or release
AC_ARG_ENABLE(debug,[--enable-debug enables debug build],
				     [debugmode=$enableval],	
				     [debugmode=false]
)

AC_ARG_ENABLE(efence,[--enable-efence enables electric fence],
				      [efencemode=$enableval],
				      [efencemode=false]
)

CXXFLAGS="${CXXFLAGS} -Werror=return-type"

if test "$debugmode" != false 
then
     CXXFLAGS="${CXXFLAGS} -g3 -O0 -DDEBUG"
     CPPFLAGS="${CXXFLAGS} -g3 -O0 -DDEBUG"
     AC_MSG_RESULT(Debug Build)
else
     CXXFLAGS="${CXXFLAGS} -DNDEBUG -O2" 
     CPPFLAGS="${CXXFLAGS} -DNDEBUG -O2"
     AC_MSG_RESULT(Non-debug Build)
fi

if test "$efencemode" != false
then
	AC_CHECK_LIB(efence,main,[LIBS="$LIBS -lefence"])
	AC_MSG_RESULT(efence on)
else
	AC_MSG_RESULT(efence off)
fi

AC_ARG_ENABLE(editor, [--enable-editor creates an editor build],
		      editor=$enableval,editor=no)
if test "$editor" != no
then
	CFLAGS="-DSR2_EDITOR=1 ${CFLAGS}"
	CPPFLAGS="-DSR2_EDITOR=1 ${CPPFLAGS}"
	CXXFLAGS="${CPPFLAGS}"
	AC_MSG_RESULT(Editor Build)
else
	AC_MSG_RESULT(Non-editor Build)
fi

AC_ARG_ENABLE(gprof,
[       --enable-gprof          enable profiling with gprof],
gprof=$enableval,gprof=no)
if test "$gprof" != no
then
    CFLAGS="-pg $CFLAGS"
    CPPFLAGS="-pg $CPPFLAGS"
    CXXFLAGS="-pg $CXXFLAGS"
    AC_CHECK_LIB(gmon, main,[LIBS="$LIBS -lgmon"])
    AC_MSG_RESULT(gprof enabled)
else
    AC_MSG_RESULT(gprof disabled)
fi
				     
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_OUTPUT(Makefile)

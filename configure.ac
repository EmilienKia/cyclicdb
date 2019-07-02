dnl Process this file with autoconf to produce a configure script.

AC_INIT(cyclicdb, 0.1)

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([aux-bin])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])


AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11


LT_INIT

BOOST_REQUIRE
BOOST_PROGRAM_OPTIONS

dnl ***************************************************************************
dnl Documentation (Doxygen)
dnl ***************************************************************************

DX_INIT_DOXYGEN(cyclicdb, [Doxyfile], [doxygen])


AC_OUTPUT([
Makefile
src/Makefile
tests/Makefile
examples/Makefile
doc/Makefile
src/cyclicstore.pc
])

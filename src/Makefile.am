## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(CYCLICDB_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g


lib_LTLIBRARIES = libcyclicstore.la

libcyclicstore_la_SOURCES =  \
	common-type.hpp \
	common-base.hpp \
	common-base.cpp \
	common-file.hpp \
	common-file.cpp \
	libstore.hpp \
	libstore.cpp \
	libstore-base-impl.hpp \
	libstore-base-impl.cpp \
	libstore-mem-impl.hpp \
	libstore-mem-impl.cpp \
	libstore-file-impl.hpp \
	libstore-file-impl.cpp

bin_PROGRAMS = cyclicstore

cyclicstore_SOURCES = \
	store-parser-values.hpp \
	store-parser-commands.hpp \
	store-commands.hpp \
	store-commands.cpp \
	store.cpp

cyclicstore_CPPFLAGS = $(BOOST_CPPFLAGS)

cyclicstore_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

cyclicstore_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) libcyclicstore.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cyclicstore.pc

cyclicdbincludedir = $(includedir)/cyclicdb
cyclicdbinclude_HEADERS = \
	common-type.hpp \
	common-base.hpp \
	libstore.hpp

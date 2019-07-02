# cyclicdb: tests


TESTS = utests

check_PROGRAMS = $(TESTS)

utests_CXXFLAGS = -I../src/
utests_LDADD = ../src/libcyclicstore.la
utests_SOURCES = catch.hpp runner.cpp \
	test-common-type.cpp \
	test-mem-store.cpp \
	test-simple-store.cpp \
	test-store-parser-types.cpp \
	test-store-parser-commands.cpp \
	../src/store-commands.cpp

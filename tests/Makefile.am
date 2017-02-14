# cyclicdb: tests

# List of src files for CppUnit tests
CPPUNITTESTSRC = \
	test-common-type.cpp \
	test-mem-store.cpp \
	test-simple-store.cpp \
	test-store-parser-commands.cpp \
	test-store-parser-types.cpp

if HAVE_CPPUNIT

TESTS = cppunittest

check_PROGRAMS = $(TESTS)

cppunittest_CXXFLAGS = $(CPPUNIT_CFLAGS) -I../src/
cppunittest_LDFLAGS = $(CPPUNIT_LIBS)
cppunittest_LDADD = ../src/libcyclicstore.la

cppunittest_SOURCES = $(CPPUNITTESTSRC) cpputest.cpp \
	../src/store-commands.cpp


else !HAVE_CPPUNIT

EXTRA_DIST = example.cpp cpputest.cpp $(CPPUNITTESTSRC)

endif !HAVE_CPPUNIT

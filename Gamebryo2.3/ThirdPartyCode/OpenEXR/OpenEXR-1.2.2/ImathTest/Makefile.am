## Process this file with automake to produce Makefile.in

check_PROGRAMS = ImathTest

ImathTest_SOURCES = main.cpp testExtractEuler.cpp testExtractSHRT.cpp \
		    testFrustum.cpp testFun.cpp testInvert.cpp \
		    testMatrix.cpp testRandom.cpp testRoots.cpp \
		    testShear.cpp testColor.cpp testColor.h \
		    testExtractEuler.h testExtractSHRT.h testFrustum.h \
		    testFun.h testInvert.h testMatrix.h testRandom.h \
		    testRoots.h testShear.h

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/Imath -I$(top_srcdir)/Iex -I$(top_srcdir)/Half

LDADD = -L$(top_builddir)/Imath -L$(top_builddir)/Iex -L$(top_builddir)/Half \
	-lImath -lIex -lHalf

TESTS = ImathTest

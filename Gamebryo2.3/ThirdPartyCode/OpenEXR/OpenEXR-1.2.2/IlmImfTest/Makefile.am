## Process this file with automake to produce Makefile.in

check_PROGRAMS = IlmImfTest

IlmImfTest_SOURCES = main.cpp tmpDir.h testAttributes.cpp testChannels.cpp \
		     testCompression.cpp testCopyPixels.cpp \
		     testCustomAttributes.cpp testHuf.cpp testLineOrder.cpp \
		     testLut.cpp testRgba.cpp testSampleImages.cpp \
		     testWav.cpp testXdr.cpp testAttributes.h testChannels.h \
		     testCompression.h testCopyPixels.h \
		     testCustomAttributes.h testHuf.h testLineOrder.h \
		     testLut.h testRgba.h testSampleImages.h testWav.h \
		     testXdr.h testConversion.cpp testConversion.h \
		     testNativeFormat.cpp testNativeFormat.h \
		     testPreviewImage.cpp testPreviewImage.h \
		     testMagic.cpp testMagic.h testStandardAttributes.cpp \
		     testStandardAttributes.h testExistingStreams.cpp \
		     testExistingStreams.h testScanLineApi.cpp \
		     testScanLineApi.h testTiledCompression.cpp \
		     testTiledCompression.h testTiledCopyPixels.cpp \
		     testTiledCopyPixels.h testTiledLineOrder.cpp \
		     testTiledLineOrder.h testTiledRgba.cpp \
		     testTiledRgba.h compareFloat.h compareFloat.cpp \
		     testTiledYa.cpp testTiledYa.h \
		     testYca.cpp testYca.h \
		     testIsComplete.cpp testIsComplete.h

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/Half -I$(top_srcdir)/Iex \
	   -I$(top_srcdir)/Imath -I$(top_srcdir)/IlmImf

LDADD = -L$(top_builddir)/Iex -L$(top_builddir)/Half \
	-L$(top_builddir)/Imath -L$(top_builddir)/IlmImf \
	-lIlmImf -lImath -lIex -lHalf -lz

TESTS = IlmImfTest

EXTRA_DIST = comp_none.exr comp_piz.exr comp_rle.exr comp_zip.exr \
             comp_zips.exr test_native1.exr test_native2.exr invalid.exr \
	     tiled.exr

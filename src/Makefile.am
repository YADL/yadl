

# Setup libyadl sources here
lib_LTLIBRARIES = libyadl.la
libyadl_la_SOURCES = dedup.c Restore_file.c \
					 hash.c block.c stub.c clean_buff.c \
					 catalog.c md5.c sha1.c Rabin_Karp.c \
					 vector.c object_store.c namespace.c
libyadl_la_LDFLAGS = -lssl -lcrypto 

noinst_HEADERS = block.h catalog.h clean_buff.h \
				 config.h dedup.h \
				 hash.h main.h md5.h restore.h \
				 sha1.h stub.h Rabin_Karp.h \
				 vector.h object_store.h namespace.h

# Create a program called 'dedup' but do not install it
bin_PROGRAMS = yadl_dedup
yadl_dedup_SOURCES = main.c
yadl_dedup_CFLAGS = -O2 -g
yadl_dedup_LDADD = libyadl.la

# --- UNIT TESTS
#  Initialize variables
CLEANFILES = *_xunit.xml
TESTS = 
noinst_PROGRAMS = 

# dedup_test
dedup_test_CFLAGS = $(UNITTEST_CFLAGS)
dedup_test_LDFLAGS = $(UNITTEST_LIBS)
dedup_test_SOURCES = dedup_test.c #dedup.c dedup.h
TESTS += dedup_test

# delete_test
delete_test_CFLAGS = $(UNITTEST_CFLAGS)
delete_test_LDFLAGS = $(UNITTEST_LIBS)
delete_test_SOURCES = delete_test.c #delete.c delete.h
TESTS += delete_test

# --- End UNIT TEST

# Make TESTS be programs which are not installed
noinst_PROGRAMS += $(TESTS)

# Here we place the exported header
#yadlincludedir = $(includedir)/yadl
#yadlinclude_HEADERS = yadl.h

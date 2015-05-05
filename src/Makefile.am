# Setup libyadl sources here
libyadl_la_SOURCES = dedup.c delete.c Restore_file.c \
					 hash.c block.c stub.c clean_buff.c \
					 catalog.c md5.c sha1.c 
libyadl_la_LDFLAGS = -lssl -lcrypto 
lib_LTLIBRARIES = libyadl.la

# Create a program called 'dedup' but do not install it
bin_PROGRAMS = yadl_dedup
yadl_dedup_SOURCES = main.c
yadl_dedup_CFLAGS = -O2 -g
yadl_dedup_LDADD = libyadl.la

# Here we place the exported header
#yadlincludedir = $(includedir)/yadl
#yadlinclude_HEADERS = yadl.h

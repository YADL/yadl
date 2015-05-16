
lib_LTLIBRARIES = liblog.la
liblog_la_SOURCES = log_yadl.c

liblog_la_LDFLAGS = -lssl -lcrypto 

noinst_HEADERS = log_yadl.h

bin_PROGRAMS = log_yadl
log_yadl_SOURCES = log_yadl.c
log_yadl_CFLAGS = -O2 -g -c
log_yadl_LDADD = liblog.la


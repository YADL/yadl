log:log_yadl.o liblog.a
log_yadl.o:log_yadl.c log_yadl.h
	gcc $(CFLAGS) -w -c -g log_yadl.c -l log_yadl.h
liblog.a:log_yadl.o
	ar cr liblog.a log_yadl.o
clean:
	rm -f log_yadl.o liblog.a

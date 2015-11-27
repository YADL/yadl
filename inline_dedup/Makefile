test:ydl_namespace.o ydl_clean_buff.o libydl.a client
ydl_clean_buff.o:ydl_clean_buff.c ydl_clean_buff.h
	gcc -g -c ydl_clean_buff.c -l ydl_clean_buff.h
ydl_namespace.o:ydl_namespace.c ydl_namespace.h
	gcc -g -c ydl_namespace.c -l ydl_namespace.h
libydl.a:
	ar cr libydl.a ydl_clean_buff.o ydl_namespace.o
client:
	gcc -g -o client client.c -lleveldb -L. -lydl
clean:
	rm -f ydl_clean_buff.o ydl_namespace.o libydl.a client
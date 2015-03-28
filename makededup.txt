test:md5.o sha1.o delete.o hash.o block.o stub.o catalog.o dedup.o Restore_file.o dedup libdedup.a
dedup.o:dedup.c dedup.h
	gcc -c dedup.c -lssl -lcrypto 
delete.o:delete.c delete.h
	gcc -c delete.c
Restore_file.o:Restore_file.c restore.h
	gcc -c Restore_file.c
hash.o:hash.c hash.h
	gcc -c hash.c
block.o:block.c block.h
	gcc -c block.c
stub.o:stub.c stub.h
	gcc -c stub.c
catalog.o:catalog.c catalog.h
	gcc -c catalog.c
md5.o:md5.c md5.h
	gcc -c md5.c -lssl -lcrypto
sha1.o:sha1.c sha1.h
	gcc -c sha1.c -lssl -lcrypto
libdedup.a:md5.o sha1.o delete.o hash.o block.o stub.o catalog.o dedup.o Restore_file.o
	ar cr libdedup.a md5.o sha1.o delete.o hash.o block.o stub.o catalog.o dedup.o Restore_file.o
dedup:main.c libdedup.a
	gcc -g main.c -o dedup -L. -ldedup -lssl -lcrypto
clean:
	rm -f *.c

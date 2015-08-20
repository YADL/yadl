test:vector.o md5.o Rabin_Karp.o clean_buff.o object_store.o parsing.o stub.o ldb.o minhash libminhash.a
vector.o:vector.c vector.h
	gcc -c vector.c
md5.o:md5.c md5.h
	gcc -c md5.c -lssl -lcrypto
Rabin_Karp.o:Rabin_Karp.c Rabin_Karp.h
	gcc -c Rabin_Karp.c
clean_buff.o:clean_buff.c clean_buff.h
	gcc -c clean_buff.c
parsing.o:parsing.c parsing.h
	gcc -c parsing.c
stub.o:stub.c stub.h
	gcc -c stub.c
object_store.o:object_store.c object_store.h
	gcc -c object_store.c
ldb.o:ldb.c ldb.h
	gcc -c ldb.c -lleveldb
libminhash.a:md5.o Rabin_Karp.o md5.o Rabin_Karp.o clean_buff.o vector.o parsing.o vector.o object_store.o stub.o ldb.o
	ar cr libminhash.a md5.o Rabin_Karp.o clean_buff.o vector.o parsing.o vector.o object_store.o stub.o ldb.o
minhash:main.c libminhash.a
	gcc -g main.c -o min_hash -L. -lminhash -lssl -lcrypto -lleveldb

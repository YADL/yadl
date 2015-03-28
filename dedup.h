#include<stdio.h>
#include<string.h>  
#include<stdlib.h> 
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<ctype.h> 
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<time.h> 
#include <libgen.h> 
#include<openssl/md5.h>
#if defined(Srinivas)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#include <linux/limits.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif
#define block 10
int selecthash(char[],int,int);
int readTemp(int);
int searchhash(char *out,int filedes);
int readBlockTemp(int filedes);
int write_hash(char buff[],int filedes,int offset);
int write_to_block(char buff[],size_t l,int filedes);
int writecatalog(int fc,char* argv);
int readcatalog(int fc);
int searchpath(char out[],int filedes);
int file_exist(char *filename);
int write_to_stub(char buff[],size_t l,int filedes,int b_offset,int e_offset);
char* get_hash(int  st1,int b_offset,int e_offset);
int searchBlock(int filedes,int filedes1,int st1,int b_offset,int  e_offset);
int getposition(int filedes,char* hash);
char* getblock(int filedes1,int pos);
int searchhashstub(int  st1,int b_offset,int e_offset);
int  searchstubhash(int  st1,int b_offset,int e_offset);
char* real_path1();

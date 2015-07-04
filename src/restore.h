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
#if defined(CFLAG)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#include <linux/limits.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif
#define block 10
#define NAME_SIZE 100
#define int_size sizeof(int)
#define FILE_SIZE  200

/*@description:Function to get the position of specific block in hash
@in: char* hash-hash
@out: int 
@return: -1 for error and 0 if found. */
int getposition(char* hash);

/*@description:Function to restore file.  
@in: char* path-path of file to be restored
@out: int
@return: -1 for error and 0 if found. */
int restorefile(char* path);

/*@description:Function to search whether file path is present or not.If present will call restorefile to restore file. 
@in: void
@out: int
@return: -1 for error and 0 if found. */
int restore_file();

/*@description:Function to get specific block from specified position 
@in: int pos-position of block,
@out: char*
@return: block */
char* get_block(int pos, int *length);

/*@description:Function to get specific block from object
@in: char *hash - hash of block
@out: char*
@return: block */
char *get_block_from_object(char *hash,int *length);

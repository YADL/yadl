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

/*enum to define hash types used for hashing*/
enum hash_type
{
	sha_1    =   1,
	sha_256,
	md_5
};

/*@description:Function to create hashstore
@in: void
@out: int 
@return: -1 for error and 0 if created successfully */
int init_hash_store();

/*@description:Function to insert hash to hashstore
@in: char *buff-buffer that contains hash,int offset-starting position of block
@out: int 
@return: -1 for error and 0 if inserted successfully */
int insert_hash(char *buff,int offset);

/*@description:Function to check whether hash is already present or not.
@in: char *out-input hash
@out: int hash
@return: -1 for error and 0 if hash already present */
int searchhash(char *out);

/*@description:Function to get the position of specific block in hash
@in: char* hash-hash
@out: int 
@return: -1 for error and 0 if found. */
int getposition(char* hash);

/*@description:Function to create blockstore
@in: void
@out: int 
@return: -1 for error and 0 if closed successfully */
int fini_hash_store();

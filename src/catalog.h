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


/*@description:Function to create catalogstore
@in: void
@out: int 
@return: -1 for error and 0 if created successfully */
int init_catalog_store();

/*@description:Function to write the full path of file to catalog
@in: char* filename-filename of file that has been deduped
@out: int 
@return: -1 for error and 0 if inserted successfully */
int writecatalog(char* filename);

/*@description:Function to read all deduped files from a catalog file.
@in: void
@out: int 
@return: -1 for error and 0 if created successfully */
int readfilecatalog();

/*@description:Function to compare absolute path of file in file catalog.
@in: char out[]-path of file.
@out: int 
@return: -1 for error and 0 if created successfully */
int comparepath(char out[]);

/*@description:Function to close filedescriptor of catalogstore
@in: void
@out: int 
@return: -1 for error and 0 if closed successfully */
int fini_catalog_store();

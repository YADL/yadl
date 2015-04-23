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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include<cmockery/pbc.h>
#include <cmockery/cmockery.h>
#include <cmockery/cmockery_override.h>
#define block 10
#define NAME_SIZE 100
#define int_size sizeof(int)
#include "clean_buff.h"

/*@description:Function to write hash,beginning offset and ending offset of block to stub
@in: char *buff-buffer containing hash,size_t l-length of hash,int filedes-file descriptor of stub,int b_offset-beginning offset of block,int e_offset-ending offset of bloc
@out: int 
@return: -1 for error and 0 if found. */
int 
write_to_stub(char buff[],size_t l,int fd_stub,int b_offset,int e_offset);

/*@description:Function to check single instance of block of specified position
@in: int  fd_stub-filedescriptor of stub,int b_offset-beginning offset of block,int e_offset-ending offset of block
@out: int 
@return: -1 for error and 0 if found. */
int 
searchstubhash(int fd_stub,int b_offset,int e_offset);

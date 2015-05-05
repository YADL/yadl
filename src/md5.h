#include <openssl/sha.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h> 
#include<openssl/md5.h>
#if defined(CFLAG)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif

/*@description:Function to create hash using md5.
@in: char *str-Buffer to store block for which the hash is generated,int length-length of block
@out: char*-returns hash of block using md5
@return: hash */
char *str2md5(const char *str, int length) ;

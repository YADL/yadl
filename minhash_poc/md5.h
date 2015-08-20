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

typedef struct vector *vector_ptr;

typedef unsigned char DIGEST;

/*Function to implement md5.
Input:char *buffer - String to be encrypted
Output:char* - 16 byte char.
*/
DIGEST *str2md5(vector_ptr list);

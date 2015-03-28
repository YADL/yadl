#include <openssl/sha.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h> 
#include<openssl/md5.h>
#if defined(Srinivas)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif
char *str2md5(const char *str, int length) ;

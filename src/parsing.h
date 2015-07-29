#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <openssl/md5.h>
#include <stdlib.h>
#include <openssl/sha.h>
#if defined(CFLAG)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif

typedef unsigned char DIGEST;

/*Function to implement md5.
Input:char *buffer - String to be encrypted
Output:char* - 16 byte char.
*/
DIGEST *str2md5(char *buffer);

/*
 * Function to convert hexadecimal digit to integer.
 * Input:char -- 
 * Output:int -- 
 */
int hex_to_int(char digit);

/*
 * Function to parse the md5  16 byte char to 32 byte hexadecimal.
 * Input:DIGEST -- 16 byte char.
 * Output:char *str -- md5 32 byte hexadecimal
 */
char *parse(DIGEST *digest);

/*
 * Function to unparse the md5 32 byte hexadecimal to 16 byte char.
 * Input:char *str -- md5 32 byte hexadecimal
 * Output:DIGEST -- 16 byte char.
 */
DIGEST *unparse(char *str);
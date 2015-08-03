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

/*Function to create hash using sha1
Input:char *buffer
Output:DIGEST
*/
DIGEST *sha1(char *buffer);

/*
 * Function to convert hexadecimal digit to integer.
 * Input:char digit
 * Output:int hex_digit
 */
int hex_to_int(char digit);

/*
 * Function to parse the hash byte char to hexadecimal hash value.
 * Input:DIGEST -- byte char of hash.
 * Output:char *str -- Hexadecimal hash value
 */
char *parse(DIGEST *digest, int len);

/*
 * Function to unparse the hexadecimal to byte char.
 * Input:char *str -- Hexadecimal hash value
 * Output:DIGEST -- byte char of hash.
 */
DIGEST *unparse(char *str, int len);
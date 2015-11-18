#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>
#include "md5.h"

typedef unsigned char DIGEST;
/*
 * Function to convert hexadecimal digit to integer.
 * Input:char digit
 * Output:int hex_digit
 */
int hex_to_int(char digit);

/*
 * Function to parse the hash byte char to hexadecimal hash value.
 * Input:DIGEST -- byte char of hash.
 * Output:char *str -- Hexadecimal hash value.
 */
char *parse(DIGEST *digest, int len);

/*
 * Function to unparse the hexadecimal to byte char.
 * Input:char *str -- Hexadecimal hash value
 * Output:DIGEST -- byte char of hash.
 */
DIGEST *unparse(char *str, int len);

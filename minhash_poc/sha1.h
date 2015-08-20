#include<openssl/sha.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

/*Function to create hash using sha1
Input:char *buffer
Output:DIGEST
*/
DIGEST *sha1(char *buffer);
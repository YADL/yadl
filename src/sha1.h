#include<openssl/sha.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include "vector.h"

/*@description:Function to create hash using sha1.
@in: char str[]-Buffer to store block for which the hash is generated,int length-length of block
@out: char*-returns hash of block using sha1
@return: hash */
char* sha1(vector_ptr list);

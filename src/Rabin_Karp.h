#include<stdio.h>
#include<string.h>
#include<fcntl.h>  
#include<stdlib.h> 
#include<errno.h> 
#include<error.h>
#include<sys/stat.h>
#include<unistd.h>

#define PRIME 27
#define N 128
#define FINGER_PRINT 34
#define BUFFER_LEN 1024

typedef unsigned int y_uint32;

/*@description:Function to find the rolling hash of a particular window.
Input:
        char* buffer            : Buffer to store window content
        ssize_t length          : Stores the length of the window
        int *ret                : To return the status 0 on success, 
         -1 on failure
Output:
        y_uint32 hash_value     : Rolling hash of a particular window
*/
y_uint32 calc_hash (char *buffer, ssize_t length, int *ret);

/*@description:Function to generate variable size chunk using rabin-karp.
Input:
        char* filename  : Name of the file to be chunked 
        int *ret        : Pointer to return 0 on success, -1 on failure
        int *size       : Poniter to return remaining size of the file
Output:
        char*           : Chunk to be returned
*/
char* get_variable_chunk (int fd,int *ret,int *size);


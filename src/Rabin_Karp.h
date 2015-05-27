#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<error.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

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

/*@description:Function to keep track remaining content of previous
 buffer when there is no match with fingerprint

Input:
        ssize_t remaining_length       : Remaining length of the
                                        previous buffer
        char** remaining_buffer_content: Contains remaining content of
                                        the previous buffer
        char** remaining_window_content: Contains remaining content of
                                        the previous window
        char** buffer                  : Contains current buffer content
        ssize_t start                  : Starting offset of buffer
        ssize_t wstart                 : Starting offset of window
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
get_remaining_buffer_content(char **remaining_buffer_content,
        char **remaining_window_content, ssize_t remaining_length,
        char **buffer, ssize_t start, ssize_t wstart);

/*@description:Function to get the chunk when there is a match with
 fingerprint
Input:
        ssize_t* remaining_length      : Remaining length of the
                                        previous buffer
        char** remaining_buffer_content: Contains remaining content of
                                        the previous buffer
        char** remaining_window_content: Contains remaining content of
                                        the previous window
        char** chunk_buffer            : Holds the chunk content
        char** buffer                  : Contains current buffer content
        ssize_t start                  : Starting offset of buffer
        ssize_t end                    : Ending offset of buffer
        ssize_t wstart                 : Starting offset of window
        ssize_t slide_incr             : Keeps track of buffer sliding
        ssize_t* remaining_content_incr: Keeps track of sliding of
                                        previous buffer
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
get_chunk_buffer(ssize_t *remaining_content_incr, ssize_t *remaining_length,
        char **chunk_buffer, char **buffer, char **remaining_buffer_content,
        char **remaining_window_content, ssize_t start, ssize_t end,
        ssize_t wstart, ssize_t slide_incr);

/*@description:Function to generate variable size chunk using rabin-karp.
Input:
        char* filename  : Name of the file to be chunked
        int *ret        : Pointer to return 0 on success, -1 on failure
        int *size       : Poniter to return remaining size of the file
Output:
        char*           : Chunk to be returned
*/
char *get_variable_chunk (int fd, int *ret, int *size);


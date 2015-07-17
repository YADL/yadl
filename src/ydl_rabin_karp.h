#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<error.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include "clean_buff.h"

#define PRIME 23
#define FINGER_PRINT 938
#define BUFFER_LEN 10240
#define M 1021
#define WINDOW_SIZE 2048

typedef unsigned int y_uint32;

class ydl_rabin_karp
{
public:
        y_uint32 calc_hash (char *buffer, y_uint32 *power, int *ret);
        int get_remaining_buffer_content(char **remaining_buffer_content,
                                        char **remaining_window_content,
                                        ssize_t remaining_length,
                                        char **buffer,
                                        ssize_t start);
        int get_chunk_buffer(ssize_t *remaining_content_incr,
                             ssize_t *remaining_length,
                             char **chunk_buffer, char **buffer,
                             char **remaining_buffer_content,
                             char **remaining_window_content,
                             ssize_t start, ssize_t end,
                             ssize_t slide_incr, int *chunk_length);
        char *get_variable_chunk (int fd, int *ret, int *size,
                                  int *chunk_flag, int *chunk_length);
};
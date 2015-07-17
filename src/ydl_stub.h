#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<ctype.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#define INT_SIZE sizeof(int)

class ydl_stub
{
public:
        int write_to_stub( char *buff,size_t length,int fd_stub,
                           int b_offset,int e_offset);
};
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
#include "clean_buff.h"
#define INT_SIZE sizeof(int)

class ydl_hash
{
        static int hash_fd;
public:
        int init_hash_store();
        int insert_hash(char *buff,int offset);
        int searchhash(char *out);
        int getposition(char* hash);
        int fini_hash_store();
};

int ydl_hash::hash_fd;
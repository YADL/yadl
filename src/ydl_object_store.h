#include "vector.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>


class ydl_object_store
{
public:
        int insert_block_to_object(char *hash,vector_ptr list);
        char *get_block_from_object(char *hash,int *length);
};